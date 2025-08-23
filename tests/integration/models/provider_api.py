"""
DDS Provider API - Auto-generated from protobuf definitions
"""

from __future__ import annotations
from enum import Enum
from typing import List, Optional, Dict, Any
from pydantic import BaseModel, Field


class GeoPositionMode(str, Enum):
    """Auto-generated from protobuf enum GeoPositionMode"""
    GEO_AUTO = "GEO_AUTO"
    GEO_MANUAL = "GEO_MANUAL"
    GEO_ALWAYS_MANUAL = "GEO_ALWAYS_MANUAL"


class SensorType(str, Enum):
    """Auto-generated from protobuf enum SensorType"""
    SENSOR_RFD = "SENSOR_RFD"
    SENSOR_RADAR = "SENSOR_RADAR"
    SENSOR_CAMERA = "SENSOR_CAMERA"


class JammerMode(str, Enum):
    """Auto-generated from protobuf enum JammerMode"""
    JAMMER_AUTO = "JAMMER_AUTO"
    JAMMER_MANUAL = "JAMMER_MANUAL"


class TargetClass(str, Enum):
    """Auto-generated from protobuf enum TargetClass"""
    TARGET_UNDEFINED = "TARGET_UNDEFINED"
    TARGET_DRONE = "TARGET_DRONE"
    TARGET_CAR = "TARGET_CAR"
    TARGET_TRUCK = "TARGET_TRUCK"
    TARGET_HUMAN = "TARGET_HUMAN"
    TARGET_TREE = "TARGET_TREE"
    TARGET_PEOPLE_GROUP = "TARGET_PEOPLE_GROUP"
    TARGET_MOTORCYCLE = "TARGET_MOTORCYCLE"
    TARGET_JET_SKI = "TARGET_JET_SKI"
    TARGET_BOAT = "TARGET_BOAT"
    TARGET_SHIP = "TARGET_SHIP"
    TARGET_REMOTE_CONSOLE = "TARGET_REMOTE_CONSOLE"


class AlarmLevel(str, Enum):
    """Auto-generated from protobuf enum AlarmLevel"""
    ALARM_NONE = "ALARM_NONE"
    ALARM_MEDIUM = "ALARM_MEDIUM"
    ALARM_HIGH = "ALARM_HIGH"
    ALARM_CRITICAL = "ALARM_CRITICAL"


class CameraIdRequest(BaseModel):
    """Auto-generated from protobuf message CameraIdRequest"""
    sensor_id: Optional[str] = None


class CameraIdResponse(BaseModel):
    """Auto-generated from protobuf message CameraIdResponse"""
    camera_id: Optional[str] = None


class SetDisabledRequest(BaseModel):
    """Auto-generated from protobuf message SetDisabledRequest"""
    device_id: Optional[str] = None
    disabled: Optional[bool] = None


class SetPositionModeRequest(BaseModel):
    """Auto-generated from protobuf message SetPositionModeRequest"""
    device_id: Optional[str] = None
    position_mode: Optional[GeoPositionMode] = None


class SetPositionRequest(BaseModel):
    """Auto-generated from protobuf message SetPositionRequest"""
    device_id: Optional[str] = None
    position: Optional[GeoPosition] = None


class GeoPosition(BaseModel):
    """Auto-generated from protobuf message GeoPosition"""
    pass


class GeoCoordinate(BaseModel):
    """Auto-generated from protobuf message GeoCoordinate"""
    latitude: Optional[float] = None
    longitude: Optional[float] = None
    altitude: Optional[float] = None


class WorkzoneSector(BaseModel):
    """Auto-generated from protobuf message WorkzoneSector"""
    number: Optional[int] = None
    distance: Optional[float] = None


class HwInfo(BaseModel):
    """Auto-generated from protobuf message HwInfo"""
    temperature: Optional[float] = None
    voltage: Optional[float] = None
    current: Optional[float] = None


class JammerInfo(BaseModel):
    """Auto-generated from protobuf message JammerInfo"""
    jammer_id: Optional[str] = None
    model: Optional[str] = None
    serial: Optional[str] = None
    sw_version: Optional[str] = None
    sensor_id: Optional[str] = None
    group_id: Optional[str] = None


class JammersResponse(BaseModel):
    """Auto-generated from protobuf message JammersResponse"""
    jammer_infos: List[JammerInfo] = Field(default_factory=list)


class JammerInfoRequest(BaseModel):
    """Auto-generated from protobuf message JammerInfoRequest"""
    jammer_id: Optional[str] = None


class JammerInfoDynamicRequest(BaseModel):
    """Auto-generated from protobuf message JammerInfoDynamicRequest"""
    jammer_id: Optional[str] = None


class JammerInfoDynamicResponse(BaseModel):
    """Auto-generated from protobuf message JammerInfoDynamicResponse"""
    disabled: Optional[bool] = None
    state: Optional[str] = None
    position: Optional[GeoPosition] = None
    position_mode: Optional[GeoPositionMode] = None
    workzone: List[WorkzoneSector] = Field(default_factory=list)
    bands: List[Band] = Field(default_factory=list)
    band_options: List[BandOption] = Field(default_factory=list)
    hw_info: Optional[HwInfo] = None
    timeout_status: Optional[JammerTimeoutStatus] = None


class GroupsResponse(BaseModel):
    """Auto-generated from protobuf message GroupsResponse"""
    jammer_groups: List[JammerGroup] = Field(default_factory=list)


class JammerGroup(BaseModel):
    """Auto-generated from protobuf message JammerGroup"""
    group_id: Optional[str] = None
    name: Optional[str] = None


class SetJammerBandsRequest(BaseModel):
    """Auto-generated from protobuf message SetJammerBandsRequest"""
    jammer_id: Optional[str] = None
    bands_active: List[str] = Field(default_factory=list)
    duration: Optional[int] = None


class Band(BaseModel):
    """Auto-generated from protobuf message Band"""
    label: Optional[str] = None
    active: Optional[bool] = None


class BandOption(BaseModel):
    """Auto-generated from protobuf message BandOption"""
    bands: List[str] = Field(default_factory=list)


class JammerTimeoutStatus(BaseModel):
    """Auto-generated from protobuf message JammerTimeoutStatus"""
    started: Optional[Dict[str, Any]] = None
    now: Optional[Dict[str, Any]] = None
    duration: Optional[int] = None


class SensorInfo(BaseModel):
    """Auto-generated from protobuf message SensorInfo"""
    sensor_id: Optional[str] = None
    jammer_ids: List[str] = Field(default_factory=list)
    type: Optional[SensorType] = None
    model: Optional[str] = None
    serial: Optional[str] = None
    sw_version: Optional[str] = None


class SensorsResponse(BaseModel):
    """Auto-generated from protobuf message SensorsResponse"""
    sensor_infos: List[SensorInfo] = Field(default_factory=list)


class SensorInfoDynamicRequest(BaseModel):
    """Auto-generated from protobuf message SensorInfoDynamicRequest"""
    sensor_id: Optional[str] = None


class SensorInfoDynamicResponse(BaseModel):
    """Auto-generated from protobuf message SensorInfoDynamicResponse"""
    disabled: Optional[bool] = None
    state: Optional[str] = None
    position: Optional[GeoPosition] = None
    position_mode: Optional[GeoPositionMode] = None
    workzones: List[WorkzoneSector] = Field(default_factory=list)
    jammer_mode: Optional[JammerMode] = None
    jammer_auto_timeout: Optional[int] = None
    hw_info: Optional[HwInfo] = None


class SetJammerModeRequest(BaseModel):
    """Auto-generated from protobuf message SetJammerModeRequest"""
    sensor_id: Optional[str] = None
    jammer_mode: Optional[JammerMode] = None
    timeout: Optional[int] = None


class TargetsResponse(BaseModel):
    """Auto-generated from protobuf message TargetsResponse"""
    target_id: Optional[str] = None
    class_name: Optional[TargetClass] = None
    alarm_status: Optional[AlarmStatus] = None
    last_updated: Optional[Dict[str, Any]] = None
    sensor_id: Optional[str] = None
    track_id: Optional[str] = None
    camera_track: Optional[CameraTrack] = None
    alarm_ids: List[str] = Field(default_factory=list)
    cls_ids: List[str] = Field(default_factory=list)
    rfd_attributes: TargetAttributesRFD
    radar_attributes: TargetAttributesRADAR
    camera_attributes: TargetAttributesCAMERA


class TargetPosition(BaseModel):
    """Auto-generated from protobuf message TargetPosition"""
    coordinate: Optional[GeoCoordinate] = None
    source_id: Optional[str] = None
    direction: Optional[TargetDirection] = None


class CameraTrack(BaseModel):
    """Auto-generated from protobuf message CameraTrack"""
    camera_id: Optional[str] = None
    target_id: Optional[str] = None


class AlarmStatus(BaseModel):
    """Auto-generated from protobuf message AlarmStatus"""
    level: Optional[AlarmLevel] = None
    score: Optional[float] = None


class TargetAttributesRFD(BaseModel):
    """Auto-generated from protobuf message TargetAttributesRFD"""
    digital: Optional[bool] = None
    frequencies: List[int] = Field(default_factory=list)
    geolocation: Optional[bool] = None
    description: Optional[str] = None


class TargetAttributesRADAR(BaseModel):
    """Auto-generated from protobuf message TargetAttributesRADAR"""
    pass


class TargetAttributesCAMERA(BaseModel):
    """Auto-generated from protobuf message TargetAttributesCAMERA"""
    pass


class TargetDirection(BaseModel):
    """Auto-generated from protobuf message TargetDirection"""
    pass


class SettingsResponse(BaseModel):
    """Auto-generated from protobuf message SettingsResponse"""
    settings: Optional[Settings] = None


class SetSettingsRequest(BaseModel):
    """Auto-generated from protobuf message SetSettingsRequest"""
    settings: Optional[Settings] = None


class LicenseStatusResponse(BaseModel):
    """Auto-generated from protobuf message LicenseStatusResponse"""
    valid: Optional[bool] = None
    description: Optional[str] = None
    expiry_date: Optional[str] = None
    signature: Optional[str] = None


class UploadLicenseRequest(BaseModel):
    """Auto-generated from protobuf message UploadLicenseRequest"""
    license_data: Optional[str] = None


class Settings(BaseModel):
    """Auto-generated from protobuf message Settings"""
    device_id_mapping: List[DeviceIdMapping] = Field(default_factory=list)
    devices: List[DeviceSettings] = Field(default_factory=list)


class DeviceIdMapping(BaseModel):
    """Auto-generated from protobuf message DeviceIdMapping"""
    device_id: Optional[str] = None
    fingerprint: Optional[str] = None
    class_name: Optional[str] = None


class DeviceSettings(BaseModel):
    """Auto-generated from protobuf message DeviceSettings"""
    device_id: Optional[str] = None
    disabled: Optional[bool] = None
    position_mode: Optional[GeoPositionMode] = None
    position: Optional[GeoPosition] = None


# Helper classes for Connect/gRPC
class EmptyResponse(BaseModel):
    """Empty response for void operations"""
    pass


class ConnectError(BaseModel):
    """Connect/gRPC error response"""
    code: str
    message: str
    details: Optional[List[Dict[str, Any]]] = None
