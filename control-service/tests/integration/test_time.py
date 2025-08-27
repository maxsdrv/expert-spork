import pytest

from datetime import datetime, UTC

from helpers.misc import sleep

from models import misc

async def test_ntp_servers(client):
    servers = ["0.debian.pool.ntp.org", "1.arch.pool.ntp.org"]

    await client.set_ntp_servers(servers[1:])
    assert await client.get_ntp_servers() == servers[1:]

    await client.set_ntp_servers(servers)

    assert await client.get_ntp_servers() == servers

async def test_ntp_sync(client):
    await  client.set_ntp_sync(False)
    assert await client.get_ntp_sync() == False

    await  client.set_ntp_sync(True)
    assert await client.get_ntp_sync() == True

async def test_set_and_get_time(client):
    await  client.set_ntp_sync(False)

    set_timestamp = datetime.now(UTC).isoformat()
    await client.set_time(set_timestamp)

    await sleep(1)

    get_timestamp = await client.get_time()

    set_time = datetime.fromisoformat(set_timestamp)
    get_time = datetime.fromisoformat(get_timestamp)

    assert abs((get_time - set_time).total_seconds()) <= 2

    await  client.set_ntp_sync(True)

    with pytest.raises(misc.UnprocessableEntity):
        await client.set_time(set_timestamp)
