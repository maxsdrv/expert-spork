package core

type BandsOptioned interface {
	JammerBands
	GetOptions() *[]BandList
}
