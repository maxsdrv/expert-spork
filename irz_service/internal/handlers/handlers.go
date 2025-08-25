package handlers

import (
	"dds-provider/internal/controllers"
)

type Handlers struct {
	controllers *controllers.Controllers
}

func New(controllers *controllers.Controllers) *Handlers {
	return &Handlers{controllers: controllers}
}
