#!/bin/bash

script_path="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
build_path="${script_path}/../../build"

# build test executables
cmake --build "${build_path}"

# run server
"${build_path}/bin/client_server_tests.exe" &
# run client tests
python3 "${script_path}/test_client.py" &
