#pragma once

#include "payment_processor/hash_map.hpp"
#include "payment_processor/transaction.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace payments {

class PaymentProcessor {
public:
    explicit PaymentProcessor(std::size_t expected_transactions,
                              std::size_t expected_accounts = 0);

    void reserve_accounts(std::size_t expected_accounts);
    void set_balance(std::uint32_t account_id, std::int64_t cents);

    bool process_line(std::string_view line, ProcessStats& stats);
    bool process(const Transaction& tx, ProcessStats& stats);

    [[nodiscard]] std::int64_t balance(std::uint32_t account_id) const;
    [[nodiscard]] std::size_t unique_transaction_count() const noexcept;

private:
    FastHashMap<std::uint64_t, std::uint8_t> seen_ids_;
    FastHashMap<std::uint32_t, std::int64_t> balances_;
};

} // namespace payments
