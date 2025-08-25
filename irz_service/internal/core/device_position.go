package core

type DevicePosition interface {
	Azimuth() AngleType
	GeoCoordinate() GeoCoordinate
}

type devicePosition struct {
	azimuth    AngleType
	coordinate GeoCoordinate
}

//func NewDevicePosition(jsonStr string) DevicePosition {}
