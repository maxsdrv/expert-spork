import asyncio
import uuid


def sleep(delay):
    return asyncio.sleep(delay)

def gen_uuid():
    return uuid.uuid4().hex
