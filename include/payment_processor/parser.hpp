#pragma once

#include "payment_processor/transaction.hpp"
#include <string_view>

namespace payments {

class FastParser {
public:
    static bool parse_line(std::string_view line, Transaction& out);

private:
    static bool parse_u64(std::string_view s, std::uint64_t& out);
    static bool parse_u32(std::string_view s, std::uint32_t& out);
    static bool parse_i64(std::string_view s, std::int64_t& out);
};

} // namespace payments
