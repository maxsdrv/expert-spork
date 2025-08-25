package bulat

import (
	"context"
	"encoding/json"
	"fmt"
	"io"

	"github.com/opticoder/ctx-log/go/ctx_log"
)

var logging = ctx_log.GetLogger(nil)

type LoginRequest struct {
	Action string `json:"action"`
	Token  string `json:"token"`
	Capt   string `json:"capt"`
	Lang   string `json:"lang"`
}

type Error struct {
	Code            string
	Message         string
	CaptchaRequired bool
}

func (e *Error) Error() string {
	return fmt.Sprintf("[%s]: %s, captcha required=%v", e.Code, e.Message, e.CaptchaRequired)
}

func (s *Service) Authenticate(ctx context.Context) error {
	logger := logging.WithCtxFields(ctx)

	logger.Debugf("Starting authentication")

	// TODO: Implement validator or something else for auth token
	if s.authToken == "" {
		return fmt.Errorf("auth token is empty")
	}

	payload := &LoginRequest{
		Action: "login",
		Token:  s.authToken,
		Capt:   "current_captcha",
		Lang:   "ru",
	}
	raw, err := json.Marshal(payload)
	if err != nil {
		logger.Errorf("Failed marshal login payload: %v", err)
		return err
	}

	_, httpResp, _ := s.client.DefaultAPI.LoginWithToken(ctx).ReqJson(string(raw)).Execute()
	body, _ := io.ReadAll(httpResp.Body)
	defer httpResp.Body.Close()

	var response []interface{}
	if err := json.Unmarshal(body, &response); err != nil {
		logger.Errorf("Failed unmarshal login response: %v", err)
		return err
	}

	verdict := response[0].(string)
	if verdict == "good" {
		return s.handleSuccessResponse(response)
	} else {
		return s.handleErrorResponse(response)
	}
}

func (s *Service) PSPToken() string {
	return s.pspToken
}

func (s *Service) Permissions() map[string]interface{} {
	return s.permissions
}

func (s *Service) handleSuccessResponse(response []interface{}) error {
	if userType, ok := response[1].(float64); ok {
		s.userType = int32(userType)
	}
	s.pspToken = response[2].(string)
	fmt.Printf("PSP token: %s", s.pspToken)
	s.client.GetConfig().AddDefaultHeader("Cookie", fmt.Sprintf("psp=%s", s.pspToken))

	if perms, ok := response[3].(map[string]interface{}); ok {
		s.permissions = make(map[string]interface{})
		for key, value := range perms {
			s.permissions[key] = value.(string)
		}
	}
	return nil
}

func (s *Service) handleErrorResponse(response []interface{}) error {
	captchaRequired := false
	if flag, ok := response[2].(float64); ok && flag == 1 {
		captchaRequired = true
	}
	details := response[1].([]interface{})
	fmt.Print(details)
	return &Error{
		Code:            response[0].(string),
		Message:         details[0].(string),
		CaptchaRequired: captchaRequired,
	}
}
