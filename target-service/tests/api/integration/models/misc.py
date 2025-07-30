from pydantic import BaseModel
from .http_api import Error
from httpx import HTTPStatusError

class EmptyResponse(BaseModel):
    pass

class APIError(BaseException):
    def __init__(self, error):
        self.error = error

class BadRequest(BaseException):
    pass
