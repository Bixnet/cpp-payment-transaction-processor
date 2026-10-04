# C++ Payment Transaction Processor

High-throughput payment transaction ingestion, validation, deduplication, and settlement in modern C++20.

> **Status:** In progress

## Core pipeline

Wire format:
`transaction_id|account_id|type|amount_cents\n`

The processor:
1. Parses and validates each transaction with `std::from_chars`.
2. Deduplicates transaction IDs using a pre-sized hash table.
3. Checks balances before debits settle.
4. Applies credits/debits using 64-bit integer cents.

## Performance design

- Custom parser instead of line-by-line stream extraction.
- `absl::flat_hash_map` when Abseil is installed, with a standard-library fallback for portability.
- Hash tables are pre-sized from expected transaction/account counts.
- Hot-path settlement uses stack-local transaction state and integer cents.
- Single-pass validation, deduplication, and settlement.

## Build

Ubuntu/Debian:

```bash
sudo apt-get install -y cmake ninja-build libabsl-dev
cmake -S . -B build -G Ninja -DPAYMENT_BUILD_BENCHMARKS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Run:

```bash
./build/payment_cli 1000000 100000
```

## Benchmark

Install Google Benchmark, then:

```bash
cmake -S . -B build -G Ninja -DPAYMENT_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/payment_benchmarks --benchmark_min_time=0.5
```

The suite compares a stream-based baseline with the custom parser + flat-hash-map implementation.

## perf

```bash
perf stat -d ./build/payment_cli 5000000 500000
perf record -g ./build/payment_cli 5000000 500000
perf report
```

## Portfolio benchmark claim

The intended statement is:

> Reached **[N] million transactions/s**, a **[X]×** speedup over the baseline, measured with Google Benchmark and perf.

Those values should only be filled in after an apples-to-apples benchmark run on the same CPU, compiler, optimization flags, input distribution, and account count.

| Dataset | Baseline | Optimized | Speedup |
| --- | ---: | ---: | ---: |
| 50k | TBD | TBD | TBD |
| 500k | TBD | TBD | TBD |
| 1M | TBD | TBD | TBD |

## Tech stack

C++20 · absl::flat_hash_map · Google Benchmark · perf · CMake · GitHub Actions CI
