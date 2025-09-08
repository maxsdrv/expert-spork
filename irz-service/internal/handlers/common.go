package handlers

import (
	"context"

	"connectrpc.com/connect"
	"github.com/opticoder/ctx-log/go/ctx_log"
	"google.golang.org/protobuf/proto"
	"google.golang.org/protobuf/types/known/emptypb"

	"dds-provider/internal/generated/api/proto"
)

var logging = ctx_log.GetLogger(nil)

func (s *Handlers) Settings(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
) (*connect.Response[apiv1.SettingsResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	settings := &apiv1.Settings{
		DeviceIdMapping: []*apiv1.DeviceIdMapping{
			{
				DeviceId:    proto.String("device-001"),
				Fingerprint: proto.String("fp-12345"),
				Class:       proto.String("jammer"),
			},
		},
		Devices: []*apiv1.DeviceSettings{
			{
				DeviceId:     proto.String("device-001"),
				Disabled:     proto.Bool(false),
				PositionMode: apiv1.GeoPositionMode_GEO_AUTO.Enum(),
				Position: &apiv1.GeoPosition{
					Azimuth: proto.Float32(0.0),
					Coordinate: &apiv1.GeoCoordinate{
						Latitude:  proto.Float64(55.7558),
						Longitude: proto.Float64(37.6173),
						Altitude:  proto.Float32(120.0),
					},
				},
			},
		},
	}
	return connect.NewResponse(&apiv1.SettingsResponse{Settings: settings}), nil
}

func (s *Handlers) LicenseStatus(
	ctx context.Context,
	req *connect.Request[emptypb.Empty],
) (*connect.Response[apiv1.LicenseStatusResponse], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&apiv1.LicenseStatusResponse{
		Valid:       proto.Bool(false),
		Description: proto.String("No valid license found"),
		ExpiryDate:  proto.String("01.01.2025"),
		Signature:   proto.String("1234567890"),
	}), nil
}

func (s *Handlers) UploadLicense(
	ctx context.Context,
	req *connect.Request[apiv1.UploadLicenseRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&emptypb.Empty{}), nil
}

func (s *Handlers) SetSettings(
	ctx context.Context,
	req *connect.Request[apiv1.SetSettingsRequest],
) (*connect.Response[emptypb.Empty], error) {
	logger := logging.WithCtxFields(ctx)
	logger.Debug("Request data: ", req.Msg)

	return connect.NewResponse(&emptypb.Empty{}), nil
}
