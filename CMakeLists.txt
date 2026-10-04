cmake_minimum_required(VERSION 3.20)
project(payment_transaction_processor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PAYMENT_BUILD_BENCHMARKS "Build Google Benchmark target" ON)
option(PAYMENT_BUILD_TESTS "Build unit tests" ON)

add_library(payment_processor
    src/parser.cpp
    src/processor.cpp
    src/generator.cpp
)
target_include_directories(payment_processor PUBLIC include)
target_compile_features(payment_processor PUBLIC cxx_std_20)

if(MSVC)
  target_compile_options(payment_processor PRIVATE /W4 /permissive-)
else()
  target_compile_options(payment_processor PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

find_package(absl CONFIG QUIET)
if(absl_FOUND)
  target_compile_definitions(payment_processor PUBLIC PAYMENT_USE_ABSL=1)
  target_link_libraries(payment_processor PUBLIC absl::flat_hash_map)
else()
  message(WARNING "Abseil not found; using std::unordered_map fallback.")
endif()

add_executable(payment_cli src/main.cpp)
target_link_libraries(payment_cli PRIVATE payment_processor)

if(PAYMENT_BUILD_TESTS)
  enable_testing()
  add_executable(payment_tests tests/test_processor.cpp)
  target_link_libraries(payment_tests PRIVATE payment_processor)
  add_test(NAME payment_tests COMMAND payment_tests)
endif()

if(PAYMENT_BUILD_BENCHMARKS)
  find_package(benchmark CONFIG QUIET)
  if(benchmark_FOUND)
    add_executable(payment_benchmarks benchmarks/payment_benchmark.cpp)
    target_link_libraries(payment_benchmarks PRIVATE payment_processor benchmark::benchmark benchmark::benchmark_main)
  endif()
endif()
