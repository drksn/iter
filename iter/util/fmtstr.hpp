#ifndef ITER_FMT_STR_HPP
#define ITER_FMT_STR_HPP

#include <string>
#include <cstdio>
#include <memory>

namespace iter {

template<class ...Types>
inline std::string FmtStr(const std::string& format, Types ...args) {
    int n = format.size() << 1;    
    std::unique_ptr<char[]> buf(new char[n]);
    int ret = snprintf(buf.get(), n, format.c_str(), args...);
    if (ret <= 0) return "";
    if (ret >= n) {
        n = ret + 1; // Extra space for '/0'.
        buf.reset(new char[n]);
        snprintf(buf.get(), n, format.c_str(), args...);
    }  
    return std::string(buf.get());
}

} // namespace iter

#endif // ITER_FMT_STR_HPP

