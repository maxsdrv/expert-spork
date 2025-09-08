package core

import (
	apiv1 "dds-provider/internal/generated/api/proto"
)

type BandOptions []apiv1.BandOption

func NewBandOptions(bandsSlice [][]string) BandOptions {
	var options []apiv1.BandOption
	for _, bands := range bandsSlice {
		options = append(options, apiv1.BandOption{
			Bands: bands,
		})
	}
	return BandOptions(options)
}
