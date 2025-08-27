from pydantic import BaseModel
# from httpx import HTTPStatusError

class EmptyResponse(BaseModel):
    pass

class BadRequest(BaseException):
    pass

class UnprocessableEntity(BaseException):
    pass
