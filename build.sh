#!/bin/bash

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=${TYPE} ..
make -j$(nproc)