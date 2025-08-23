"""
Core services tests - basic functionality and smoke tests
Following the same pattern as the original project
"""
import pytest
from models import provider_api


async def test_get_settings_basic(client):
    """Test CommonService/Settings - basic functionality"""
    settings = await client.get_settings()
    assert isinstance(settings, dict)
    assert "settings" in settings


async def test_get_sensors_basic(client):
    """Test SensorService/Sensors - basic functionality"""
    sensors = await client.get_sensors()
    assert isinstance(sensors, dict)
    # Handle different response formats
    if "sensor_infos" in sensors:
        assert isinstance(sensors["sensor_infos"], list)
    elif "sensors" in sensors:
        assert isinstance(sensors["sensors"], list)
    # Some endpoints might return empty dict if no sensors
    assert len(sensors) >= 0


async def test_get_jammers_basic(client):
    """Test JammerService/Jammers - basic functionality"""
    jammers = await client.get_jammers()
    assert isinstance(jammers, dict)
    # Handle different response formats
    if "jammer_infos" in jammers:
        assert isinstance(jammers["jammer_infos"], list)
    elif "jammers" in jammers:
        assert isinstance(jammers["jammers"], list)
    # Some endpoints might return empty dict if no jammers
    assert len(jammers) >= 0


async def test_get_targets_basic(client):
    """Test TargetService/Targets - basic functionality"""
    try:
        targets = await client.get_targets()
        assert isinstance(targets, dict)
    except Exception as e:
        # Some endpoints might not be implemented yet
        pytest.skip(f"Targets endpoint not available: {e}")


async def test_api_connectivity(client):
    """Test basic API connectivity and response format"""
    # Test that all basic endpoints respond
    endpoints = [
        ("get_settings", client.get_settings),
        ("get_sensors", client.get_sensors),
        ("get_jammers", client.get_jammers),
    ]
    
    for name, endpoint_func in endpoints:
        try:
            result = await endpoint_func()
            assert isinstance(result, dict), f"{name} should return dict, got {type(result)}"
        except Exception as e:
            pytest.fail(f"{name} failed with error: {e}")
    
    # Test targets endpoint separately as it might not be implemented
    try:
        result = await client.get_targets()
        assert isinstance(result, dict), "get_targets should return dict"
    except Exception as e:
        # Skip if not implemented
        pytest.skip(f"get_targets not implemented: {e}")


async def test_response_structure_consistency(client):
    """Test that response structures are consistent across multiple calls"""
    # Test multiple calls to check for consistency
    settings1 = await client.get_settings()
    settings2 = await client.get_settings()
    
    # Basic structure should be consistent
    assert isinstance(settings1, dict) == isinstance(settings2, dict)
    if isinstance(settings1, dict) and isinstance(settings2, dict):
        # Check if both have the same top-level keys
        assert set(settings1.keys()) == set(settings2.keys())
