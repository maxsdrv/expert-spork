package core

import (
	"fmt"
	"strings"

	"github.com/google/uuid"
)

type DeviceId = EntityId
type TargetId = EntityId
type AlarmId = EntityId

type EntityId struct {
	id string
}

func NewId(uuid_arg string) (EntityId, error) {
	trimmed := strings.TrimSpace(uuid_arg)
	if err := uuid.Validate(trimmed); err != nil {
		return EntityId{}, fmt.Errorf("new entity id: %w: %s", err, trimmed)
	}
	return EntityId{id: trimmed}, nil
}

func GenerateId() EntityId {
	id := uuid.New()
	return EntityId{id: id.String()}
}

func (d EntityId) String() string {
	return d.id
}

func (d EntityId) Equal(other EntityId) bool {
	return d.id == other.String()
}
