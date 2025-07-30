import json
from httpx import AsyncClient
from models import http_api
from models import misc


class ProviderAPIClient:
    def __init__(self, api_url):
        self.api_url = api_url

    async def _call(self, method, url, model, data, query):
        async with AsyncClient(timeout=10) as client:
            # print(url, data)
            if query is not None and isinstance(query, misc.BaseModel):
                query = query.model_dump()
            if data is not None and isinstance(data, misc.BaseModel):
                data = data.model_dump()
            result = await client.request(method, self.api_url + url, content=json.dumps(data, default=str), params=query, headers={"Content-Type": "application/json"})

            if result.status_code == 400:
                raise misc.BadRequest

            # treat HTTP 500 as a valid response with error data ...
            if result.status_code == 500:
                raise misc.APIError(http_api.Error.model_validate(result.json()))

            result.raise_for_status()
            print(result.text)
            result_json = result.json()
            #print(json)
            model.model_validate(result_json)
            return result_json

    async def _get(self, url, model, query=None):
            return await self._call("GET", url, model, None, query=query)

    async def _post(self, url, model, data, query=None):
            return await self._call("POST", url, model, data, query=query)


    async def api_version(self):
        result = await self._get("/common/get_api_version", http_api.CommonGetApiVersionGetResponse)
        return result["api_version"]


    async def sensors(self):
        result = await self._get("/sensor/get_sensors", http_api.SensorList)
        return result["sensors"]

    async def sensor_info(self, id):
        if isinstance(id, str):
            id = http_api.SensorGetSensorInfoGetParametersQuery(sensor_id=id)
        result = await self._get("/sensor/get_sensor_info", http_api.SensorGetSensorInfoGetResponse, id)
        return result["sensor_info"]

    async def set_sensor_disabled(self, id, value):
        if isinstance(id, str):
            id = http_api.SensorSetSensorDisabledPostRequest(sensor_id=id, disabled=value)
            value = None
        return await self._post("/sensor/set_sensor_disabled", misc.EmptyResponse, id, value)


    async def jammers(self):
        result = await self._get("/jammer/get_jammers", http_api.JammerList)
        return result["jammers"]

    async def jammer_info(self, id):
        if isinstance(id, str):
            id = http_api.JammerGetJammerInfoGetParametersQuery(jammer_id=id)
        result = await self._get("/jammer/get_jammer_info", http_api.JammerGetJammerInfoGetResponse, id)
        return result["jammer_info"]

    async def set_jammer_disabled(self, id, value):
        if isinstance(id, str):
            id = http_api.JammerSetJammerDisabledPostRequest(jammer_id=id, disabled=value)
            value = None
        return await self._post("/jammer/set_jammer_disabled", misc.EmptyResponse, id, value)

    async def set_jammer_bands(self, id, value):
        if isinstance(id, str):
            id = http_api.JammerSetJammerBandsPostRequest(jammer_id=id, bands=value)
            value = None
        return await self._post("/jammer/set_jammer_bands", misc.EmptyResponse, id, value)


    async def camera_orientation(self, id):
        if isinstance(id, str):
            id = http_api.CameraGetOrientationGetParametersQuery(id=id)
        return await self._get("/camera/get_orientation", http_api.CameraOrientation, id)

    async def set_geo_direction(self, id, value):
        if isinstance(id, str):
            body = http_api.CameraSetGeoDirectionPostRequest(id=id, direction=value)
            value = None
        return await self._post("/camera/set_geo_direction", misc.EmptyResponse, id, value)
