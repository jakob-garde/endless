#!/bin/bash
cd build/
rm -rf endless/
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
cd endless
./endless
