#ifndef ITER_MACRO_BASIC_HPP
#define ITER_MACRO_BASIC_HPP

#include <iter/log/detail/logger.hpp>
#include <string>

namespace iter {

#define ITER_LOG_LEVEL_NUM 5

static Logger g_logger[ITER_LOG_LEVEL_NUM];

static const int INFO = 2;
static const int WARN = 4;
static const int ERROR = 8;
static const int FATAL = 16;

static void SetLogDestination(int lv_mask, const std::string& dest) {
    for (int i = 0; i < ITER_LOG_LEVEL_NUM; i ++) {
        if ((lv_mask >> i)&1) {
            g_logger[i].Init(dest);
        }
    }
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
