#ifndef ITER_MACRO_BASIC_HPP
#define ITER_MACRO_BASIC_HPP

#include <iter/log/detail/logger.hpp>
#include <iter/log/detail/log_basic.hpp>

namespace iter {
    static Logger g_logger;
} // namespace iter

#ifndef ITER_LOG_INIT
#define ITER_LOG_INIT(log_path) \
    iter::g_logger.Init(log_path)
#endif // ITER_LOG_INIT

#ifdef ITER_LOG_DISABLE
#define ITER_LOG_WRITE(log_str)
#endif // ITER_LOG_DISABLE

#ifndef ITER_LOG_WRITE
#define ITER_LOG_WRITE(log_str) \
    iter::g_logger.Print(log_str)
#endif // ITER_LOG_WRITE

#ifndef ITER_LOG_HEAD
#define ITER_LOG_HEAD(lv) \
    iter::LogHead(lv, __FILE__, __LINE__, __FUNCTION__)
#endif // ITER_LOG_HEAD

#endif // ITER_MACRO_BASE_HPP
