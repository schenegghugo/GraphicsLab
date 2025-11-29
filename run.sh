#!/bin/bash

# ensure build directory exists
mkdir -p build
cd build

# Compile (only recompiles changed files)
cmake .. > /dev/null
make -j$(nproc)

# If compile succeeds, run the app
if [ $? -eq 0 ]; then
    ./GraphicsLab
else
    echo "Build Failed"
fi
