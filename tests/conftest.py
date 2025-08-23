"""
Test configuration and fixtures
"""
import pytest
import asyncio
import os
import sys
from pathlib import Path

# Add the integration directory to Python path so we can import our modules
sys.path.insert(0, str(Path(__file__).parent))

from dds_client import DDSClient

def pytest_addoption(parser):
    parser.addoption(
        "--url", 
        action="store", 
        default="http://localhost:8080",
        help="DDS Provider service URL"
    )


# @pytest.fixture(scope="session")
# def event_loop():
#     """Create event loop for async tests"""
#     loop = asyncio.new_event_loop()
#     yield loop
#     loop.close()

@pytest.fixture
async def client(request):
    """DDS Provider API client fixture"""
    # Try to get URL from command line, then environment, then default
    url = request.config.getoption("--url")
    if not url:
        url = os.environ.get("SERVICE_URL", "http://localhost:8080")
    
    print(f"Testing against: {url}")
    
    client = DDSClient(url)
    yield client
    await client.close()

@pytest.fixture
def valid_sensor_id():
    """Valid sensor UUID for testing"""
    return "550e8400-e29b-41d4-a716-446655440000"


@pytest.fixture  
def valid_jammer_id():
    """Valid jammer UUID for testing"""
    return "660f8400-e29b-41d4-a716-446655440001"


@pytest.fixture
def valid_device_id():
    """Valid device UUID for testing"""
    return "770e8400-e29b-41d4-a716-446655440002"
