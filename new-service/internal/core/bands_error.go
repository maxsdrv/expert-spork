package core

import "fmt"

type BandNotSupportedError struct {
	band string
}

func (e *BandNotSupportedError) Error() string {
	return fmt.Sprintf("band %s not supported", e.band)
}
