#!/bin/bash

# Setup strict mode, if any command fails, the script will exit immediately
set -e

g++ ./backend/server.cpp -o ./build/server -std=c++17 -Isrc/lib
echo "Compiled."


pkill server 2>/dev/null || true


./build/server &
echo "Server started."
