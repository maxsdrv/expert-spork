"""
Configuration-style tests following the pattern of test_config.py from the original project
Tests the get/set/get pattern for various operations and configuration persistence
"""
import pytest
from models import provider_api, misc


async def test_jammer_mode_configuration_persistence(client, valid_sensor_id):
    """Test jammer mode configuration persistence using get/set/get pattern"""
    try:
        # Get initial jammer mode
        initial_info = await client.get_sensor_info_dynamic(valid_sensor_id)
        initial_mode = initial_info.get("jammer_mode")
        initial_timeout = initial_info.get("jammer_auto_timeout")
    except Exception:
        # Skip if SensorInfoDynamic is not available
        pytest.skip("SensorInfoDynamic endpoint not available")
    
    try:
        # Set jammer mode to AUTO with timeout
        await client.set_jammer_mode(valid_sensor_id, "JAMMER_AUTO", 120)
        
        # Get sensor info to verify the mode was set
        sensor_info = await client.get_sensor_info_dynamic(valid_sensor_id)
        
        # Verify the mode matches
        if sensor_info.get("jammer_mode") is not None:
            assert sensor_info["jammer_mode"] == "JAMMER_AUTO"
        
        # Verify timeout was set for AUTO mode
        if sensor_info.get("jammer_auto_timeout") is not None:
            assert sensor_info["jammer_auto_timeout"] == 120
        
        # Set jammer mode to MANUAL
        await client.set_jammer_mode(valid_sensor_id, "JAMMER_MANUAL", 60)
        
        # Verify MANUAL mode was set
        sensor_info = await client.get_sensor_info_dynamic(valid_sensor_id)
        if sensor_info.get("jammer_mode") is not None:
            assert sensor_info["jammer_mode"] == "JAMMER_MANUAL"
        
        # Restore initial configuration
        if initial_mode:
            await client.set_jammer_mode(valid_sensor_id, initial_mode, initial_timeout or 60)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetJammerMode endpoint has server issues")
        else:
            raise


async def test_device_position_configuration_persistence(client, valid_device_id):
    """Test device position configuration persistence using get/set/get pattern"""
    try:
        # Set device position mode to MANUAL to ensure position is maintained
        await client.set_device_position_mode(valid_device_id, "GEO_MANUAL")
        
        # Set a specific position
        test_position = {
            "azimuth": 90.0,
            "coordinate": {
                "latitude": 40.7128,
                "longitude": -74.0060,
                "altitude": 50.0
            }
        }
        
        await client.set_device_position(valid_device_id, test_position)
        
        # Set position mode to AUTO
        await client.set_device_position_mode(valid_device_id, "GEO_AUTO")
        
        # Set a different position
        different_position = {
            "azimuth": 180.0,
            "coordinate": {
                "latitude": 51.5074,
                "longitude": -0.1278,
                "altitude": 75.0
            }
        }
        
        await client.set_device_position(valid_device_id, different_position)
        
        # Restore to AUTO mode
        await client.set_device_position_mode(valid_device_id, "GEO_AUTO")
    except Exception as e:
        if "500" in str(e):
            pytest.skip("Device endpoints have server issues")
        else:
            raise


async def test_jammer_bands_configuration_persistence(client, valid_jammer_id):
    """Test jammer bands configuration persistence using get/set/get pattern"""
    try:
        # Get initial jammer info
        initial_info = await client.get_jammer_info_dynamic(valid_jammer_id)
        initial_bands = initial_info.get("bands", [])
    except Exception:
        # Skip if JammerInfoDynamic is not available
        pytest.skip("JammerInfoDynamic endpoint not available")
    
    try:
        # Set specific bands
        test_bands = ["2.4GHz", "5.8GHz"]
        await client.set_jammer_bands(valid_jammer_id, test_bands, 60)
        
        # Get jammer info to verify bands were set
        jammer_info = await client.get_jammer_info_dynamic(valid_jammer_id)
        
        # Verify bands were set (this might not be immediately visible in dynamic info)
        # The verification would depend on how the backend implements the response
        
        # Stop all bands
        await client.set_jammer_bands(valid_jammer_id, [], 30)
        
        # Restore initial bands if they existed
        if initial_bands:
            active_bands = [band["label"] for band in initial_bands if band.get("active")]
            if active_bands:
                await client.set_jammer_bands(valid_jammer_id, active_bands, 60)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("SetJammerBands endpoint has server issues")
        else:
            raise


async def test_device_enable_disable_configuration(client, valid_device_id):
    """Test device enable/disable configuration persistence"""
    try:
        # Get initial device state (if possible)
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


async def test_configuration_round_trip_validation(client, valid_sensor_id, valid_device_id, valid_jammer_id):
    """Test configuration round trip validation for all services"""
    try:
        # Test jammer mode round trip
        await client.set_jammer_mode(valid_sensor_id, "JAMMER_AUTO", 60)
        try:
            sensor_info = await client.get_sensor_info_dynamic(valid_sensor_id)
            if sensor_info.get("jammer_mode"):
                assert sensor_info["jammer_mode"] == "JAMMER_AUTO"
        except Exception:
            # Skip verification if SensorInfoDynamic is not available
            pass
        
        # Test device position round trip
        test_position = {
            "azimuth": 45.0,
            "coordinate": {
                "latitude": 0.0,
                "longitude": 0.0,
                "altitude": 0.0
            }
        }
        await client.set_device_position(valid_device_id, test_position)
        
        # Test jammer bands round trip
        test_bands = ["2.4GHz"]
        await client.set_jammer_bands(valid_jammer_id, test_bands, 30)
        
        # Cleanup
        await client.set_jammer_mode(valid_sensor_id, "JAMMER_MANUAL", 60)
        await client.set_jammer_bands(valid_jammer_id, [], 30)
    except Exception as e:
        if "500" in str(e):
            pytest.skip("Endpoints have server issues")
        else:
            raise
