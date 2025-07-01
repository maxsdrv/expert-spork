package core

type DeviceWorkzoneSector interface {
	Number() int
	Distance() float64
	MinAngle() AngleType
	MaxAngle() AngleType
}

type deviceWorkzoneSector struct {
	Number   int
	Distance float64
	MinAngle AngleType
	MaxAngle AngleType
}

type DeviceWorkzone interface {
	Sectors() []DeviceWorkzoneSector
}

type deviceWorkzone struct {
	Sectors []DeviceWorkzoneSector
}
