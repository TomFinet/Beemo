#!/bin/bash

sudo ~/wrk/wrk -t4 -c400 -d5s http://localhost:9001 &
sudo perf record -e cpu-clock ./build/bin/server
pid=$!
sudo kill -INT ${pid}
sudo perf report --stdio --dsos=server
