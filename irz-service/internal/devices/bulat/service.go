package bulat

import (
	"context"
	"encoding/json"
	"io"
	"net/http"
	"strings"

	api "dds-provider/internal/generated/bulat"
)

type Service struct {
	client      *api.APIClient
	authToken   string
	pspToken    string
	userType    int32
	permissions map[string]any
}

func New(ctx context.Context, client *api.APIClient, token string) (*Service, error) {
	logger := logging.WithCtxFields(ctx)

	if client != nil && client.GetConfig() != nil {
		cfg := client.GetConfig()
		if len(cfg.Servers) > 0 {
			base := cfg.Servers[0].URL
			if !strings.HasPrefix(base, "https://") {
				cfg.Servers[0].URL = "https://" + base
				logger.Debugf("Bulat base URL lacked scheme, normalized to %s", cfg.Servers[0].URL)
			}
		}
	}

	svc := &Service{
		client:    client,
		authToken: token,
	}

	if err := svc.Authenticate(ctx); err != nil {
		logger.Errorf("Authentication failed: %v", err)
		return nil, err
	}
	logger.Infof("Authentication successful, userType=%d", svc.userType)
	return svc, nil
}

func (s *Service) doRequest(ctx context.Context, operation string, payload map[string]any) ([]byte, error) {
	logger := logging.WithCtxFields(ctx)

	raw, err := json.Marshal(payload)
	if err != nil {
		logger.Errorf("Failed to marshal payload: %v", err)
		return nil, err
	}

	var httpResp *http.Response

	switch operation {
	case loginAction:
		_, httpResp, _ = s.client.DefaultAPI.LoginWithToken(ctx).ReqJson(string(raw)).Execute()
	default:
		_, httpResp, _ = s.client.DefaultAPI.DetectorMonitorActions(ctx).ReqJson(string(raw)).Execute()
	}
	defer httpResp.Body.Close()

	body, err := io.ReadAll(httpResp.Body)
	if err != nil {
		logger.Errorf("Failed to read body: %v", err)
		return nil, err
	}

	if err = s.HandleAPIResponse(body); err != nil {
		return nil, err
	}

	return body, nil
}
