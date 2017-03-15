#ifndef ITER_KV_STR_HPP
#define ITER_KV_STR_HPP

#include <sstream>
#include <string>
#include <utility>
#include <type_traits>

namespace iter {

template <class K, class V>
inline auto Kv(K&& k, V&& v)->decltype(std::make_pair(k, v)) {
    return std::make_pair(std::forward<K>(k), std::forward<V>(v));
}

template <class N, class K, class V>
inline auto Kv(N&& n, K&& k, V&& v)->decltype(std::make_pair(k, v)) {
    return std::make_pair(std::forward<K>(k), std::forward<V>(v));
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
        int precision = ITER_KV_PRECISION) :
        sep_outer_(sep_outer),
        sep_inner_(sep_inner),
        precision_(precision) {}

    std::string sep_inner() const { return sep_inner_; }
    std::string sep_outer() const { return sep_outer_; }
    int precision() const { return precision_; }

    template <class ...Types>
    std::string operator () (Types&& ...args) const {
        return write(std::forward<Types>(args)...);
    }

private:
    std::string sep_outer_, sep_inner_;
    int precision_;

private:
    std::string write(const std::string& str) const {
        return str;
    }

    std::string&& write(std::string&& str) const {
        return std::move(str);
    }

    template <class T,
        class = typename std::enable_if <
            std::tuple_size <T>::value == 2>::type>
    std::string write(const T& t) const {
        std::stringstream ss;
        ss.precision(precision_);
        ss << std::get<0>(t) << sep_inner_ << std::get<1>(t);
        return ss.str();
    }

    template <class T,
        class = typename std::enable_if <
            std::tuple_size <
                typename T::value_type>::value == 2>::type,
        class = typename T::iterator>
    std::string write(const T& t) const {
        if (std::begin(t) == std::end(t)) return "";
        auto i = std::begin(t);
        std::stringstream ss;
        ss.precision(precision_);
        ss << std::get<0>(*i) << sep_inner_ << std::get<1>(*i);
        for (i++; i != std::end(t); i++) {
            ss << sep_outer_ << std::get<0>(*i)
                << sep_inner_ << std::get<1>(*i);
        }
        return ss.str();
    }

    template <class First, class Second, class ...Types>
    std::string write(
        First&& first, Second&& second, Types&& ...args) const {
        std::string first_str = write(first);
        std::string args_str = write(second, args...);
        if (args_str.size() == 0) return first_str;
        return first_str + sep_outer_ + args_str;
    }
};

} // namespace iter

#endif // ITER_KV_STR_HPP
