#!/usr/bin/python3

import asyncio
import aiohttp

import unittest

from aiohttp import ClientSession, ClientConnectorError

async def make_io_buf_overflow_request(session, test, **kwargs):
    try:
        resp = await session.request(method="GET", url="http://localhost:9007", **kwargs)
        test.assertEqual(resp.status, 200)
        test.assertEqual(await resp.text(), "a" * 2000)
        return 0
    except ClientConnectorError:
        return 1 

async def test_concurrent_requests(number_requests, test):
    async with ClientSession() as session:
        tasks = [make_io_buf_overflow_request(session, test) for i in range(number_requests)]
        test_resuts = await asyncio.gather(*tasks)
        return sum(test_resuts) == 0

class TestWebServer(unittest.TestCase):
    
    def test_tx_buf_overflow(self):
        asyncio.run(test_concurrent_requests(2, self))

if __name__ == '__main__':
    unittest.main()
