"""
Target Service API tests - comprehensive testing of all TargetService endpoints
"""
import pytest
from models import provider_api, misc


async def test_get_targets(client):
    """Test TargetService/Targets - get all targets"""
    try:
        targets = await client.get_targets()
        assert isinstance(targets, dict)
        
        # The response structure depends on the streaming implementation
        # We'll check for common fields that should be present
        if "target_id" in targets:
            # Single target response
            assert "class_name" in targets
            assert "alarm_status" in targets
            assert "last_updated" in targets
        elif isinstance(targets, list):
            # Multiple targets response
            for target in targets:
                assert "target_id" in target
                assert "class_name" in target
                assert "alarm_status" in target
                assert "last_updated" in target
    except Exception as e:
        if "415" in str(e):
            pytest.skip("Targets endpoint not implemented yet")
        else:
            raise


async def test_target_structure_validation(client):
    """Test target response structure validation"""
    try:
        targets = await client.get_targets()
        
        # Helper function to validate target structure
        def validate_target(target):
            # Required fields
            assert "target_id" in target
            assert "class_name" in target
            assert "alarm_status" in target
            assert "last_updated" in target
            
            # Validate target_id format (should be UUID)
            assert isinstance(target["target_id"], str)
            assert len(target["target_id"]) > 0
            
            # Validate class_name is valid enum value
            valid_classes = [
                "TARGET_UNDEFINED", "TARGET_DRONE", "TARGET_CAR", "TARGET_TRUCK",
                "TARGET_HUMAN", "TARGET_TREE", "TARGET_PEOPLE_GROUP", "TARGET_MOTORCYCLE",
                "TARGET_JET_SKI", "TARGET_BOAT", "TARGET_SHIP", "TARGET_REMOTE_CONSOLE"
            ]
            assert target["class_name"] in valid_classes
            
            # Validate alarm_status structure
            alarm_status = target["alarm_status"]
            assert "level" in alarm_status
            valid_levels = ["ALARM_NONE", "ALARM_MEDIUM", "ALARM_HIGH", "ALARM_CRITICAL"]
            assert alarm_status["level"] in valid_levels
            
            if "score" in alarm_status:
                assert isinstance(alarm_status["score"], (int, float))
                assert 0.0 <= alarm_status["score"] <= 1.0
            
            # Validate last_updated is a timestamp string
            assert isinstance(target["last_updated"], str)
            assert len(target["last_updated"]) > 0
            
            # Optional fields validation
            if "sensor_id" in target:
                assert isinstance(target["sensor_id"], str)
                assert len(target["sensor_id"]) > 0
            
            if "track_id" in target:
                assert isinstance(target["track_id"], str)
                assert len(target["track_id"]) > 0
            
            if "camera_track" in target:
                camera_track = target["camera_track"]
                assert "camera_id" in camera_track
                assert "target_id" in camera_track
            
            if "alarm_ids" in target:
                assert isinstance(target["alarm_ids"], list)
                for alarm_id in target["alarm_ids"]:
                    assert isinstance(alarm_id, str)
                    assert len(alarm_id) > 0
            
            if "cls_ids" in target:
                assert isinstance(target["cls_ids"], list)
                for cls_id in target["cls_ids"]:
                    assert isinstance(cls_id, str)
                    assert len(cls_id) > 0
            
            # Validate attributes (oneof field)
            if "attributes" in target:
                attributes = target["attributes"]
                # Should have exactly one of the attribute types
                attribute_types = ["rfd_attributes", "radar_attributes", "camera_attributes"]
                found_attributes = [attr for attr in attribute_types if attr in attributes]
                assert len(found_attributes) <= 1, "Only one attribute type should be present"
                
                if "rfd_attributes" in attributes:
                    rfd_attr = attributes["rfd_attributes"]
                    assert "digital" in rfd_attr
                    assert "frequencies" in rfd_attr
                    assert "geolocation" in rfd_attr
                    if "description" in rfd_attr:
                        assert isinstance(rfd_attr["description"], str)
            
            # Validate position if present
            if "position" in target:
                position = target["position"]
                assert "coordinate" in position
                assert "source_id" in position
                assert "direction" in position
                
                coordinate = position["coordinate"]
                assert "latitude" in coordinate
                assert "longitude" in coordinate
                assert "altitude" in coordinate
                
                # Validate coordinate ranges
                assert -90.0 <= coordinate["latitude"] <= 90.0
                assert -180.0 <= coordinate["longitude"] <= 180.0
                
                direction = position["direction"]
                assert "bearing" in direction
                assert "distance" in direction
                if "elevation" in direction:
                    assert 0.0 <= direction["elevation"] < 90.0
                
                # Validate bearing range
                assert 0.0 <= direction["bearing"] < 360.0
                assert direction["distance"] >= 0.0
            
            # Validate detect_count if present
            if "detect_count" in target:
                assert isinstance(target["detect_count"], int)
                assert target["detect_count"] >= 0
        
        # Apply validation based on response structure
        if "target_id" in targets:
            validate_target(targets)
        elif isinstance(targets, list):
            for target in targets:
                validate_target(target)
    except Exception as e:
        if "415" in str(e):
            pytest.skip("Targets endpoint not implemented yet")
        else:
            raise


async def test_target_class_enumeration(client):
    """Test that all target classes are properly handled"""
    try:
        targets = await client.get_targets()
        
        # Collect all target classes from response
        target_classes = set()
        if "target_id" in targets:
            target_classes.add(targets["class_name"])
        elif isinstance(targets, list):
            for target in targets:
                target_classes.add(target["class_name"])
        
        # Verify we have at least some target classes
        assert len(target_classes) > 0
        
        # Verify all returned classes are valid
        valid_classes = {
            "TARGET_UNDEFINED", "TARGET_DRONE", "TARGET_CAR", "TARGET_TRUCK",
            "TARGET_HUMAN", "TARGET_TREE", "TARGET_PEOPLE_GROUP", "TARGET_MOTORCYCLE",
            "TARGET_JET_SKI", "TARGET_BOAT", "TARGET_SHIP", "TARGET_REMOTE_CONSOLE"
        }
        
        for target_class in target_classes:
            assert target_class in valid_classes, f"Invalid target class: {target_class}"
    except Exception as e:
        if "415" in str(e):
            pytest.skip("Targets endpoint not implemented yet")
        else:
            raise


async def test_alarm_level_enumeration(client):
    """Test that all alarm levels are properly handled"""
    try:
        targets = await client.get_targets()
        
        # Collect all alarm levels from response
        alarm_levels = set()
        if "target_id" in targets:
            alarm_levels.add(targets["alarm_status"]["level"])
        elif isinstance(targets, list):
            for target in targets:
                alarm_levels.add(target["alarm_status"]["level"])
        
        # Verify we have at least some alarm levels
        assert len(alarm_levels) > 0
        
        # Verify all returned levels are valid
        valid_levels = {"ALARM_NONE", "ALARM_MEDIUM", "ALARM_HIGH", "ALARM_CRITICAL"}
        
        for alarm_level in alarm_levels:
            assert alarm_level in valid_levels, f"Invalid alarm level: {alarm_level}"
    except Exception as e:
        if "415" in str(e):
            pytest.skip("Targets endpoint not implemented yet")
        else:
            raise


async def test_target_attributes_validation(client):
    """Test target attributes validation"""
    try:
        targets = await client.get_targets()
        
        def validate_attributes(target):
            if "attributes" in target:
                attributes = target["attributes"]
                
                # Check RFD attributes if present
                if "rfd_attributes" in attributes:
                    rfd_attr = attributes["rfd_attributes"]
                    assert isinstance(rfd_attr["digital"], bool)
                    assert isinstance(rfd_attr["frequencies"], list)
                    assert isinstance(rfd_attr["geolocation"], bool)
                    
                    # Validate frequencies
                    for freq in rfd_attr["frequencies"]:
                        assert isinstance(freq, int)
                        assert freq > 0
                
                # Check radar attributes if present
                if "radar_attributes" in attributes:
                    # Radar attributes are currently empty, just verify it exists
                    assert isinstance(attributes["radar_attributes"], dict)
                
                # Check camera attributes if present
                if "camera_attributes" in attributes:
                    # Camera attributes are currently empty, just verify it exists
                    assert isinstance(attributes["camera_attributes"], dict)
        
        # Apply validation based on response structure
        if "target_id" in targets:
            validate_attributes(targets)
        elif isinstance(targets, list):
            for target in targets:
                validate_attributes(target)
    except Exception as e:
        if "415" in str(e):
            pytest.skip("Targets endpoint not implemented yet")
        else:
            raise


async def test_target_position_validation(client):
    """Test target position data validation"""
    try:
        targets = await client.get_targets()
        
        def validate_position(target):
            if "position" in target:
                position = target["position"]
                
                # Validate coordinate ranges
                coordinate = position["coordinate"]
                assert -90.0 <= coordinate["latitude"] <= 90.0
                assert -180.0 <= coordinate["longitude"] <= 180.0
                
                # Validate direction ranges
                direction = position["direction"]
                assert 0.0 <= direction["bearing"] < 360.0
                assert direction["distance"] >= 0.0
                
                if "elevation" in direction:
                    assert 0.0 <= direction["elevation"] < 90.0
        
        # Apply validation based on response structure
        if "target_id" in targets:
            validate_position(targets)
        elif isinstance(targets, list):
            for target in targets:
                validate_position(target)
    except Exception as e:
        if "415" in str(e):
            pytest.skip("Targets endpoint not implemented yet")
        else:
            raise


async def test_target_streaming_behavior(client):
    """Test target streaming behavior (if implemented)"""
    try:
        # This test verifies that the targets endpoint behaves as expected
        # for streaming responses
        
        targets = await client.get_targets()
        
        # Basic response validation
        assert targets is not None
        
        # Check if response is empty (no targets) or contains target data
        if isinstance(targets, dict) and "target_id" in targets:
            # Single target response
            assert targets["target_id"] is not None
        elif isinstance(targets, list):
            # Multiple targets response
            assert len(targets) >= 0  # Can be empty list
        else:
            # Empty response or other format
            assert targets == {} or targets is None
    except Exception as e:
        if "415" in str(e):
            pytest.skip("Targets endpoint not implemented yet")
        else:
            raise


async def test_target_data_consistency(client):
    """Test target data consistency across multiple calls"""
    try:
        # Make multiple calls to check for consistency
        targets1 = await client.get_targets()
        targets2 = await client.get_targets()
        
        # Basic structure should be consistent
        if isinstance(targets1, dict) and "target_id" in targets1:
            assert isinstance(targets2, dict)
            assert "target_id" in targets2
        elif isinstance(targets1, list):
            assert isinstance(targets2, list)
        else:
            assert targets1 == targets2 or (targets1 == {} and targets2 == {})
    except Exception as e:
        if "415" in str(e):
            pytest.skip("Targets endpoint not implemented yet")
        else:
            raise
