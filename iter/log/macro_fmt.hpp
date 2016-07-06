#ifndef ITER_MACRO_FMT_HPP
#define ITER_MACRO_FMT_HPP

#include <iter/log/macro_basic.hpp>
#include <iter/util/fmtstr.hpp>

#ifdef ITER_LOG_DISABLE
#define ITER_LOG_FMT(log_level, log_fmt, log_arg...)
#endif // ITER_LOG_DISABLE

#ifndef ITER_LOG_FMT
#define ITER_LOG_FMT(log_level, log_fmt, log_arg...)        \
    ITER_LOG_WRITE(iter::FmtStr("%s " log_fmt "\n",         \
        (ITER_LOG_HEAD(log_level)).c_str(), ##log_arg))
#endif // ITER_LOG_FMT

#ifndef ITER_DEBUG_FMT
#define ITER_DEBUG_FMT(log_fmt, log_arg...) \
    ITER_LOG_FMT(0, log_fmt, ##log_arg)
#endif // ITER_DEBUG_FMT

#ifndef ITER_INFO_FMT
#define ITER_INFO_FMT(log_fmt, log_arg...) \
    ITER_LOG_FMT(1, log_fmt, ##log_arg)
#endif // ITER_INFO_FMT

#ifndef ITER_WARN_FMT
#define ITER_WARN_FMT(log_fmt, log_arg...) \
    ITER_LOG_FMT(2, log_fmt, ##log_arg)
#endif // ITER_WARN_FMT

#ifndef ITER_ERROR_FMT
#define ITER_ERROR_FMT(log_fmt, log_arg...) \
    ITER_LOG_FMT(3, log_fmt, ##log_arg)
#endif // ITER_ERROR_FMT

#ifndef ITER_FATAL_FMT
#define ITER_FATAL_FMT(log_fmt, log_arg...) \
    ITER_LOG_FMT(4, log_fmt, ##log_arg)
#endif // ITER_FATAL_FMT

#endif // ITER_MACRO_FMT_HPP
