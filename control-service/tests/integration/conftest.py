import pytest
import os


def pytest_addoption(parser):
    parser.addoption(
        "--url", action="store", required=True, help="DDS Control API URL",
    )
    parser.addoption(
        "--data_root", action="store", default="../dds-local-env/.devdata", help="data root path",
    )

def pytest_configure(config):
    config.addinivalue_line("markers", "dev_only: only for service deployed by local dev environment or tamerlan-dev VM")


@pytest.fixture(scope="session")
def client(request):
    url = request.config.getoption("--url")
    from control_client import ControlAPIClient
    return ControlAPIClient(url)

@pytest.fixture(scope="session")
def service_data_file(request):
    data_root = request.config.getoption("--data_root")
    def file_path(*args):
        return os.path.join(data_root, *args)
    return file_path
