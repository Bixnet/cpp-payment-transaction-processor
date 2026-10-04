#include "payment_processor/parser.hpp"

#include <charconv>
#include <system_error>

namespace payments {

namespace {
bool parse_number(std::string_view s, auto& out) {
    if (s.empty()) return false;
    const auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), out);
    return ec == std::errc{} && ptr == s.data() + s.size();
}
}

bool FastParser::parse_u64(std::string_view s, std::uint64_t& out) {
    return parse_number(s, out);
}

bool FastParser::parse_u32(std::string_view s, std::uint32_t& out) {
    return parse_number(s, out);
}

bool FastParser::parse_i64(std::string_view s, std::int64_t& out) {
    return parse_number(s, out);
}

bool FastParser::parse_line(std::string_view line, Transaction& out) {
    while (!line.empty() && (line.back() == '
' || line.back() == '')) line.remove_suffix(1);

    std::string_view fields[4];
    std::size_t count = 0;
    std::size_t start = 0;

    while (start <= line.size() && count < 4) {
        const auto pos = line.find('|', start);
        const auto end = pos == std::string_view::npos ? line.size() : pos;
        fields[count++] = line.substr(start, end - start);
        if (pos == std::string_view::npos) break;
        start = pos + 1;
    }

    if (count != 4 || line.find('|', start) != std::string_view::npos) return false;

    Transaction tx{};
    if (!parse_u64(fields[0], tx.id) ||
        !parse_u32(fields[1], tx.account_id) ||
        !parse_i64(fields[3], tx.cents)) {
        return false;
    }

    if (fields[2] == "D") tx.type = TransactionType::debit;
    else if (fields[2] == "C") tx.type = TransactionType::credit;
    else return false;

    if (tx.cents <= 0) return false;

    out = tx;
    return true;
}

} // namespace payments
