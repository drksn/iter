#ifndef ITER_LOG_UTIL_HPP
#define ITER_LOG_UTIL_HPP

#include <iter/util/fmtstr.hpp>

#include <sys/time.h>
#include <sys/stat.h>

#include <ctime>
#include <sstream>
#include <string>
#include <thread>
#include <functional>

namespace iter {

static const char LOG_LEVEL[][6] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

static std::string LogTimestamp() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    struct tm vtm;
    localtime_r(&tv.tv_sec, &vtm);

    return FmtStr("%04d-%02d-%02dT%02d:%02d:%02d.%03d%+05d",
            (int)vtm.tm_year+1900, (int)vtm.tm_mon + 1, (int)vtm.tm_mday,
            (int)vtm.tm_hour, (int)vtm.tm_min, (int)vtm.tm_sec,
            (int)tv.tv_usec/1000, (int)(tz.tz_minuteswest/-60)*100);
}

#ifndef ITER_LOG_THREAD_ID_MOD
#define ITER_LOG_THREAD_ID_MOD 997
#endif // ITER_LOG_THREAD_ID_MOD

static std::string LogHead(int level, const char* filename, int line, const char* function) {
    std::stringstream ss;

    #ifndef ITER_LOG_DISABLE_LEVEL
        ss << "[" << LOG_LEVEL[level] << "]";
    #endif

    #ifndef ITER_LOG_DISABLE_TIMESTAMP
        ss << "[" << LogTimestamp() << "]";
    #endif

    #ifndef ITER_LOG_DISABLE_THREAD_ID
        static std::hash <std::thread::id> hash_tid;
        ss << "[" << hash_tid(std::this_thread::get_id()) % ITER_LOG_THREAD_ID_MOD << "]";
    #endif

    #ifdef ITER_LOG_ENABLE_POSITION
        ss << "[" << filename << ":" << line << "]";
    #endif

    #ifdef ITER_LOG_ENABLE_FUNCTION
        ss << "[" << function << "]";
    #endif

    return ss.str();
}

#ifndef ITER_LOG_HEAD
#define ITER_LOG_HEAD(log_lv) \
    iter::LogHead(log_lv, __FILE__, __LINE__, __FUNCTION__)
#endif // ITER_LOG_HEAD

} // namespace iter

#endif // ITER_LOG_UTIL_HPP
