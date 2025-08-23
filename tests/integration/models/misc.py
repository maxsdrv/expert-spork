"""
Miscellaneous models and exceptions for DDS Provider API
"""

from pydantic import BaseModel


class EmptyResponse(BaseModel):
    """Empty response model for void operations"""
    pass


# Connect/gRPC specific exceptions
class ConnectError(Exception):
    """Base class for Connect/gRPC errors"""
    def __init__(self, code: str, message: str, details=None):
        self.code = code
        self.message = message
        self.details = details or []
        super().__init__(f"{code}: {message}")


class InvalidArgument(ConnectError):
    """INVALID_ARGUMENT error (equivalent to HTTP 400)"""
    def __init__(self, message: str, details=None):
        super().__init__("INVALID_ARGUMENT", message, details)


class NotFound(ConnectError):
    """NOT_FOUND error (equivalent to HTTP 404)"""
    def __init__(self, message: str, details=None):
        super().__init__("NOT_FOUND", message, details)


class Unauthenticated(ConnectError):
    """UNAUTHENTICATED error (equivalent to HTTP 401)"""
    def __init__(self, message: str, details=None):
        super().__init__("UNAUTHENTICATED", message, details)


class PermissionDenied(ConnectError):
    """PERMISSION_DENIED error (equivalent to HTTP 403)"""
    def __init__(self, message: str, details=None):
        super().__init__("PERMISSION_DENIED", message, details)


class Internal(ConnectError):
    """INTERNAL error (equivalent to HTTP 500)"""
    def __init__(self, message: str, details=None):
        super().__init__("INTERNAL", message, details)


class Unavailable(ConnectError):
    """UNAVAILABLE error (equivalent to HTTP 503)"""
    def __init__(self, message: str, details=None):
        super().__init__("UNAVAILABLE", message, details)


# Backward compatibility aliases
BadRequest = InvalidArgument
UnprocessableEntity = InvalidArgument  # For validation errors
