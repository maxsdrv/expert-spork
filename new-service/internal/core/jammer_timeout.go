package core

import (
	"time"

	apiv1 "dds-provider/internal/generated/api/proto"
	"google.golang.org/protobuf/proto"
	"google.golang.org/protobuf/types/known/timestamppb"
)

// t, err := time.Parse(time.RFC3339, "2023-05-02T09:34:01Z")

type JammerTimeoutStatus struct {
	Started  time.Time
	Now      time.Time
	Duration time.Duration
}

func (j *JammerTimeoutStatus) ToAPI() *apiv1.JammerTimeoutStatus {
	return &apiv1.JammerTimeoutStatus{
		Started:  timestamppb.New(j.Started),
		Now:      timestamppb.New(j.Now),
		Duration: proto.Int32(int32(j.Duration)),
	}
}
