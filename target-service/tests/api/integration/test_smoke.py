import pytest
from models.misc import BadRequest
from helpers.misc import gen_uuid


async def test_api_version(client):
    result = await client.api_version()
    assert result == "2.1.0"

async def test_get_sensors(client):
    result = await client.sensors()
    assert len(result) > 0

async def test_get_sensor_info(client):
    result = await client.sensors()
    for sensor in result:
        assert await client.sensor_info(sensor["id"]) == sensor

async def test_get_sensor_info_invalid_params(client):
    with pytest.raises(BadRequest):
        await client.sensor_info({})
    with pytest.raises(BadRequest):
        await client.sensor_info({"invalid": "param"})
    # 200 OK: dumb rest::ApiGateway
    # with pytest.raises(BadRequest):
    #     await client.sensor_info({"sensor_id": None})
    with pytest.raises(BadRequest):
        await client.sensor_info({"sensor_id": 1234})
    with pytest.raises(BadRequest):
        await client.sensor_info({"sensor_id": "invalid_uuid"})
    with pytest.raises(BadRequest):
        await client.sensor_info(gen_uuid())

async def test_set_sensor_disabled_invalid_params(client):
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"invalid": "param"}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"sensor_id": None}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"sensor_id": 1234}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"sensor_id": 1234, "disabled": False}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"sensor_id": None, "disabled": False}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"sensor_id": "invalid_uuid", "disabled": False}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled(gen_uuid(), False)

    result = await client.sensors()
    id = result[0]["id"]

    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"sensor_id": id}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"sensor_id": id, "disabled": "str"}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"sensor_id": id, "disabled": None}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"sensor_id": id, "disabled": 1234}, None)

@pytest.mark.jammers
async def test_get_jammers(client):
    result = await client.jammers()
    assert len(result) > 0

@pytest.mark.jammers
async def test_get_jammer_info(client):
    result = await client.jammers()
    for jammer in result:
        assert await client.jammer_info(jammer["id"]) == jammer

@pytest.mark.jammers
async def test_get_jammer_info_invalid_params(client):
    with pytest.raises(BadRequest):
        await client.jammer_info({})
    with pytest.raises(BadRequest):
        await client.jammer_info({"invalid": "param"})
    # 200 OK: dumb rest::ApiGateway
    # with pytest.raises(BadRequest):
    #     await client.jammer_info({"jammer_id": None})
    with pytest.raises(BadRequest):
        await client.jammer_info({"jammer_id": 1234})
    with pytest.raises(BadRequest):
        await client.jammer_info({"jammer_id": "invalid_uuid"})
    with pytest.raises(BadRequest):
        await client.jammer_info(gen_uuid())

@pytest.mark.jammers
async def test_set_jammer_disabled_invalid_params(client):
    with pytest.raises(BadRequest):
        await client.set_jammer_disabled({}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_disabled({"invalid": "param"}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"jammer_id": None}, None)
    with pytest.raises(BadRequest):
        await client.set_sensor_disabled({"jammer_id": 1234}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_disabled({"jammer_id": 1234, "disabled": False}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_disabled({"jammer_id": None, "disabled": False}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_disabled({"jammer_id": "invalid_uuid", "disabled": False}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_disabled(gen_uuid(), False)

    result = await client.jammers()
    id = result[0]["id"]

    with pytest.raises(BadRequest):
        await client.set_jammer_disabled({"jammer_id": id}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_disabled({"jammer_id": id, "disabled": "str"}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_disabled({"jammer_id": id, "disabled": None}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_disabled({"jammer_id": id, "disabled": 1234}, None)

@pytest.mark.jammers
async def test_set_jammer_bands_invalid_params(client):
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"invalid": "param"}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": None, "bands": [{"label": "433MHz", "active": False}]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": 1234, "bands": [{"label": "433MHz", "active": False}]}, None)

    result = await client.jammers()
    id = result[0]["id"]

    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": None}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": {}}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [None]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [{}]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": ["label", "active"]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [{"label": None}]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [{"label": 1234, "active": False}]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [{"label": "433MHz", "active": None}]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "invalid": [{"label": "433MHz", "active": False}]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [{"invalid_lbl": "433MHz", "active": False}]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [{"label": "433MHz", "invalid": False}]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [{"label": "433MHz", "active": False}, None]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [{"label": "433MHz", "active": False}, {}]}, None)
    with pytest.raises(BadRequest):
        await client.set_jammer_bands({"jammer_id": id, "bands": [{"label": "433MHz", "active": False},
                                                                  {"label": "866MHz", "active": "str"}]}, None)
