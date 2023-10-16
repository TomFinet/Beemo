#!/bin/bash

SCRIPT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

"${SCRIPT_PATH}/gperf.exe" -L C++ -t -K key "${SCRIPT_PATH}/http_keyword_hash.gperf" \
    > "${SCRIPT_PATH}/../include/httparser/http_keyword_map.h"