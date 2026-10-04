#include "payment_processor/processor.hpp"
#include "payment_processor/parser.hpp"

#include <limits>

namespace payments {

PaymentProcessor::PaymentProcessor(std::size_t expected_transactions,
                                   std::size_t expected_accounts) {
    seen_ids_.reserve(expected_transactions);
    balances_.reserve(expected_accounts);
}

void PaymentProcessor::reserve_accounts(std::size_t expected_accounts) {
    balances_.reserve(expected_accounts);
}

void PaymentProcessor::set_balance(std::uint32_t account_id, std::int64_t cents) {
    balances_.insert_or_assign(account_id, cents);
}

bool PaymentProcessor::process_line(std::string_view line, ProcessStats& stats) {
    ++stats.parsed;
    Transaction tx{};
    if (!FastParser::parse_line(line, tx)) {
        ++stats.rejected_invalid;
        return false;
    }
    return process(tx, stats);
}

bool PaymentProcessor::process(const Transaction& tx, ProcessStats& stats) {
    if (!seen_ids_.try_emplace(tx.id, 1).second) {
        ++stats.rejected_duplicate;
        return false;
    }

    auto [it, inserted] = balances_.try_emplace(tx.account_id, 0);
    (void)inserted;
    auto& balance = it->second;

    if (tx.type == TransactionType::debit) {
        if (tx.cents > balance) {
            ++stats.rejected_insufficient_funds;
            return false;
        }
        balance -= tx.cents;
        stats.net_cents -= tx.cents;
    } else {
        if (tx.cents > std::numeric_limits<std::int64_t>::max() - balance) {
            ++stats.rejected_invalid;
            return false;
        }
        balance += tx.cents;
        stats.net_cents += tx.cents;
    }

    ++stats.accepted;
    return true;
}

std::int64_t PaymentProcessor::balance(std::uint32_t account_id) const {
    const auto it = balances_.find(account_id);
    return it == balances_.end() ? 0 : it->second;
}

std::size_t PaymentProcessor::unique_transaction_count() const noexcept {
    return seen_ids_.size();
}

} // namespace payments
