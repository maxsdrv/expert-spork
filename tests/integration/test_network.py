import pytest

async def test_get_and_set_settings(client):
    initial_settings = await client.get_network_settings()

    await client.set_network_settings(initial_settings)
    assert await client.get_network_settings() == initial_settings
