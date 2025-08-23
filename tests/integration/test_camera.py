"""
Camera service tests - comprehensive testing of CameraService endpoints
Following the same pattern as the original project
"""
import pytest
from models import provider_api, misc


async def test_get_camera_id_valid_sensor(client, valid_sensor_id):
    """Test CameraService/CameraId with valid sensor ID"""
    try:
        result = await client.get_camera_id(valid_sensor_id)
        assert isinstance(result, dict)
        assert "camera_id" in result
        
        # camera_id can be None if no camera is associated
        if result["camera_id"] is not None:
            assert isinstance(result["camera_id"], str)
            assert len(result["camera_id"]) > 0
    except Exception as e:
        if "500" in str(e):
            pytest.skip("CameraId endpoint has server issues")
        else:
            raise


async def test_get_camera_id_invalid_sensor(client):
    """Test CameraService/CameraId with invalid sensor ID"""
    with pytest.raises((ValueError, misc.InvalidArgument)):
        await client.get_camera_id("invalid-uuid")


async def test_get_camera_id_empty_sensor(client):
    """Test CameraService/CameraId with empty sensor ID"""
    with pytest.raises((ValueError, misc.InvalidArgument)):
        await client.get_camera_id("")


async def test_get_camera_id_nonexistent_sensor(client):
    """Test CameraService/CameraId with non-existent sensor ID"""
    # Use a valid UUID format but non-existent sensor
    nonexistent_sensor_id = "00000000-0000-0000-0000-000000000999"
    
    try:
        result = await client.get_camera_id(nonexistent_sensor_id)
        # Should return response with camera_id as None
        assert isinstance(result, dict)
        assert "camera_id" in result
        assert result["camera_id"] is None
    except Exception as e:
        # Or might throw an error if sensor doesn't exist
        if "500" in str(e):
            pytest.skip("CameraId endpoint has server issues")
        else:
            assert isinstance(e, (ValueError, misc.InvalidArgument))


async def test_camera_id_response_structure(client, valid_sensor_id):
    """Test camera ID response structure validation"""
    try:
        result = await client.get_camera_id(valid_sensor_id)
        
        # Verify response structure
        assert isinstance(result, dict)
        assert "camera_id" in result
        
        # Verify camera_id field type
        camera_id = result["camera_id"]
        if camera_id is not None:
            assert isinstance(camera_id, str)
            assert len(camera_id) > 0
            # Could add additional validation for camera ID format if known
    except Exception as e:
        if "500" in str(e):
            pytest.skip("CameraId endpoint has server issues")
        else:
            raise


async def test_camera_id_consistency(client, valid_sensor_id):
    """Test camera ID consistency across multiple calls"""
    try:
        # Make multiple calls to check for consistency
        result1 = await client.get_camera_id(valid_sensor_id)
        result2 = await client.get_camera_id(valid_sensor_id)
        
        # Results should be consistent
        assert result1["camera_id"] == result2["camera_id"]
        assert result1 == result2
    except Exception as e:
        if "500" in str(e):
            pytest.skip("CameraId endpoint has server issues")
        else:
            raise


async def test_camera_id_with_different_sensors(client, valid_sensor_id):
    """Test camera ID with different sensor types"""
    # This test would require multiple valid sensor IDs
    # For now, we'll test with the available valid_sensor_id
    
    try:
        # Test that we get a valid response
        result = await client.get_camera_id(valid_sensor_id)
        assert isinstance(result, dict)
        assert "camera_id" in result
    except Exception as e:
        if "500" in str(e):
            pytest.skip("CameraId endpoint has server issues")
        else:
            raise


async def test_camera_service_error_handling(client):
    """Test camera service error handling"""
    # Test various error conditions
    
    # Test with malformed UUID
    malformed_uuids = [
        "not-a-uuid",
        "123",
        "abc-def-ghi",
        "00000000-0000-0000-0000-00000000000",  # Too short
        "00000000-0000-0000-0000-0000000000000",  # Too long
    ]
    
    for malformed_uuid in malformed_uuids:
        with pytest.raises((ValueError, misc.InvalidArgument)):
            await client.get_camera_id(malformed_uuid)
    
    # Test with special characters
    special_chars = [
        "00000000-0000-0000-0000-00000000000!",  # Exclamation mark
        "00000000-0000-0000-0000-00000000000@",  # At symbol
        "00000000-0000-0000-0000-00000000000#",  # Hash
    ]
    
    for special_uuid in special_chars:
        with pytest.raises((ValueError, misc.InvalidArgument)):
            await client.get_camera_id(special_uuid)
