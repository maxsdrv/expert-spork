package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
	"google.golang.org/protobuf/types/known/timestamppb"
)

type JammerTimeoutStatus apiv1.JammerTimeoutStatus

// use
// t, err := time.Parse(time.RFC3339, "2023-05-02T09:34:01Z")
//	ts := timestamppb.New(t)

func NewJammerTimeoutStatus(started *timestamppb.Timestamp, now *timestamppb.Timestamp, duration int32) JammerTimeoutStatus {
	return (JammerTimeoutStatus)(apiv1.JammerTimeoutStatus{
		Started:  started,
		Now:      now,
		Duration: &duration,
	})
}
