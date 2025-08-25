package bulat

import (
	"fmt"
	"strings"
)

type apiError struct {
	Code    string
	Message string
}

func (e *apiError) Error() string {
	return fmt.Sprintf("[%s]: %s", e.Code, e.Message)
}

var errorCodeMap = map[string]string{
	"noacc":    "No access to the requested resource",
	"noaccess": "No access to the requested resource",
	"dberror":  "Database execution error",
	"noname":   "Invalid or missing name in the request",
	"noid":     "Invalid or missing ID in the request",
	"no_acc_c": "Incorrect data in the query",
}

func ParseErrors(code string) error {
	code = strings.Trim(code, "\"")
	if msg, ok := errorCodeMap[code]; ok {
		return &apiError{Code: code, Message: msg}
	}
	return nil
}
