package metrics

import (
	"context"
	"net/http"
	"strconv"
	"strings"
	"sync"
	"time"

	"github.com/opticoder/ctx-log/go/ctx_log"
	"github.com/prometheus/client_model/go"
	"github.com/prometheus/common/expfmt"

	"dds-control-api/internal/generated/api"
)

const (
	cpuSecondsNode = "node_cpu_seconds_total"
	cpuLabel       = "cpu"
	modeLabel      = "mode"
	idleLabel      = "idle"

	freeDiskSpaceNode = "node_filesystem_avail_bytes"
	mountpointLabel   = "mountpoint"
	diskMountpoint    = "/"

	availableMemoryNode = "node_memory_MemAvailable_bytes"
)

var logging = ctx_log.GetLogger(nil)

type Service struct {
	url         string
	mutex       sync.Mutex
	metrics     api.SystemMetrics
	cpuCount    int
	idleSeconds []float64
}

const (
	pollIntervalSec           = 1
	cpuLoadAveragingPeriodSec = 10
	maxIdleSecondsSize        = cpuLoadAveragingPeriodSec / pollIntervalSec
)

func New(ctx context.Context, host string) *Service {
	svc := &Service{
		url:         "http://" + host + ":9100/metrics",
		cpuCount:    1,
		idleSeconds: make([]float64, maxIdleSecondsSize),
	}

	go svc.updateData(ctx)
	return svc
}

func (s *Service) Get() api.SystemMetrics {
	s.mutex.Lock()
	defer s.mutex.Unlock()
	return s.metrics
}

func (s *Service) updateData(ctx context.Context) {
	logger := logging.WithCtxFields(ctx)

	ticker := time.NewTicker(pollIntervalSec * time.Second)

	for _ = range ticker.C {
		metricFamilies, err := s.readMetrics(ctx)
		if err != nil {
			logger.WithError(err).Warn("Error reading metrics")
			continue
		}
		s.parseMetrics(ctx, metricFamilies)
	}
}

func (s *Service) readMetrics(ctx context.Context) (map[string]*io_prometheus_client.MetricFamily, error) {
	logger := logging.WithCtxFields(ctx)

	logger.Trace("Getting metrics")

	reader := strings.NewReader("")
	request, err := http.NewRequest("GET", s.url, reader)
	if err != nil {
		return nil, err
	}
	client := &http.Client{}
	response, err := client.Do(request)
	if err != nil {
		return nil, err
	}
	var parser expfmt.TextParser
	metricFamilies, err := parser.TextToMetricFamilies(response.Body)
	if err != nil {
		return nil, err
	}
	logger.Trace("Metrics received")
	return metricFamilies, nil
}

func (s *Service) parseMetrics(ctx context.Context, metricFamilies map[string]*io_prometheus_client.MetricFamily) {
	logger := logging.WithCtxFields(ctx)

	for metricNode := range metricFamilies {
		if metricNode == cpuSecondsNode {
			var secondsTotal float64 = 0
			for _, metric := range metricFamilies[metricNode].Metric {
				for _, label := range metric.Label {
					if *label.Name == cpuLabel {
						cpuIndex, _ := strconv.Atoi(*label.Value)
						if cpuIndex >= s.cpuCount {
							s.cpuCount = cpuIndex + 1
						}
					}
					if *label.Name == modeLabel && *label.Value == idleLabel {
						secondsTotal += *metric.Counter.Value
					}
				}
			}
			s.idleSeconds = s.idleSeconds[1:]
			s.idleSeconds = append(s.idleSeconds, secondsTotal)
			logger.Trace("CPU seconds data processed")
		}
		if metricNode == freeDiskSpaceNode {
			for _, metric := range metricFamilies[metricNode].Metric {
				for _, label := range metric.Label {
					if *label.Name == mountpointLabel && *label.Value == diskMountpoint {
						s.mutex.Lock()
						s.metrics.Disk = int64(*metric.Gauge.Value)
						s.mutex.Unlock()
						logger.Trace("Free disk space data updated")
					}
				}
			}
		}
		if metricNode == availableMemoryNode {
			s.mutex.Lock()
			s.metrics.Memory = int64(*metricFamilies[metricNode].Metric[0].Gauge.Value)
			s.mutex.Unlock()
			logger.Trace("Free memory data updated")
		}
	}
	idleSecondsChangePerSecRate := (s.idleSeconds[len(s.idleSeconds)-1] - s.idleSeconds[0]) / float64(len(s.idleSeconds))
	s.mutex.Lock()
	s.metrics.Cpu = int32(100 * (1 - idleSecondsChangePerSecRate/float64(s.cpuCount)))
	s.mutex.Unlock()
	logger.Trace("CPU load data updated")
}
