#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace payments {

struct GeneratedData {
    std::vector<std::string> lines;
    std::size_t unique_transaction_count{};
};

GeneratedData generate_transactions(std::size_t count,
                                    std::uint32_t account_count,
                                    std::uint64_t seed = 42,
                                    double duplicate_rate = 0.01);

} // namespace payments
