#pragma once

#include <cstdint>

namespace payments {

enum class TransactionType : std::uint8_t { debit, credit };

struct Transaction {
    std::uint64_t id{};
    std::uint32_t account_id{};
    TransactionType type{TransactionType::debit};
    std::int64_t cents{};
};

struct ProcessStats {
    std::uint64_t parsed{};
    std::uint64_t accepted{};
    std::uint64_t rejected_duplicate{};
    std::uint64_t rejected_invalid{};
    std::uint64_t rejected_insufficient_funds{};
    std::int64_t net_cents{};
};

} // namespace payments
