#!/bin/bash

curl "127.0.0.1:9001" --max-time 1 --verbose \
    -X POST -H "Transfer-Encoding: gzip, chunked" \
    -d '{move: e4} dsfjdksjfklsadjfdjdjfkdjskdjfklsajfksdjflkdfkdsjfkljdfhsghgfhgfhgjhgdjfhgfhgvsdjkhgvjkfdhgrheuihewjkfgvnfkngveriohgoerjnoefnvgergvnerjiogergjiogjioertfvj'