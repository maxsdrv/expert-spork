package proxy

import (
	"errors"
	"fmt"
)

type ProxyError struct {
	Op  string
	Err error
}

func (e *ProxyError) Error() string {
	return fmt.Sprintf("bridge: %s: %v", e.Op, e.Err)
}

var (
	ErrProxyTimeout = ProxyError{Op: "http", Err: errors.New("timeout")}
	//add more
)
