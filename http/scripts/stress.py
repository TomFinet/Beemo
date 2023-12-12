#!/usr/bin/python3

import asyncio
import aiohttp

from aiohttp import ClientSession, ClientConnectorError


async def make_request(session, **kwargs):
    try:
        resp = await session.request(method="GET", url="http://localhost:9001", **kwargs)
    except ClientConnectorError:
        return 1
    return 0


async def test_concurrent_requests(number_requests):
    async with ClientSession() as session:
        tasks = [make_request(session) for i in range(number_requests)]
        test_resuts = await asyncio.gather(*tasks)
        return sum(test_resuts) == 0


if __name__ == "__main__":
    asyncio.run(test_concurrent_requests(1))
