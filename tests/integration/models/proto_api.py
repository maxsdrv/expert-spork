"""
DDS Provider API Models (protobuf-based)
Auto-generated equivalent of OpenAPI models for protobuf/Connect API
"""

from __future__ import annotations
from enum import Enum
from typing import List, Optional, Dict, Any
from pydantic import BaseModel, Field


# Enum Models
class SensorType(str, Enum):
    SENSOR_RFD = "SENSOR_RFD"
    SENSOR_RADAR = "SENSOR_RADAR"
    SENSOR_CAMERA = "SENSOR_CAMERA"


class JammerMode(str, Enum):
    JAMMER_AUTO = "JAMMER_AUTO"
    JAMMER_MANUAL = "JAMMER_MANUAL"


class GeoPositionMode(str, Enum):
    GEO_AUTO = "GEO_AUTO"
    GEO_MANUAL = "GEO_MANUAL"


# Common Models
class GeoCoordinate(BaseModel):
    latitude: float
    longitude: float
    altitude: float


class GeoPosition(BaseModel):
    azimuth: float
    coordinate: GeoCoordinate


class WorkzoneSector(BaseModel):
    # Add fields as needed based on your proto definition
    pass


class HwInfo(BaseModel):
    # Add fields as needed based on your proto definition
    pass


# Sensor Service Models
class SensorInfo(BaseModel):
    sensor_id: str
    jammer_ids: List[str] = Field(default_factory=list)
    type: Optional[SensorType] = None
    model: Optional[str] = None
    serial: Optional[str] = None
    sw_version: Optional[str] = None


class SensorsResponse(BaseModel):
    sensor_infos: List[SensorInfo] = Field(default_factory=list)


class SensorInfoDynamicRequest(BaseModel):
    sensor_id: str


class SensorInfoDynamicResponse(BaseModel):
    disabled: Optional[bool] = None
    state: Optional[str] = None
    position: Optional[GeoPosition] = None
    position_mode: Optional[GeoPositionMode] = None
    workzones: List[WorkzoneSector] = Field(default_factory=list)
    jammer_mode: Optional[JammerMode] = None
    jammer_auto_timeout: Optional[int] = None
    hw_info: Optional[HwInfo] = None


class SetJammerModeRequest(BaseModel):
    sensor_id: str
    jammer_mode: JammerMode
    timeout: Optional[int] = None


# Jammer Service Models
class JammerInfo(BaseModel):
    jammer_id: str
    # Add other jammer fields based on your proto definition


class JammersResponse(BaseModel):
    jammer_infos: List[JammerInfo] = Field(default_factory=list)


class SetJammerBandsRequest(BaseModel):
    jammer_id: str
    bands_active: List[str]
    duration: int


class JammerInfoDynamicRequest(BaseModel):
    jammer_id: str


class JammerInfoDynamicResponse(BaseModel):
    # Add fields based on your proto definition
    pass


# Device Service Models
class SetDisabledRequest(BaseModel):
    device_id: str
    disabled: bool


class SetPositionModeRequest(BaseModel):
    device_id: str
    position_mode: GeoPositionMode


class SetPositionRequest(BaseModel):
    device_id: str
    position: GeoPosition


# Camera Service Models  
class CameraIdRequest(BaseModel):
    sensor_id: str


class CameraIdResponse(BaseModel):
    camera_id: Optional[str] = None


# Target Service Models
class TargetsResponse(BaseModel):
    # Add target fields based on your proto definition
    pass


# Common Service Models
class SettingsResponse(BaseModel):
    # Add settings fields based on your proto definition  
    pass


# Request/Response wrappers for Connect protocol
class ConnectRequest(BaseModel):
    """Generic Connect request wrapper"""
    msg: Dict[str, Any] = Field(default_factory=dict)


class ConnectResponse(BaseModel):
    """Generic Connect response wrapper"""
    data: Optional[Dict[str, Any]] = None
    error: Optional[str] = None


# Validation Error Models (for Connect/gRPC errors)
class ConnectError(BaseModel):
    code: str
    message: str
    details: Optional[List[Dict[str, Any]]] = None
