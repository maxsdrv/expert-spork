package core

import (
	"strings"

	"github.com/google/uuid"
)

type DeviceId = EntityId
type TargetId = EntityId
type AlarmId = EntityId

type EntityId struct {
	id string
}

func NewId(uuid_arg string) EntityId {
	trimmed := strings.TrimSpace(uuid_arg)
	return EntityId{id: trimmed}
}

func GenerateId() EntityId {
	id := uuid.New()
	return EntityId{id: id.String()}
}

func (d EntityId) String() string {
	return d.id
}

func (d EntityId) Validate() error {
	if err := uuid.Validate(d.id); err != nil {
		return coreError("validate uuid: %v: id string: %s", err, d.id)
	}
	return nil
}

func (d EntityId) Equal(other EntityId) bool {
	return d.id == other.String()
}
