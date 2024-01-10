#!/usr/bin/python3

import asyncio

from aiohttp import ClientSession, ClientConnectorError


async def make_request(session, **kwargs):
    try:
        resp = await session.request(method="GET", url="http://localhost:9000", **kwargs)
    except ClientConnectorError:
        return 0
    return 1


async def test_concurrent_requests(number_requests):
    async with ClientSession() as session:
        tasks = [make_request(session) for i in range(number_requests)]
        test_resuts = await asyncio.gather(*tasks)
        print(sum(test_resuts))


if __name__ == "__main__":
    asyncio.run(test_concurrent_requests(10000))
