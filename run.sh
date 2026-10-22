#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

if [ ! -f hospital_system ] || [ main.cpp -nt hospital_system ]; then
    echo "Compiling hospital_system..."
    g++ -std=c++17 -O2 main.cpp -o hospital_system
fi

exec ./hospital_system "$@"
