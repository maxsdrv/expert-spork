package handlers

import (
	"dds-provider/internal/controllers"
)

type Handlers struct {
	controllers *controllers.Controllers
}

func NewHandlers(controllers *controllers.Controllers) *Handlers {
	return &Handlers{controllers: controllers}
}
