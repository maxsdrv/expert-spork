package bulat

import (
	"context"
	"encoding/json"
	"io"
	"strings"

	"dds-provider/internal/generated/bulat"
)

type Service struct {
	client      *api.APIClient
	authToken   string
	pspToken    string
	userType    int32
	permissions map[string]interface{}
}

func New(ctx context.Context, client *api.APIClient, token string) (*Service, error) {
	logger := logging.WithCtxFields(ctx)

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

func (s *Service) doRequest(ctx context.Context, payload map[string]interface{}) ([]byte, error) {
	logger := logging.WithCtxFields(ctx)

	raw, err := json.Marshal(payload)
	if err != nil {
		logger.Errorf("Failed to marshal payload: %v", err)
		return nil, err
	}

	_, httpResp, _ := s.client.DefaultAPI.DetectorMonitorActions(ctx).ReqJson(string(raw)).Execute()
	defer httpResp.Body.Close()

	body, err := io.ReadAll(httpResp.Body)
	if err != nil {
		logger.Errorf("Failed to read body: %v", err)
		return nil, err
	}

	errorMessage := strings.Trim(string(body), `"`)
	if err = ParseErrors(errorMessage); err != nil {
		logger.Errorf("Edit object returned an error: %v", err)
		return nil, err
	}

	return body, nil
}
