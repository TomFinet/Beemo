#!/bin/bash

curl "127.0.0.1:9001" --max-time 1 --verbose \
    -X POST -H "Transfer-Encoding: chunked" \
    -H "Content-Type: text/html" \
    -d '12\r\nspace marine\r\n10\r\n damage: 5\r\n0\r\n'