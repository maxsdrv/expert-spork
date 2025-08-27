import pytest

import datetime
import io
import lzma
import os
import tarfile

@pytest.mark.dev_only
async def test_list_today_rotated(client, service_data_file):
    def log_file_path(name):
        return service_data_file("logs", name)

    today_param = datetime.datetime.now(datetime.UTC).strftime('%Y-%m-%d')
    today = datetime.datetime.now(datetime.UTC).strftime('%Y%m%d')
    last_rotated_log = "services.log-" + today + ".xz"

    content = b"Lots of content here"
    f = open(log_file_path(last_rotated_log), "wb")
    f.write(lzma.compress(content))
    f.close()

    f = open(log_file_path("services.log"), "wb")
    f.write(content)
    f.close()

    # print(content)
    # print(log_file_path("services.log"))
    files = await client.list_logs(today_param, today_param)
    assert files == [last_rotated_log, "services.log"]

@pytest.mark.dev_only
async def test_list_only_current(client, service_data_file):
    def log_file_path(name):
        return service_data_file("logs", name)

    today_param = datetime.datetime.now(datetime.UTC).strftime('%Y-%m-%d')
    today = datetime.datetime.now(datetime.UTC).strftime('%Y%m%d')

    try:
        os.remove(log_file_path("services.log-" + today + ".xz"))
    except FileNotFoundError:
        pass

    content = b"Lots of content here"
    f = open(log_file_path("services.log"), "wb")
    f.write(content)
    f.close()


    # print(content)
    # print(log_file_path("services.log"))
    files = await client.list_logs(today_param, today_param)
    assert files == ["services.log"]

@pytest.mark.dev_only
async def test_get_current(client, service_data_file):
    def log_file_path(name):
        return service_data_file("logs", name)

    today = datetime.datetime.now(datetime.UTC).strftime('%Y%m%d')
    content = bytes("Lots of content here" + today, "utf-8")

    f = open(log_file_path("services.log"), "wb")
    f.write(content)
    f.close()

    archive = await client.get_logs_archive("services.log")
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        file = tar.extractfile("services.log.xz")
        assert lzma.decompress(file.read()) == content

@pytest.mark.dev_only
async def test_get_rotated(client, service_data_file):
    def log_file_path(name):
        return service_data_file("logs", name)

    today = datetime.datetime.now(datetime.UTC).strftime('%Y%m%d')
    content = bytes("Lots of content here" + today, "utf-8")
    last_rotated_log = "services.log-" + today + ".xz"

    f = open(log_file_path(last_rotated_log), "wb")
    f.write(lzma.compress(content))
    f.close()

    archive = await client.get_logs_archive(last_rotated_log)
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        file = tar.extractfile(last_rotated_log)
        assert lzma.decompress(file.read()) == content
