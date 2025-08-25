package core

import ()

type Direction interface {
	Azimuth() AngleType
	GeoCoordinate() GeoCoordinate
}

type direction struct {
	azimuth    AngleType
	coordinate GeoCoordinate
}
