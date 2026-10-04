#include "payment_processor/generator.hpp"
#include "payment_processor/parser.hpp"
#include "payment_processor/processor.hpp"

#include <benchmark/benchmark.h>

#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace {

struct BaselineTransaction {
    std::uint64_t id{};
    std::uint32_t account_id{};
    char type{};
    std::int64_t cents{};
};

bool baseline_parse(const std::string& line, BaselineTransaction& tx) {
    std::istringstream in(line);
    char sep1 = 0, sep2 = 0, sep3 = 0;
    return (in >> tx.id >> sep1 >> tx.account_id >> sep2 >> tx.type >> sep3 >> tx.cents)
        && sep1 == '|' && sep2 == '|' && sep3 == '|' && tx.cents > 0
        && (tx.type == 'D' || tx.type == 'C');
}

void BM_Baseline(benchmark::State& state) {
    const std::size_t count = static_cast<std::size_t>(state.range(0));
    const auto data = payments::generate_transactions(count, 100'000, 42, 0.01);

    for (auto _ : state) {
        std::unordered_set<std::uint64_t> seen;
        std::unordered_map<std::uint32_t, std::int64_t> balances;
        seen.reserve(count);
        balances.reserve(100'000);
        for (std::uint32_t id = 1; id <= 100'000; ++id) balances.emplace(id, 1'000'000);

        for (const auto& line : data.lines) {
            BaselineTransaction tx{};
            if (!baseline_parse(line, tx)) continue;
            if (!seen.emplace(tx.id).second) continue;
            auto& balance = balances[tx.account_id];
            if (tx.type == 'D') {
                if (tx.cents <= balance) balance -= tx.cents;
            } else {
                balance += tx.cents;
            }
            benchmark::DoNotOptimize(balance);
        }
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(count) * state.iterations());
}

void BM_CustomProcessor(benchmark::State& state) {
    const std::size_t count = static_cast<std::size_t>(state.range(0));
    const auto data = payments::generate_transactions(count, 100'000, 42, 0.01);

    for (auto _ : state) {
        payments::PaymentProcessor processor(count, 100'000);
        payments::ProcessStats stats{};
        for (std::uint32_t id = 1; id <= 100'000; ++id) processor.set_balance(id, 1'000'000);

        for (const auto& line : data.lines) {
            benchmark::DoNotOptimize(processor.process_line(line, stats));
        }
        benchmark::DoNotOptimize(stats.accepted);
    }
    state.SetItemsProcessed(static_cast<std::int64_t>(count) * state.iterations());
}

} // namespace

BENCHMARK(BM_Baseline)->Arg(50'000)->Arg(500'000)->Arg(1'000'000);
BENCHMARK(BM_CustomProcessor)->Arg(50'000)->Arg(500'000)->Arg(1'000'000);
