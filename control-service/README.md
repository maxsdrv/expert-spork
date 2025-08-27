# DDS control

### Development
###### Set Docker context to Dev VM Docker Daemon
```bash
devspace run use-vm-docker
```

### Testing
To upload test encrypted firmware file (created with dds-firmware) run
```bash
firmware=fw.bin
size=$(wc -c < "$firmware")
curl -v localhost:8080/api/v1/upload/firmware --data-binary @"$firmware" -H "Content-Type: application/octet-stream" -H "X-Content-Range: bytes 0-$(($size-1))/$(($size+0))" -H "X-Session-ID: 111111111111" -H "Content-Disposition: attachment"
```
