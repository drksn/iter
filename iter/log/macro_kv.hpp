#ifndef ITER_MACRO_KV_HPP
#define ITER_MACRO_KV_HPP

#include <iter/log/macro_basic.hpp>
#include <iter/util/kvstr.hpp>

#ifndef MSG
#define MSG(arg) std::make_pair("msg", arg)
#endif // MSG

#ifdef ITER_LOG_DISABLE
#define ITER_LOG_KV(log_level, log_arg...)
#endif // ITER_LOG_DISABLE

#ifndef ITER_LOG_KV
#define ITER_LOG_KV(log_level, log_arg...) \
    ITER_LOG_WRITE(ITER_LOG_HEAD(log_level) + " " + iter::KvStr(log_arg) + "\n")
#endif // ITER_LOG_KV

#ifndef ITER_DEBUG_KV
#define ITER_DEBUG_KV(log_arg...) ITER_LOG_KV(0, ##log_arg)
#endif // ITER_DEBUG_KV

#ifndef ITER_INFO_KV
#define ITER_INFO_KV(log_arg...) ITER_LOG_KV(1, ##log_arg)
#endif // ITER_INFO_KV

#ifndef ITER_WARN_KV
#define ITER_WARN_KV(log_arg...) ITER_LOG_KV(2, ##log_arg)
#endif // ITER_WARN_KV

#ifndef ITER_ERROR_KV
#define ITER_ERROR_KV(log_arg...) ITER_LOG_KV(3, ##log_arg)
#endif // ITER_ERROR_KV

#ifndef ITER_FATAL_KV
#define ITER_FATAL_KV(log_arg...) ITER_LOG_KV(4, ##log_arg)
#endif // ITER_FATAL_KV

#endif // ITER_MACRO_KV_HPP
