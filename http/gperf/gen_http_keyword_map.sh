#!/bin/bash

SCRIPT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

"${SCRIPT_PATH}/gperf.exe" \
    --language=C++ \
    --struct-type \
    --ignore-case \
    -K key \
    --includes \
    --class-name=http_keyword_map \
    --enum \
    "${SCRIPT_PATH}/http_keyword_hash.gperf" > \
    "${SCRIPT_PATH}/../include/httparser/http_keyword_map.h"