#!/bin/bash
cmake -S . -B build -GNinja && \
cmake --build build && \
chmod +x build/rls && \
strip --strip-all build/rls && \
upx --best build/rls
