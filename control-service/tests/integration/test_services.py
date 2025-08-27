from datetime import datetime

from models.http_api import ServiceState

services_exclude = [
    "dds-control-api",
    "dds-control-update",
    "dds-control-proxy",
]

async def test_services_restart(client):
    services_info = await client.services_list()
    start_times = { svc["service"]: datetime.fromisoformat(svc["info"]["since"]) for svc in services_info}

    for svc in services_info:
        if svc["service"] not in services_exclude:
            await client.services_set_state(svc["service"], ServiceState.restarting)

    services_info = await client.services_list()
    for svc in services_info:
        if svc["service"] not in services_exclude:
            assert svc["info"]["state"] == ServiceState.running.name
            assert datetime.fromisoformat(svc["info"]["since"]) > start_times[svc["service"]]

async def test_services_stop_start(client):
    services_info = await client.services_list()
    start_times = { svc["service"]: datetime.fromisoformat(svc["info"]["since"]) for svc in services_info}

    for svc in services_info:
        if svc["service"] not in services_exclude:
            await client.services_set_state(svc["service"], ServiceState.exited)

    services_info = await client.services_list()
    for svc in services_info:
        if svc["service"] not in services_exclude:
            assert svc["info"]["state"] == ServiceState.exited.name

    for svc in services_info:
        if svc["service"] not in services_exclude:
            await client.services_set_state(svc["service"], ServiceState.running)

    services_info = await client.services_list()
    for svc in services_info:
        if svc["service"] not in services_exclude:
            assert svc["info"]["state"] == ServiceState.running.name
            assert datetime.fromisoformat(svc["info"]["since"]) > start_times[svc["service"]]
