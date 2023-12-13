#!/bin/bash

curl "127.0.0.1:9002" --max-time 1 --verbose \
    -X GET \
    -H "Content-Length: 0"