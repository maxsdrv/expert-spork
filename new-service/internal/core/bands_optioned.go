package core

type BandsOptioned interface {
	Bands
	GetOptions() *[]BandList
}
