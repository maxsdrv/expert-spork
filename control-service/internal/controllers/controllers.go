package controllers

import (
	"dds-control-api/internal/services/logs"
	"dds-control-api/internal/services/metrics"
	"dds-control-api/internal/services/update"
	"dds-control-api/internal/services/versions"
)

type Controllers struct {
	svcUpdate   update.Service
	svcVersions *versions.Service
	svcMetrics  *metrics.Service
	svcLogs     *logs.Service
}

func New(svcUpdate update.Service, svcVersions *versions.Service, svcMetrics *metrics.Service, svcLogs *logs.Service) *Controllers {
	return &Controllers{
		svcUpdate:   svcUpdate,
		svcVersions: svcVersions,
		svcMetrics:  svcMetrics,
		svcLogs:     svcLogs,
	}
}
