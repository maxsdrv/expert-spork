# Debug

#### API Request Examples
```bash
devspace run buf curl http://localhost:19080/api.v1.CommonService/Settings --schema api/proto/common.proto 

devspace run buf curl http://localhost:19080/api.v1.SensorService/Sensors --schema api/proto/sensor.proto 

devspace run buf curl http://localhost:19080/api.v1.JammerService/Jammers --schema api/proto/jammer.proto 

devspace run buf curl http://localhost:19080/api.v1.JammerService/SetJammerBands --schema api/proto/jammer.proto -d'{"jammer_id":"550e8400-e29b-41d4-a716-446655440000","bands_active":["2.4GHz","5.8GHz"],"duration":60}'

devspace run buf curl http://localhost:19080/api.v1.DeviceService/SetPosition --schema api/proto/device.proto -d'{"device_id":"550e8400-e29b-41d4-a716-446655440000","position":{"azimuth":45.5,"coordinate":{"latitude":50.4501,"longitude":30.5234,"altitude":100.0}}}'
