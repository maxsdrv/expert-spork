# Auto-generated DDS Control HTTP API Models

from __future__ import annotations

from datetime import date, datetime
from enum import Enum
from typing import List, Optional

from pydantic import BaseModel, Field, RootModel


class ServiceUpdate(BaseModel):
    version: str
    service: Optional[str] = None


class Status(Enum):
    idle = 'idle'
    backup = 'backup'
    preparing = 'preparing'
    active = 'active'
    done = 'done'
    error = 'error'


class UpdateStatus(BaseModel):
    status: Status
    log: Optional[str] = Field(None, description="Error log in case of 'error' status")


class LogsList(RootModel[List[str]]):
    root: List[str] = Field(..., description='List of the log files to download')


class Status1(Enum):
    idle = 'idle'
    done = 'done'
    error = 'error'


class SystemRebootStatus(BaseModel):
    status: Status1
    log: Optional[str] = Field(None, description="Error log in case of 'error' status")


class ServiceState(Enum):
    running = 'running'
    restarting = 'restarting'
    exited = 'exited'


class Info(BaseModel):
    state: ServiceState
    version: str
    revision: str
    since: datetime
    crashes: int


class ServiceInfo(BaseModel):
    service: str
    info: Optional[Info] = None


class NtpServers(BaseModel):
    servers: List[str]


class NetworkSettings(BaseModel):
    address: str
    prefix: int
    gateway: Optional[str] = None
    dns_nameservers: List[str]


class SystemMetrics(BaseModel):
    cpu: int
    disk: int
    memory: int


class NtpSyncState(BaseModel):
    enabled: Optional[bool] = None


class SystemTime(BaseModel):
    timestamp: datetime


class ConfigType(Enum):
    DEVICES = 'DEVICES'
    SYSTEM = 'SYSTEM'


class ConfigFileData(BaseModel):
    data: str = Field(..., description='Configuration file content')


class ServicesGetResponse(RootModel[List[ServiceInfo]]):
    root: List[ServiceInfo]


class ServicesSetStatePostRequest(BaseModel):
    service: str
    state: ServiceState


class LogsListGetParametersQuery(BaseModel):
    start: date
    end: date


class LogsGetParametersQuery(BaseModel):
    logs_list: str


class ConfigGetConfigGetParametersQuery(BaseModel):
    type: ConfigType


class ConfigSetConfigPostParametersQuery(ConfigGetConfigGetParametersQuery):
    pass


class BackupsGetResponse1(BaseModel):
    name: Optional[str] = Field(None, description='The name of the backup archive')
    day: Optional[date] = Field(None, description='The date (day) of backup')


class BackupsGetResponse(RootModel[List[BackupsGetResponse1]]):
    root: List[BackupsGetResponse1]
