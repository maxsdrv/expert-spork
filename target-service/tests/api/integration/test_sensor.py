from helpers.misc import sleep


async def test_set_sensor_disabled(client):
    result = await client.sensors()
    id = result[0]["id"]

    await client.set_sensor_disabled(id, False)
    info = await client.sensor_info(id)
    assert info["disabled"] == False

    poll_interval_sec = 1
    # TODO: force info change of the sensor imitator

    # info should change on the enabled sensor
    await sleep(poll_interval_sec + 1)
    new_info = await client.sensor_info(id)
    assert new_info != info

    await client.set_sensor_disabled(id, True)
    info = await client.sensor_info(id)
    assert info["disabled"] == True

    # info must not change on the disabled sensor
    await sleep(poll_interval_sec + 1)
    new_info = await client.sensor_info(id)
    assert new_info == info

async def test_set_sensor_disabled_status(client):
    result = await client.sensors()
    id = result[0]["id"]

    await client.set_sensor_disabled(id, True)
    info = await client.sensor_info(id)
    assert info["disabled"] == True

    await client.set_sensor_disabled(id, False)
    info = await client.sensor_info(id)
    assert info["disabled"] == False
