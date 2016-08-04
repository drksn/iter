#ifndef ITER_KV_STR_HPP
#define ITER_KV_STR_HPP

#include <sstream>
#include <string>
#include <utility>
#include <type_traits>

#define ITER_KV_PRECISION 15

namespace iter {

template <class K, class V>
inline auto Kv(K&& k, V&& v)->decltype(std::make_pair(k, v)) {
    return std::make_pair(std::forward <K> (k),
        std::forward<V> (v));
}

template <class N, class K, class V>
inline auto Kv(N&& n, K&& k, V&& v)->decltype(std::make_pair(k, v)) {
    return std::make_pair(std::forward <K> (k),
        std::forward<V> (v));
}

#ifndef KV
#define KV(arg, args...) iter::Kv(#arg, arg, ##args)
#endif // KV

inline std::string KvStr(const std::string& str) {
    return str;
}

template <class T,
    class = typename std::enable_if <std::tuple_size <T>::value == 2>::type>
inline std::string KvStr(const T& t) {
    std::stringstream ss;
    ss.precision(ITER_KV_PRECISION);
    ss << std::get <0>(t) << "=" << std::get <1>(t);
    return ss.str();
}


template <class T,
    class = typename std::enable_if <
        std::tuple_size <typename T::value_type>::value == 2>::type,
    class = typename T::iterator>
inline std::string KvStr(const T& t) {
    if (std::begin(t) == std::end(t)) return "";
    auto i = std::begin(t);
    std::stringstream ss;
    ss.precision(ITER_KV_PRECISION);
    ss << std::get <0>(*i) << "=" << std::get <1>(*i);
    for (i++; i != std::end(t); i++) {
        ss << "||" << std::get <0>(*i) << "=" << std::get <1>(*i);
    }
    return ss.str();
}

template <class First, class Second, class ...Types>
inline std::string KvStr(First&& first, Second&& second, Types&& ...args) {
    std::string first_str = KvStr(first);
    std::string args_str = KvStr(second, args...);
    if (args_str.size() == 0) return first_str;
    return first_str + "||" + args_str;
}

} // namespace iter

#endif // ITER_KV_STR_HPP
