package core

type GeoPosition interface {
	Azimuth() AngleType
	GeoCoordinate() GeoCoordinate
}

type geoPosition struct {
	azimuth    AngleType
	coordinate GeoCoordinate
}

//func NewDevicePosition(jsonStr string) DevicePosition {}
