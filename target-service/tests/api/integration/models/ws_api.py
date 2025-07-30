# Auto-generated DDS Provider WebSocket API Models

from __future__ import annotations

from enum import Enum
from typing import List, Optional, Union
from uuid import UUID

from pydantic import AwareDatetime, BaseModel, Field, confloat


class Level(Enum):
    NONE = 'NONE'
    MEDIUM = 'MEDIUM'
    HIGH = 'HIGH'
    CRITICAL = 'CRITICAL'


class AlarmStatus(BaseModel):
    level: Level
    score: Optional[float] = None


class Band(BaseModel):
    label: str = Field(..., examples=['2.4G'])
    active: bool


class ClassName(Enum):
    UNDEFINED = 'UNDEFINED'
    DRONE = 'DRONE'
    CAR = 'CAR'
    TRUCK = 'TRUCK'
    HUMAN = 'HUMAN'
    TREE = 'TREE'
    PEOPLE_GROUP = 'PEOPLE_GROUP'
    MOTORCYCLE = 'MOTORCYCLE'
    JET_SKI = 'JET_SKI'
    BOAT = 'BOAT'
    SHIP = 'SHIP'
    REMOTE_CONSOLE = 'REMOTE_CONSOLE'


class GeoCoordinate(BaseModel):
    altitude: Optional[float] = Field(
        None, description='Altitude in meters above sea level'
    )
    latitude: float = Field(..., description='Latitude in decimal degrees')
    longitude: float = Field(..., description='Longitude in decimal degrees')


class GeoDirection(BaseModel):
    bearing: float = Field(..., description='Bearing on object in angular degrees')
    distance: float = Field(..., description='Distance to object in meters')
    elevation: Optional[float] = Field(
        None, description='Object elevation in angular degrees'
    )


class GeoPosition(BaseModel):
    azimuth: float
    coordinate: GeoCoordinate


class JammerMode(Enum):
    AUTO = 'AUTO'
    MANUAL = 'MANUAL'


class HwInfo(BaseModel):
    temperature: Optional[str] = Field(None, examples=['12 ℃'])
    voltage: Optional[str] = Field(None, examples=['48 V'])
    current: Optional[str] = Field(None, examples=['840 mA'])


class SensorType(Enum):
    RFD = 'RFD'
    RADAR = 'RADAR'
    CAMERA = 'CAMERA'


class State(Enum):
    UNAVAILABLE = 'UNAVAILABLE'
    False_ = False
    FAILURE = 'FAILURE'
    OK = 'OK'


class Direction(BaseModel):
    bearing: Optional[confloat(ge=0.0, lt=360.0)] = Field(
        None, description='Bearing on target in decimal degrees'
    )
    distance: Optional[confloat(ge=0.0)] = Field(
        None, description='Distance to target in meters'
    )
    elevation: Optional[confloat(ge=0.0, lt=90.0)] = Field(
        None, description='Target elevation in decimal degrees'
    )


class Position(BaseModel):
    coordinate: Optional[GeoCoordinate] = None
    source_id: Optional[UUID] = Field(
        None, description='Matching sensor ID identification string based on UUID'
    )
    direction: Optional[Direction] = None


class TargetAttributesRFD(BaseModel):
    digital: bool
    frequencies: List[int]
    description: Optional[str] = None
    geolocation: bool


class TargetAttributesRADAR(BaseModel):
    pass


class TargetAttributesCAMERA(TargetAttributesRADAR):
    pass


class WorkzoneSector(BaseModel):
    number: int
    distance: float
    min_angle: float
    max_angle: float


class Error(BaseModel):
    code: int
    description: str


class Orientation(BaseModel):
    bearing: float
    elevation: float
    zoom: float


class CameraOrientation(BaseModel):
    id: UUID = Field(
        ..., description='Matching sensor ID identification string based on UUID'
    )
    orientation: Orientation


class TargetData(BaseModel):
    target_id: UUID = Field(
        ..., description='Matching sensor ID identification string based on UUID'
    )
    sensor_id: Optional[UUID] = Field(
        None, description='Matching sensor ID identification string based on UUID'
    )
    track_id: Optional[UUID] = Field(
        None, description='Matching sensor ID identification string based on UUID'
    )
    alarm_ids: Optional[List[UUID]] = Field(
        None, description='List of linked alarm event ids'
    )
    cls_ids: Optional[List[UUID]] = Field(
        None, description='List of linked classification event ids'
    )
    class_name: ClassName
    attributes: Optional[
        Union[TargetAttributesRFD, TargetAttributesRADAR, TargetAttributesCAMERA]
    ] = None
    alarm_status: AlarmStatus
    position: Optional[Position] = None
    last_updated: AwareDatetime = Field(
        ...,
        description='date and time in ISO 8601 format with milliseconds',
        examples=['2017-07-21T17:32:28.123Z'],
    )


class TargetList(BaseModel):
    targets: List[TargetData]


class TrackPoint(BaseModel):
    coordinate: Optional[GeoCoordinate] = None
    direction: Optional[GeoDirection] = None
    timestamp: AwareDatetime = Field(
        ...,
        description='date and time in ISO 8601 format with milliseconds',
        examples=['2017-07-21T17:32:28.123Z'],
    )


class JammerInfo(BaseModel):
    id: UUID = Field(
        ..., description='Matching sensor ID identification string based on UUID'
    )
    model: str = Field(..., description='model type string')
    serial: Optional[str] = Field(None, description='serial number string')
    sw_version: Optional[str] = Field(
        None, description='Version string', examples=['1.12.7']
    )
    disabled: bool
    state: State
    position: GeoPosition
    workzone: List[WorkzoneSector]
    sensor_id: Optional[UUID] = Field(
        None, description='Matching sensor ID identification string based on UUID'
    )
    bands: List[Band] = Field(
        ..., description='List of available bands with corresponding activity status'
    )
    band_options: Optional[List[List[str]]] = None
    hw_info: Optional[HwInfo] = None


class JammerList(BaseModel):
    jammers: List[JammerInfo]


class SensorInfo(BaseModel):
    id: UUID = Field(
        ..., description='Matching sensor ID identification string based on UUID'
    )
    type: SensorType
    model: str = Field(..., description='model type string')
    serial: Optional[str] = Field(None, description='serial number string')
    sw_version: Optional[str] = Field(
        None, description='Version string', examples=['1.12.7']
    )
    disabled: bool
    state: State
    position: GeoPosition
    workzone: List[WorkzoneSector]
    jammer_ids: Optional[List[str]] = Field(
        None, description='List of matching jammer IDs'
    )
    jammer_mode: Optional[JammerMode] = None
    jammer_timeout: Optional[int] = None
    hw_info: Optional[HwInfo] = None


class TrackData(BaseModel):
    track_id: UUID = Field(
        ..., description='Matching sensor ID identification string based on UUID'
    )
    target_id: UUID = Field(
        ..., description='Matching sensor ID identification string based on UUID'
    )
    source_id: UUID = Field(
        ..., description='Matching sensor ID identification string based on UUID'
    )
    points: List[TrackPoint]


class TrackList(BaseModel):
    tracks: List[TrackData]


class SensorList(BaseModel):
    sensors: List[SensorInfo]
