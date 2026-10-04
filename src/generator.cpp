#include "payment_processor/generator.hpp"

#include <random>

namespace payments {

GeneratedData generate_transactions(std::size_t count,
                                    std::uint32_t account_count,
                                    std::uint64_t seed,
                                    double duplicate_rate) {
    GeneratedData result;
    result.lines.reserve(count);

    if (count == 0 || account_count == 0) return result;

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<std::uint32_t> account_dist(1, account_count);
    std::uniform_int_distribution<std::int64_t> cents_dist(1, 10'000);
    std::uniform_real_distribution<double> probability(0.0, 1.0);

    std::vector<std::string> unique;
    unique.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const auto id = static_cast<std::uint64_t>(i + 1);
        const auto account = account_dist(rng);
        const bool debit = (i % 2U) == 0U;

        unique.push_back(
            std::to_string(id) + "|" +
            std::to_string(account) + "|" +
            (debit ? "D|" : "C|") +
            std::to_string(cents_dist(rng)) + "\n");
    }

    for (std::size_t i = 0; i < count; ++i) {
        if (i > 0 && probability(rng) < duplicate_rate) {
            result.lines.push_back(unique[i - 1]);
        } else {
            result.lines.push_back(unique[i]);
        }
    }

    result.unique_transaction_count = count;
    return result;
}

} // namespace payments
