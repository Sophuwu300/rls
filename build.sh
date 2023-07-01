#!/bin/bash
cmake -S . -B build -GNinja
cmake --build build
chmod +x build
strip build/rls
build/rls $1

