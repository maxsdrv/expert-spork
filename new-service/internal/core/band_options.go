package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
)

type BandOptions []apiv1.BandOption

func NewBandOptions(bandsSlice [][]string) BandOptions {
	options := make([]apiv1.BandOption, len(bandsSlice))
	for i, bands := range bandsSlice {
		options[i] = apiv1.BandOption{
			Bands: bands,
		}
	}
	return BandOptions(options)
}
