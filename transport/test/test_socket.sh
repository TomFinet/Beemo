#!/bin/bash

curl "127.0.0.1:9003" --max-time 1000 --verbose \
    -X GET \
    -H "Content-Length: 0"
