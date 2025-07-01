package core

import (
	"fmt"
	"strings"

	"github.com/google/uuid"
)

type EntityId interface {
	String() string
	Equal(other EntityId) bool

	isEntityId()
}

type DeviceId EntityId
type TargetId EntityId
type AlarmId EntityId

type entityId struct {
	value string
}

func NewId(value string) (EntityId, error) {
	trimmed := strings.TrimSpace(value)
	if err := uuid.Validate(trimmed); err != nil {
		return nil, fmt.Errorf("new entity id: %w: %s", err, trimmed)
	}
	return &entityId{value: trimmed}, nil
}

func GenerateId() EntityId {
	id := uuid.New()
	return &entityId{value: id.String()}
}

func (d *entityId) String() string {
	if d == nil {
		return ""
	}
	return d.value
}

func (d *entityId) Equal(other EntityId) bool {
	if d == nil && other == nil {
		return true
	}
	if d == nil || other == nil {
		return false
	}
	return d.value == other.String()
}

func (d *entityId) isEntityId() {}
