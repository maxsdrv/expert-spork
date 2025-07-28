package httpclient

import (
	"net/http"
	"time"

	"dds-provider/internal/generated/radariq-client/dss_target_service"
)

type SharedHttpClient struct {
	client *http.Client
}

func NewSharedHttpClient() *SharedHttpClient {
	client := &http.Client{
		Timeout: 30 * time.Second,
		Transport: &http.Transport{
			MaxIdleConns:        10,
			IdleConnTimeout:     60 * time.Second,
			DisableCompression:  false,
			DisableKeepAlives:   false,
			MaxIdleConnsPerHost: 10,
		},
	}

	return &SharedHttpClient{
		client: client,
	}
}

func (s *SharedHttpClient) CreateAPIConfiguration(baseUrl string) *dss_target_service.Configuration {
	config := dss_target_service.NewConfiguration()
	config.Servers = dss_target_service.ServerConfigurations{
		dss_target_service.ServerConfiguration{URL: baseUrl},
	}
	config.HTTPClient = s.client
	return config
}

func (s *SharedHttpClient) GetHttpClient() *http.Client {
	return s.client
}
