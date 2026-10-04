#pragma once

#include <unordered_map>

#ifdef PAYMENT_USE_ABSL
#include <absl/container/flat_hash_map.h>
#endif

namespace payments {

#ifdef PAYMENT_USE_ABSL
template <typename K, typename V>
using FastHashMap = absl::flat_hash_map<K, V>;
#else
template <typename K, typename V>
using FastHashMap = std::unordered_map<K, V>;
#endif

} // namespace payments
