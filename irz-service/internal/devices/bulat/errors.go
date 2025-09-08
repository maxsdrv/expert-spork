package bulat

import (
	"encoding/json"
	"fmt"
	"strings"

	api "dds-provider/internal/generated/bulat"
)

var errorMessages = map[api.ErrorCode]string{
	api.ERRORCODE_NOACC:    "No access to the requested resource",
	api.ERRORCODE_NOACCESS: "No access to the requested resource",
	api.ERRORCODE_DBERROR:  "Database query execution error",
	api.ERRORCODE_NONAME:   "Missing or invalid name parameter",
	api.ERRORCODE_NOID:     "Missing or invalid ID parameter",
	api.ERRORCODE_NO_ACC_C: "No access to the requested command",
}

type Error struct {
	Code            api.ErrorCode
	Message         string
	CaptchaRequired bool
}

func (e *Error) Error() string {
	if e.CaptchaRequired {
		return fmt.Sprintf("[%s]: %s (CAPTCHA required)", e.Code, e.Message)
	}
	return fmt.Sprintf("[%s]: %s", e.Code, e.Message)
}

func ParseErrors(responseBody string) error {
	if responseBody == "" {
		return nil
	}

	cleanBody := strings.Trim(responseBody, `"`)
	cleanBody = strings.TrimSpace(cleanBody)

	if code := stringToErrorCode(cleanBody); code != "" {
		return &Error{
			Code:    code,
			Message: errorMessages[code],
		}
	}

	for errorStr, code := range stringToCodeMap {
		if strings.HasPrefix(strings.ToLower(cleanBody), errorStr) {
			message := errorMessages[code]
			if len(cleanBody) > len(errorStr) {
				additionalMsg := strings.TrimPrefix(cleanBody, errorStr)
				additionalMsg = strings.TrimLeft(additionalMsg, ": ")
				if additionalMsg != "" {
					message = fmt.Sprintf("%s: %s", errorMessages[code], additionalMsg)
				}
			}
			return &Error{
				Code:    code,
				Message: message,
			}
		}
	}

	var arrayResponse []any
	if err := json.Unmarshal([]byte(responseBody), &arrayResponse); err == nil && len(arrayResponse) >= 2 {
		captchaRequired := false
		if flag, ok := arrayResponse[2].(float64); ok && flag == 1 {
			captchaRequired = true
		}
		if verdict, ok := arrayResponse[0].(string); ok && verdict == "error" {
			message := "Authentication error"
			if details, ok := arrayResponse[1].([]any); ok && len(details) > 0 {
				if msg, ok := details[0].(string); ok {
					message = msg
				}
			}
			return &Error{
				Code:            api.ERRORCODE_NOACCESS,
				Message:         message,
				CaptchaRequired: captchaRequired,
			}
		}
	}

	var statusCheck map[string]any
	if err := json.Unmarshal([]byte(responseBody), &statusCheck); err == nil {
		if status, exists := statusCheck["status"]; exists {
			if statusInt, ok := status.(float64); ok && statusInt == 1 {
				return nil
			}
		}
	}

	return nil
}

var stringToCodeMap = map[string]api.ErrorCode{
	"noacc":    api.ERRORCODE_NOACC,
	"noaccess": api.ERRORCODE_NOACCESS,
	"dberror":  api.ERRORCODE_DBERROR,
	"noname":   api.ERRORCODE_NONAME,
	"noid":     api.ERRORCODE_NOID,
	"no_acc_c": api.ERRORCODE_NO_ACC_C,
}

func stringToErrorCode(s string) api.ErrorCode {
	if code, exists := stringToCodeMap[s]; exists {
		return code
	}
	return ""
}

func (s *Service) HandleAPIResponse(body []byte) error {
	if err := ParseErrors(string(body)); err != nil {
		return err
	}
	return nil
}
