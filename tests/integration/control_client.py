from httpx import AsyncClient

from models import http_api
from models import misc


class ControlAPIClient:
    def __init__(self, api_url):
        self.api_url = api_url

    async def _request(self, method, url, model, data, query):
        async with AsyncClient(timeout=20) as client:
            # print(url, data, query)
            if query is not None and isinstance(query, http_api.BaseModel):
                query = query.model_dump()
                print("serialized query:", query)
            data_json=None
            data_plain=None
            if data is not None and isinstance(data, http_api.BaseModel):
                data_plain = data.model_dump_json()
                print("serialized data:", data_plain)
            else:
                data_json=data
            result = await client.request(method, self.api_url + url, json=data_json, content=data_plain, params=query)
            print("Response:", result.text)

            if result.status_code == 400:
                raise misc.BadRequest
            if result.status_code == 422:
                raise misc.UnprocessableEntity

            result.raise_for_status()
            if model is not None:
                result_json = result.json()
                model.model_validate(result_json)
                return result_json
            return result.content

    async def _post_bin(self, url, content, headers, query):
        async with AsyncClient(timeout=10) as client:
            # print(url, data)
            if query is not None and isinstance(query, http_api.BaseModel):
                query = query.model_dump(mode="json")
            print(query)
            result = await client.post(self.api_url + url, content=content, headers=headers, params=query)

            if result.status_code == 400:
                raise misc.BadRequest

            result.raise_for_status()
            return result.status_code

    async def _get(self, url, model, query=None):
            return await self._request("GET", url, model, None, query=query)

    async def _post(self, url, model, data, query=None):
            return await self._request("POST", url, model, data, query=query)


    async def set_firmware(self, session_id, content_range, content):
        if isinstance(session_id, str):
            print("making valid request", len(content))
            headers = {
                "X-Session-ID": session_id,
                "X-Content-Range": content_range,
                "Content-Type": "application/octet-stream",
                "Content-Disposition": "attachment",
            }
        else:
            print("making invalid request")
            headers = session_id
        return await self._post_bin("/upload/firmware", content, headers, None)

    async def services_list(self):
        return await self._get("/services", http_api.ServicesGetResponse)

    async def services_set_state(self, service, state):
        # print(type(services))
        if isinstance(service, str):
            print("making valid request")
            data = http_api.ServicesSetStatePostRequest(service=service, state=state)
        else:
            print("making invalid request")
            data = service
        return await self._post("/services/set_state", None, data)

    async def list_logs(self, start, end):
        if isinstance(start, str):
            print("making valid request")
            params = http_api.LogsListGetParametersQuery(start=start, end=end)
        else:
            print("making invalid request")
            params = start
        return await self._get("/logs_list", http_api.LogsList, params)

    async def get_logs_archive(self, files):
        if isinstance(files, str):
            print("making valid request")
            params = http_api.LogsGetParametersQuery(logs_list=files)
        else:
            print("making invalid request")
            params = files
        return await self._get("/logs", None, params)

    async def set_config(self, config_type, config_data):
        return await self._post("/config/set_config", None, http_api.ConfigFileData(config_data), query={"type":config_type})

    async def get_config(self, config_type):
        config_data = await self._get("/config/get_config", http_api.ConfigFileData, query={"type":config_type})
        return config_data

    async def get_ntp_servers(self):
       response = await self._get("/time/get_ntp_servers", http_api.NtpServers)
       return response["servers"]

    async def set_ntp_servers(self, servers):
        if not isinstance(servers, str):
            print("making valid request")
            data = http_api.NtpServers(servers=servers)
        else:
            print("making invalid request")
            data = servers
        return await self._post("/time/set_ntp_servers", None, data)

    async def get_ntp_sync(self):
        response = await self._get("/time/get_ntp_sync", http_api.NtpSyncState)
        return response["enabled"]

    async def set_ntp_sync(self, enabled):
        request = http_api.NtpSyncState(enabled=enabled)
        return await self._post("/time/set_ntp_sync", None, request)

    async def set_time(self, timestamp):
        data = {"timestamp": timestamp}
        return await self._post("/time/set_time", None, data)

    async def get_time(self):
        response = await self._get("/time/get_time", http_api.SystemTime)
        return response["timestamp"]

    async def get_network_settings(self):
        return await self._get("/network/get_settings", http_api.NetworkSettings)

    async def set_network_settings(self, settings):
        return await self._post("/network/set_settings", None, settings)
