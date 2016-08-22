#ifndef ITER_MACRO_KV_HPP
#define ITER_MACRO_KV_HPP

#include <iter/log/macro_log.hpp>
#include <iter/util/kvstr.hpp>

#ifndef MSG
#define MSG(arg) std::make_pair("msg", arg)
#endif // MSG

#ifndef ITER_DEBUG_KV
#define ITER_DEBUG_KV(args...) ITER_DEBUG(KVSTR(args))
#endif // ITER_DEBUG_KV

#ifndef ITER_INFO_KV
#define ITER_INFO_KV(args...) ITER_INFO(KVSTR(args))
#endif // ITER_INFO_KV

#ifndef ITER_WARN_KV
#define ITER_WARN_KV(args...) ITER_WARN(KVSTR(args))
#endif // ITER_WARN_KV

#ifndef ITER_ERROR_KV
#define ITER_ERROR_KV(args...) ITER_ERROR(KVSTR(args))
#endif // ITER_ERROR_KV

#ifndef ITER_FATAL_KV
#define ITER_FATAL_KV(args...) ITER_FATAL(KVSTR(args))
#endif // ITER_FATAL_KV

#endif // ITER_MACRO_KV_HPP
