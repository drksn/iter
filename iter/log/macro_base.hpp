#ifndef ITER_MACRO_BASE_HPP
#define ITER_MACRO_BASE_HPP

#include <iter/log/detail/log_base.hpp>

#ifndef ITER_LOG_INIT
#define ITER_LOG_INIT(log_path) \
    iter::LogBase::GetInstance()->Init(log_path);
#endif // ITER_LOG_INIT

#ifndef ITER_LOG_WRITE
#define ITER_LOG_WRITE(log_str) \
    iter::LogBase::GetInstance()->Print(log_str);
#endif // ITER_LOG_WRITE

#ifndef ITER_LOG_HEAD
#define ITER_LOG_HEAD(lv) \
    iter::LogBase::LogHead(lv, __FILE__, __LINE__, __FUNCTION__)
#endif // ITER_LOG_HEAD

#endif // ITER_MACRO_BASE_HPP
