async def get_set_get_compare(client, config_name):
    config_text = await client.get_config(config_name)
    await client.set_config(config_name, config_text)
    config_back = await client.get_config(config_name)
    assert config_back.strip() == config_text.strip()

async def test_config_system(client):
    await get_set_get_compare(client, "SYSTEM")

async def test_config_devices(client):
    await get_set_get_compare(client, "DEVICES")

