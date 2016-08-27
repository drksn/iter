#ifndef ITER_MACRO_BASIC_HPP
#define ITER_MACRO_BASIC_HPP

#include <iter/log/detail/logger.hpp>
#include <iter/log/detail/log_util.hpp>

namespace iter {

static Logger g_logger;

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
#define ITER_LOG_HEAD(log_lv) \
    iter::LogHead(log_lv, __FILE__, __LINE__, __FUNCTION__)
#endif // ITER_LOG_HEAD

} // namespace iter

#endif // ITER_MACRO_BASE_HPP
