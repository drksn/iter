#ifndef ITER_MACRO_BASIC_HPP
#define ITER_MACRO_BASIC_HPP

#include <iter/log/detail/logger.hpp>
#include <string>

namespace iter {

static Logger g_logger[5];

static const int INFO = 1;
static const int WARN = 2;
static const int ERROR = 3;
static const int FATAL = 4;

static bool SetLogDestination(int level, const std::string& dest) {
    if (level < INFO || level > FATAL) return false;
    g_logger[level].Init(dest);
    return true;
}

#ifdef ITER_LOG_DISABLE
#define ITER_LOG_WRITE(log_lv, log_str)
#endif // ITER_LOG_DISABLE

#ifndef ITER_LOG_WRITE
#define ITER_LOG_WRITE(log_lv, log_str) \
    iter::g_logger[log_lv].Print(log_str)
#endif // ITER_LOG_WRITE

} // namespace iter

#endif // ITER_MACRO_BASE_HPP
