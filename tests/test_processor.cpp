#include "payment_processor/parser.hpp"
#include "payment_processor/processor.hpp"

#include <cassert>
#include <iostream>

int main() {
    payments::Transaction tx{};
    assert(payments::FastParser::parse_line("42|7|D|1250\n", tx));
    assert(tx.id == 42);
    assert(tx.account_id == 7);
    assert(tx.type == payments::TransactionType::debit);
    assert(tx.cents == 1250);
    assert(!payments::FastParser::parse_line("bad|7|D|1250\n", tx));
    assert(!payments::FastParser::parse_line("42|7|X|1250\n", tx));

    payments::PaymentProcessor processor(16, 4);
    processor.set_balance(7, 5'000);
    payments::ProcessStats stats{};

    assert(processor.process_line("1|7|D|1200\n", stats));
    assert(processor.balance(7) == 3'800);
    assert(!processor.process_line("1|7|D|1200\n", stats));
    assert(stats.rejected_duplicate == 1);
    assert(!processor.process_line("2|7|D|99999\n", stats));
    assert(stats.rejected_insufficient_funds == 1);
    assert(processor.balance(7) == 3'800);
    assert(processor.process_line("3|7|C|500\n", stats));
    assert(processor.balance(7) == 4'300);

    std::cout << "all tests passed\n";
}
