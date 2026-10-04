#include "payment_processor/generator.hpp"
#include "payment_processor/processor.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const std::size_t transaction_count = argc > 1 ? std::stoull(argv[1]) : 1'000'000;
    const std::uint32_t account_count =
        argc > 2 ? static_cast<std::uint32_t>(std::stoul(argv[2])) : 100'000;

    const auto data = payments::generate_transactions(transaction_count, account_count);
    payments::PaymentProcessor processor(transaction_count, account_count);
    payments::ProcessStats stats{};

    for (std::uint32_t id = 1; id <= account_count; ++id) {
        processor.set_balance(id, 1'000'000);
    }

    const auto start = std::chrono::steady_clock::now();
    for (const auto& line : data.lines) {
        processor.process_line(line, stats);
    }

    const auto elapsed =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();

    std::cout << "processed=" << stats.parsed
              << " accepted=" << stats.accepted
              << " duplicate=" << stats.rejected_duplicate
              << " invalid=" << stats.rejected_invalid
              << " insufficient_funds=" << stats.rejected_insufficient_funds
              << " unique_ids=" << processor.unique_transaction_count()
              << " elapsed_s=" << elapsed
              << " throughput_mtx_s="
              << (static_cast<double>(stats.parsed) / elapsed / 1'000'000.0)
              << '\n';
}
