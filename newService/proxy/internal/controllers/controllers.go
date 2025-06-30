package controllers

import (
	"dds-provider/proxy/internal/services/target_provider"
)

type Controllers struct {
	svcTargetProvider *target_provider.Service
}

func New(svcTargetProvider *target_provider.Service) *Controllers {
	return &Controllers{
		svcTargetProvider: svcTargetProvider,
	}
}
