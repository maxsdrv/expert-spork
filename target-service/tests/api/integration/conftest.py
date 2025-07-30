import pytest


def pytest_addoption(parser):
    parser.addoption("--url", action="store", required=True,
                     help="DDS Provider HTTP API URL",
                     )

def pytest_configure(config):
    config.addinivalue_line("markers", "jammers: tests for jammers")

@pytest.fixture(scope="session")
def client(request):
    url = request.config.getoption("--url")
    from provider_client import ProviderAPIClient
    return ProviderAPIClient(url)
