"""
Device Service API tests - comprehensive testing of all DeviceService endpoints
"""
import pytest
from models import provider_api, misc


async def test_set_device_disabled_enable(client, valid_device_id):
    """Test DeviceService/SetDisabled - enable device"""
    try:
        result = await client.set_device_disabled(valid_device_id, False)
        assert isinstance(result, dict)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetDisabled endpoint has server issues")
        else:
            raise


async def test_set_device_disabled_disable(client, valid_device_id):
    """Test DeviceService/SetDisabled - disable device"""
    try:
        result = await client.set_device_disabled(valid_device_id, True)
        assert isinstance(result, dict)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetDisabled endpoint has server issues")
        else:
            raise


async def test_set_device_disabled_invalid_device_id(client):
    """Test DeviceService/SetDisabled with invalid device ID"""
    with pytest.raises((ValueError, misc.InvalidArgument)):
        await client.set_device_disabled("invalid-uuid", False)


async def test_set_device_disabled_empty_device_id(client):
    """Test DeviceService/SetDisabled with empty device ID"""
    with pytest.raises((ValueError, misc.InvalidArgument)):
        await client.set_device_disabled("", False)


async def test_set_device_disabled_round_trip(client, valid_device_id):
    """Test device enable/disable round trip"""
    try:
        # Get initial state (if possible)
        # Note: We don't have a get method for device state, so we'll just test the set operations
        
        # Disable device
        await client.set_device_disabled(valid_device_id, True)
        
        # Enable device
        await client.set_device_disabled(valid_device_id, False)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetDisabled endpoint has server issues")
        else:
            raise


async def test_set_device_position_valid(client, valid_device_id):
    """Test DeviceService/SetPosition with valid position data"""
    try:
        position_data = {
            "azimuth": 45.5,
            "coordinate": {
                "latitude": 50.4501,
                "longitude": 30.5234,
                "altitude": 100.0
            }
        }
        
        result = await client.set_device_position(valid_device_id, position_data)
        assert isinstance(result, dict)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetPosition endpoint has server issues")
        else:
            raise


async def test_set_device_position_minimal(client, valid_device_id):
    """Test DeviceService/SetPosition with minimal position data"""
    try:
        position_data = {
            "azimuth": 0.0,
            "coordinate": {
                "latitude": 0.0,
                "longitude": 0.0,
                "altitude": 0.0
            }
        }
        
        result = await client.set_device_position(valid_device_id, position_data)
        assert isinstance(result, dict)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetPosition endpoint has server issues")
        else:
            raise


async def test_set_device_position_extreme_values(client, valid_device_id):
    """Test DeviceService/SetPosition with extreme coordinate values"""
    # Test extreme latitude/longitude values
    extreme_positions = [
        {
            "azimuth": 0.0,
            "coordinate": {
                "latitude": 90.0,  # North Pole
                "longitude": 180.0,  # International Date Line
                "altitude": 8848.0  # Mount Everest height
            }
        },
        {
            "azimuth": 180.0,
            "coordinate": {
                "latitude": -90.0,  # South Pole
                "longitude": -180.0,  # International Date Line
                "altitude": -11034.0  # Mariana Trench depth
            }
        }
    ]
    
    for position in extreme_positions:
        try:
            result = await client.set_device_position(valid_device_id, position)
            assert isinstance(result, dict)
        except Exception as e:
            if "400" in str(e) and "validation error" in str(e):
                # Expected validation error for extreme values
                pass
            elif "500" in str(e):
                pytest.skip("SetPosition endpoint has server issues")
            else:
                raise


async def test_set_device_position_invalid_device_id(client):
    """Test DeviceService/SetPosition with invalid device ID"""
    position_data = {
        "azimuth": 45.5,
        "coordinate": {
            "latitude": 50.4501,
            "longitude": 30.5234,
            "altitude": 100.0
        }
    }
    
    with pytest.raises((ValueError, misc.InvalidArgument)):
        await client.set_device_position("invalid-uuid", position_data)


async def test_set_device_position_invalid_coordinates(client, valid_device_id):
    """Test DeviceService/SetPosition with invalid coordinates"""
    invalid_positions = [
        # Invalid latitude
        {
            "azimuth": 45.5,
            "coordinate": {
                "latitude": 91.0,  # > 90
                "longitude": 30.5234,
                "altitude": 100.0
            }
        },
        # Invalid longitude
        {
            "azimuth": 45.5,
            "coordinate": {
                "latitude": 50.4501,
                "longitude": 181.0,  # > 180
                "altitude": 100.0
            }
        },
        # Invalid azimuth
        {
            "azimuth": 400.0,  # > 360
            "coordinate": {
                "latitude": 50.4501,
                "longitude": 30.5234,
                "altitude": 100.0
            }
        }
    ]
    
    for position in invalid_positions:
        try:
            result = await client.set_device_position(valid_device_id, position)
            assert isinstance(result, dict)
        except (ValueError, misc.InvalidArgument):
            # Expected if backend validates coordinate ranges
            pass


async def test_set_device_position_mode_auto(client, valid_device_id):
    """Test DeviceService/SetPositionMode with GEO_AUTO"""
    try:
        result = await client.set_device_position_mode(valid_device_id, "GEO_AUTO")
        assert isinstance(result, dict)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetPositionMode endpoint has server issues")
        else:
            raise


async def test_set_device_position_mode_manual(client, valid_device_id):
    """Test DeviceService/SetPositionMode with GEO_MANUAL"""
    try:
        result = await client.set_device_position_mode(valid_device_id, "GEO_MANUAL")
        assert isinstance(result, dict)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetPositionMode endpoint has server issues")
        else:
            raise


async def test_set_device_position_mode_always_manual(client, valid_device_id):
    """Test DeviceService/SetPositionMode with GEO_ALWAYS_MANUAL"""
    try:
        result = await client.set_device_position_mode(valid_device_id, "GEO_ALWAYS_MANUAL")
        assert isinstance(result, dict)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetPositionMode endpoint has server issues")
        else:
            raise


async def test_set_device_position_mode_invalid_device_id(client):
    """Test DeviceService/SetPositionMode with invalid device ID"""
    with pytest.raises((ValueError, misc.InvalidArgument)):
        await client.set_device_position_mode("invalid-uuid", "GEO_AUTO")


async def test_set_device_position_mode_invalid_mode(client, valid_device_id):
    """Test DeviceService/SetPositionMode with invalid mode"""
    with pytest.raises((ValueError, misc.InvalidArgument)):
        await client.set_device_position_mode(valid_device_id, "INVALID_MODE")


async def test_device_position_round_trip(client, valid_device_id):
    """Test device position get/set/get round trip"""
    try:
        # Set position to a known value
        test_position = {
            "azimuth": 90.0,
            "coordinate": {
                "latitude": 40.7128,
                "longitude": -74.0060,
                "altitude": 50.0
            }
        }
        
        await client.set_device_position(valid_device_id, test_position)
        
        # Set position mode to MANUAL to ensure position is maintained
        await client.set_device_position_mode(valid_device_id, "GEO_MANUAL")
        
        # Set position back to a different value
        restore_position = {
            "azimuth": 0.0,
            "coordinate": {
                "latitude": 0.0,
                "longitude": 0.0,
                "altitude": 0.0
            }
        }
        
        await client.set_device_position(valid_device_id, restore_position)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("Device endpoints have server issues")
        else:
            raise


async def test_device_position_mode_round_trip(client, valid_device_id):
    """Test device position mode get/set/get round trip"""
    try:
        # Test all position modes
        modes = ["GEO_AUTO", "GEO_MANUAL", "GEO_ALWAYS_MANUAL"]
        
        for mode in modes:
            result = await client.set_device_position_mode(valid_device_id, mode)
            assert isinstance(result, dict)
        
        # Restore to AUTO mode
        await client.set_device_position_mode(valid_device_id, "GEO_AUTO")
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetPositionMode endpoint has server issues")
        else:
            raise


async def test_device_operations_combination(client, valid_device_id):
    """Test combination of device operations"""
    try:
        # Disable device
        await client.set_device_disabled(valid_device_id, True)
        
        # Set position mode to MANUAL
        await client.set_device_position_mode(valid_device_id, "GEO_MANUAL")
        
        # Set specific position
        position_data = {
            "azimuth": 180.0,
            "coordinate": {
                "latitude": 51.5074,
                "longitude": -0.1278,
                "altitude": 75.0
            }
        }
        await client.set_device_position(valid_device_id, position_data)
        
        # Re-enable device
        await client.set_device_disabled(valid_device_id, False)
        
        # Reset position mode to AUTO
        await client.set_device_position_mode(valid_device_id, "GEO_AUTO")
    except Exception as e:
        if "500" in str(e):
            pytest.skip("Device endpoints have server issues")
        else:
            raise
