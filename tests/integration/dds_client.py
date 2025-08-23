"""
DDS Provider API Client for integration tests
"""
import httpx
import json
from typing import Dict, Any, Optional


class DDSClient:
    def __init__(self, base_url: str):
        self.base_url = base_url.rstrip('/')
        self.client = httpx.AsyncClient(
            timeout=30.0,
            headers={"Content-Type": "application/json"}
        )
    
    async def close(self):
        await self.client.aclose()
    
    async def _request(self, method: str, endpoint: str, data: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Make Connect/gRPC request"""
        url = f"{self.base_url}/{endpoint}"
        
        try:
            if method == "GET":
                response = await self.client.get(url)
            else:
                json_data = json.dumps(data) if data else "{}"
                response = await self.client.post(url, content=json_data)
            
            response.raise_for_status()
            return response.json() if response.content else {}
            
        except httpx.HTTPStatusError as e:
            if e.response.status_code == 400:
                error_data = e.response.json() if e.response.content else {}
                raise ValueError(f"Validation error: {error_data.get('message', str(e))}")
            raise

    # Common Service
    async def get_settings(self) -> Dict[str, Any]:
        return await self._request("POST", "api.v1.CommonService/Settings")
    
    async def get_license_status(self) -> Dict[str, Any]:
        return await self._request("POST", "api.v1.CommonService/LicenseStatus")

    # Sensor Service  
    async def get_sensors(self) -> Dict[str, Any]:
        return await self._request("POST", "api.v1.SensorService/Sensors")
    
    async def get_sensor_info_dynamic(self, sensor_id: str) -> Dict[str, Any]:
        data = {"sensor_id": sensor_id}
        return await self._request("POST", "api.v1.SensorService/SensorInfoDynamic", data)
    
    async def set_jammer_mode(self, sensor_id: str, mode: str, timeout: Optional[int] = None) -> Dict[str, Any]:
        data = {
            "sensor_id": sensor_id,
            "jammer_mode": mode
        }
        if timeout is not None:
            data["timeout"] = timeout
        return await self._request("POST", "api.v1.SensorService/SetJammerMode", data)

    # Jammer Service
    async def get_jammers(self) -> Dict[str, Any]:
        return await self._request("POST", "api.v1.JammerService/Jammers")
    
    async def get_jammer_info_dynamic(self, jammer_id: str) -> Dict[str, Any]:
        data = {"jammer_id": jammer_id}
        return await self._request("POST", "api.v1.JammerService/JammerInfoDynamic", data)
    
    async def get_jammer_groups(self) -> Dict[str, Any]:
        return await self._request("POST", "api.v1.JammerService/Groups")
    
    async def set_jammer_bands(self, jammer_id: str, bands: list, duration: int) -> Dict[str, Any]:
        data = {
            "jammer_id": jammer_id,
            "bands_active": bands,
            "duration": duration
        }
        return await self._request("POST", "api.v1.JammerService/SetJammerBands", data)

    # Device Service
    async def set_device_disabled(self, device_id: str, disabled: bool) -> Dict[str, Any]:
        data = {
            "device_id": device_id,
            "disabled": disabled
        }
        return await self._request("POST", "api.v1.DeviceService/SetDisabled", data)
    
    async def set_device_position(self, device_id: str, position: Dict[str, Any]) -> Dict[str, Any]:
        data = {
            "device_id": device_id,
            "position": position
        }
        return await self._request("POST", "api.v1.DeviceService/SetPosition", data)
    
    async def set_device_position_mode(self, device_id: str, position_mode: str) -> Dict[str, Any]:
        data = {
            "device_id": device_id,
            "position_mode": position_mode
        }
        return await self._request("POST", "api.v1.DeviceService/SetPositionMode", data)

    # Camera Service
    async def get_camera_id(self, sensor_id: str) -> Dict[str, Any]:
        data = {"sensor_id": sensor_id}
        return await self._request("POST", "api.v1.CameraService/CameraId", data)

    # Target Service
    async def get_targets(self) -> Dict[str, Any]:
        return await self._request("POST", "api.v1.TargetService/Targets")