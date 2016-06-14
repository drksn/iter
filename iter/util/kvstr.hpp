#ifndef ITER_KV_STR_HPP
#define ITER_KV_STR_HPP

#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace iter {

template <class K, class V>
inline auto Kv(K&& k, V&& v)->decltype(std::make_pair(k, v)){
    return std::make_pair(std::forward <K> (k),
        std::forward<V> (v));
}

template <class N, class K, class V>
inline auto Kv(N&& n, K&& k, V&& v)->decltype(std::make_pair(k, v)){
    return std::make_pair(std::forward <K> (k),
        std::forward<V> (v));
}

#ifndef KV
#define KV(arg, args...) iter::Kv(#arg, arg, ##args)
#endif // KV

inline std::string KvStr(const std::string& str){
    return str;
}

template <class K, class V>
inline std::string KvStr(const std::map <K, V>& mp){
    if (mp.size() == 0) return "";
    std::stringstream ss;
    ss.precision(15);
    auto pt = mp.begin();
    ss << pt->first << "=" << pt->second;
    for (pt++; pt != mp.end(); pt++)
        ss << "||" << pt->first << "=" << pt->second;
    return ss.str();
}

template <class K, class V>
inline std::string KvStr(const std::vector <std::pair <K, V> >& vec){
    if (vec.size() == 0) return "";
    std::stringstream ss;
    ss.precision(15);
    auto pt = vec.begin();
    ss << pt->first << "=" << pt->second;
    for (pt++; pt != vec.end(); pt++)
        ss << "||" << pt->first << "=" << pt->second;
    return ss.str();
}

template <class K, class V>
inline std::string KvStr(const std::pair <K, V>& p){
    std::stringstream ss;
    ss.precision(15);
    ss << p.first << "=" << p.second;
    return ss.str();
}

template <class First, class Second, class ...Types>
inline std::string KvStr(const First& first, const Second& second,
        const Types& ...args){
    std::string first_str = KvStr(first);
    std::string args_str = KvStr(second, args...);
    if (args_str.size() == 0) return first_str;
    return first_str + "||" + args_str;
}

} // namespace iter

#endif // ITER_KV_STR_HPP
