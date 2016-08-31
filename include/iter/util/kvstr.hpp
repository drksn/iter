#ifndef ITER_KV_STR_HPP
#define ITER_KV_STR_HPP

#include <sstream>
#include <string>
#include <utility>
#include <type_traits>

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

#ifndef ITER_KV_SEP_OUTER
#define ITER_KV_SEP_OUTER "||"
#endif // ITER_KV_SEP_OUTER

#ifndef ITER_KV_SEP_INNER
#define ITER_KV_SEP_INNER "="
#endif // ITER_KV_SEP_INNER

#ifndef ITER_KV_PRECISION
#define ITER_KV_PRECISION 15
#endif // ITER_KV_PRECISION

class KvStr {
public:
    KvStr(
        std::string sep_outer = ITER_KV_SEP_OUTER,
        std::string sep_inner = ITER_KV_SEP_INNER,
        int precision = ITER_KV_PRECISION);

    const std::string& GetInnerSep();
    const std::string& GetOuterSep();
    const int& GetPrecision();

    template <class ...Types> std::string operator () (Types&& ...args);

private:
    std::string sep_outer_, sep_inner_;
    int precision_;
};

static KvStr kvstr; // Default functor.

#ifndef KVSTR
#define KVSTR(args...) iter::kvstr(args)
#endif // KVSTR

inline KvStr::KvStr(std::string sep_outer, std::string sep_inner, int precision) :
        sep_outer_(sep_outer), sep_inner_(sep_inner), precision_(precision) {}

inline const std::string& KvStr::GetInnerSep() {
    return sep_inner_;
}

inline const std::string& KvStr::GetOuterSep() {
    return sep_outer_;
}

inline const int& KvStr::GetPrecision() {
    return precision_;
}

namespace kvutil {

inline std::string KvWrite(KvStr* ptr, const std::string& str) {
    return str;
}

template <class T,
    class = typename std::enable_if <std::tuple_size <T>::value == 2>::type>
inline std::string KvWrite(KvStr* ptr, const T& t) {
    std::stringstream ss;
    ss.precision(ptr->GetPrecision());
    ss << std::get <0>(t) << ptr->GetInnerSep() << std::get <1>(t);
    return ss.str();
}

template <class T,
    class = typename std::enable_if <
        std::tuple_size <typename T::value_type>::value == 2>::type,
    class = typename T::iterator>
inline std::string KvWrite(KvStr* ptr, const T& t) {
    if (std::begin(t) == std::end(t)) return "";
    auto i = std::begin(t);
    std::stringstream ss;
    ss.precision(ptr->GetPrecision());
    ss << std::get <0>(*i) << ptr->GetInnerSep() << std::get <1>(*i);
    for (i++; i != std::end(t); i++) {
        ss << ptr->GetOuterSep() << std::get <0>(*i) << ptr->GetInnerSep() << std::get <1>(*i);
    }
    return ss.str();
}

template <class First, class Second, class ...Types>
inline std::string KvWrite(KvStr* ptr, First&& first, Second&& second, Types&& ...args) {
    std::string first_str = KvWrite(ptr, first);
    std::string args_str = KvWrite(ptr, second, args...);
    if (args_str.size() == 0) return first_str;
    return first_str + ptr->GetOuterSep() + args_str;
}

} // namespace kvutil

template <class ...Types>
std::string KvStr::operator () (Types&& ...args) {
    return kvutil::KvWrite(this, std::forward <Types> (args)...);
}

} // namespace iter

#endif // ITER_KV_STR_HPP
