package controllers

import (
	"dds-provider/proxy/internal/services/radariq"
)

type Controllers struct {
	svcRadarIq *radariq.Service
}

func New(svcRadarIq *radariq.Service) *Controllers {
	return &Controllers{
		svcRadarIq: svcRadarIq,
	}
}
