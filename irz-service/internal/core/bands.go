package core

import (
	"slices"
)

type BandList []string

type JammerBands interface {
	Set(bands BandList) (JammerBands, error)
	Active() BandList
	All() BandList
}

type bandsImpl struct {
	all    BandList
	active BandList
}

func NewBandsEmpty(all BandList) JammerBands {
	return &bandsImpl{
		all: all,
	}
}

func NewBands(all BandList, active BandList) (JammerBands, error) {
	for _, band := range active {
		if !slices.Contains(all, band) {
			return nil, &BandNotSupportedError{band: band}
		}
	}
	return &bandsImpl{
		all:    all,
		active: active,
	}, nil
}

func (b *bandsImpl) Set(bands BandList) (JammerBands, error) {
	return NewBands(b.all, bands)
}

func (b *bandsImpl) Active() BandList {
	return b.active
}

func (b *bandsImpl) All() BandList {
	return b.all
}
