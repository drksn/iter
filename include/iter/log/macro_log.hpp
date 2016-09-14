#ifndef ITER_MACRO_LOG_HPP
#define ITER_MACRO_LOG_HPP

#include <iter/log/macro_basic.hpp>
#include <iter/log/detail/log_util.hpp>
#include <iter/util/fmtstr.hpp>

#ifndef ITER_LOG_LAYOUT
#define ITER_LOG_LAYOUT(level, fmt, args...) \
    (ITER_LOG_HEAD(level) + " " + iter::FmtStr(fmt, ##args) + "\n")
#endif // ITER_LOG_LAYOUT

#ifndef ITER_LOG_LAYOUT_WRITE
#define ITER_LOG_LAYOUT_WRITE(level, args...) \
    ITER_LOG_WRITE(level, ITER_LOG_LAYOUT(level, ##args))
#endif // ITER_LOG_LAYOUT_WRITE

// NOTICE, ITER_DEBUG only used in debug mode.
#ifndef DEBUG
#undef ITER_DEBUG
#define ITER_DEBUG(fmt, args...)
#endif // DEBUG

#ifndef ITER_DEBUG
#define ITER_DEBUG(fmt, args...) ITER_LOG_LAYOUT_WRITE(0, fmt, ##args)
#endif // ITER_DEBUG

#ifndef ITER_INFO
#define ITER_INFO(fmt, args...) ITER_LOG_LAYOUT_WRITE(1, fmt, ##args)
#endif // ITER_INFO

#ifndef ITER_WARN
#define ITER_WARN(fmt, args...) ITER_LOG_LAYOUT_WRITE(2, fmt, ##args)
#endif // ITER_WARN

#ifndef ITER_ERROR
#define ITER_ERROR(fmt, args...) ITER_LOG_LAYOUT_WRITE(3, fmt, ##args)
#endif // ITER_ERROR

#ifndef ITER_FATAL
#define ITER_FATAL(fmt, args...) ITER_LOG_LAYOUT_WRITE(4, fmt, ##args)
#endif // ITER_FATAL

#endif // ITER_MACRO_LOG_HPP
