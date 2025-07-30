from helpers.misc import sleep
from models.http_api import SensorType

async def test_camera_orientation(client):
    result = await client.sensors()
    id = None
    for sensor in result:
        if sensor["type"] == SensorType.CAMERA.value:
            id = sensor["id"]
    assert id != None

    orientation = await client.camera_orientation(id)
    assert orientation["id"] == id
