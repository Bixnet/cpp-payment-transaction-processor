#!/usr/bin/env bash
set -euo pipefail

COUNT="${1:-5000000}"
ACCOUNTS="${2:-500000}"

cmake -S . -B build -G Ninja -DPAYMENT_BUILD_BENCHMARKS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

perf stat -d ./build/payment_cli "$COUNT" "$ACCOUNTS"
