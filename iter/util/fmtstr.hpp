#ifndef ITER_FMT_STR_HPP
#define ITER_FMT_STR_HPP

#include <string>
#include <cstdio>
#include <memory>

namespace iter {

template<class ...Types>
inline std::string FmtStr(const std::string& format, Types ...args) {
    size_t size = snprintf(NULL, 0, format.c_str(), args...) + 1; // Extra space for '\0'.
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside.
}

} // namespace iter

#endif // ITER_FMT_STR_HPP

