# DDS Provider Testing

This directory contains comprehensive integration tests for the DDS Provider service.

## Architecture

The testing setup tests against the **real dds-provider service**:

```
┌─────────────────┐    HTTP API     ┌─────────────────┐
│   Test Suite   │ ──────────────→ │  dds-provider   │
│   (pytest)     │                 │  Port 8080      │
└─────────────────┘                 └─────────────────┘
```

The tests use the actual `.proto` API definitions and test the real service behavior.

## Quick Start

### 1. Start the dds-provider Service

Start your dds-provider service on port 8080:

```bash
cd ..
devspace deploy
```

Or run it directly:

```bash
go run main.go -debug
```

### 2. Run the Tests

```bash
# Run all tests
pdm run pytest

# Run specific test categories
pdm run pytest tests/integration/test_core_services.py
pdm run pytest tests/integration/test_device_service.py
pdm run pytest tests/integration/test_sensor_service.py
pdm run pytest tests/integration/test_jammer_service.py
pdm run pytest tests/integration/test_camera.py
pdm run pytest tests/integration/test_target_service.py

# Run with verbose output
pdm run pytest -v

# Run with coverage
pdm run pytest --cov=integration

# Test against a different URL
pdm run pytest --url http://localhost:9000
```

## Test Structure

### Core Services (`test_core_services.py`)
- Basic API connectivity
- Response structure validation
- Settings and configuration

### Device Service (`test_device_service.py`)
- Device enable/disable operations
- Position setting and validation
- Position mode configuration
- Round-trip validation

### Sensor Service (`test_sensor_service.py`)
- Sensor information retrieval
- Jammer mode configuration
- Timeout validation
- Dynamic sensor data

### Jammer Service (`test_jammer_service.py`)
- Jammer information retrieval
- Band configuration
- Group management
- Duration validation

### Camera Service (`test_camera.py`)
- Camera ID resolution
- Sensor validation
- Response structure validation

### Target Service (`test_target_service.py`)
- Target data retrieval
- Streaming behavior
- Data consistency
- Attribute validation

### Configuration (`test_config.py`)
- Configuration persistence
- Round-trip validation
- State management

### Device Control (`test_device_control.py`)
- Workflow testing
- Edge case handling
- State transitions

### Validation (`test_validation.py`)
- Input validation across all services
- UUID validation
- Enum validation
- Range validation
- Required field validation

## Configuration

### Environment Variables

- `SERVICE_URL`: Override the default service URL
- `LOG_LEVEL`: Set logging level for tests

### Test Fixtures

- `valid_sensor_id`: UUID for testing sensor operations
- `valid_jammer_id`: UUID for testing jammer operations  
- `valid_device_id`: UUID for testing device operations
- `client`: HTTP client for API communication

## Troubleshooting

### Common Issues

1. **Service Not Running**
   ```bash
   # Check if service is running
   curl http://localhost:8080/api.v1.CommonService/Settings
   
   # Start the service if needed
   cd ..
   devspace deploy
   ```

2. **Tests Failing with Connection Errors**
   - Ensure dds-provider service is running on port 8080
   - Check firewall settings
   - Verify network configuration

3. **Proto API Mismatches**
   - Ensure tests match the current `.proto` definitions
   - Regenerate models if needed: `python3 tests/scripts/generate_models.py`

### Debug Mode

Run tests with verbose output to see detailed error information:

```bash
pdm run pytest -v --tb=long
```

## Adding New Tests

### 1. Create Test File

```python
# tests/integration/test_new_service.py
import pytest

async def test_new_endpoint(client):
    """Test new endpoint functionality"""
    result = await client.new_method()
    assert isinstance(result, dict)
    assert "expected_field" in result
```

### 2. Update Client

Add the method to `dds_client.py`:

```python
async def new_method(self) -> Dict[str, Any]:
    return await self._request("POST", "api.v1.NewService/NewMethod")
```

### 3. Update Proto Definitions

If adding new endpoints, update the `.proto` files in `api/proto/` and regenerate the Go code:

```bash
devspace gen
```

## Continuous Integration

The test suite is designed to work in CI environments:

- Uses pytest for reliable test execution
- Includes proper cleanup and teardown
- Handles connection failures gracefully
- Provides clear error reporting
- Tests against real service behavior

## Performance

- Tests run in parallel using pytest-asyncio
- Tests real API performance and behavior
- Minimal resource usage
- Fast test execution (< 30 seconds for full suite)

## API Coverage

The test suite covers all major API endpoints defined in the `.proto` files:

- **CommonService**: Settings, LicenseStatus
- **SensorService**: Sensors, SensorInfoDynamic, SetJammerMode
- **JammerService**: Jammers, JammerInfoDynamic, Groups, SetJammerBands
- **DeviceService**: SetDisabled, SetPosition, SetPositionMode
- **CameraService**: CameraId
- **TargetService**: Targets (with streaming support)

All tests validate the actual API responses and ensure the service behaves according to the protobuf specifications.
