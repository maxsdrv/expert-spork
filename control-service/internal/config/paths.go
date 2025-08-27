package config

var Paths struct {
	TriggersDir string
	UploadDir   string
	FirmwareDir string
	LogsDir     string
	BackupsDir  string
}

func init() {
	load(&Paths)
}
