package bulat

import (
	"context"
	"encoding/json"
	"fmt"

	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger(nil)

const (
	loginAction = "login"
)

func (s *Service) Authenticate(ctx context.Context) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Starting authentication")

	if s.authToken == "" {
		return fmt.Errorf("auth token is empty")
	}
	logger.Debugf("Auth token: %s", s.authToken)

	payload := map[string]any{
		"action": loginAction,
		"token":  s.authToken,
		"capt":   "current_captcha",
		"lang":   "ru",
	}

	body, err := s.doRequest(ctx, loginAction, payload)
	if err != nil {
		return err
	}

	var response []any
	if err := json.Unmarshal(body, &response); err != nil {
		logger.Errorf("Failed unmarshal login response: %v", err)
		return err
	}

	return s.handleSuccessResponse(response)
}

func (s *Service) PSPToken() string {
	return s.pspToken
}

func (s *Service) Permissions() map[string]any {
	return s.permissions
}
func (s *Service) handleSuccessResponse(response []any) error {
	if userType, ok := response[1].(float64); ok {
		s.userType = int32(userType)
	}
	s.pspToken = response[2].(string)
	fmt.Printf("PSP token: %s", s.pspToken)
	s.client.GetConfig().AddDefaultHeader("Cookie", fmt.Sprintf("psp=%s", s.pspToken))

	if perms, ok := response[3].(map[string]any); ok {
		s.permissions = make(map[string]any)
		for key, value := range perms {
			s.permissions[key] = value.(string)
		}
	}
	return nil
}
