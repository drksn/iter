#ifndef ITER_LOG_BASE_IMPL_HPP
#define ITER_LOG_BASE_IMPL_HPP

#include <sys/time.h>
#include <sys/stat.h>

#include <cstdio>
#include <cassert>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>

namespace iter {

static const char LOG_LEVEL[][6] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

LogBase* LogBase::GetInstance() {
    static LogBase log_base;
    return &log_base;
}

void LogBase::Init(
        const char* log_path, const char* log_prefix = "iter") {
    std::string log_name = std::string(log_path) + "/" + log_prefix + ".log";
    log_printer.Init(log_name);
}

void LogBase::Print(const std::string& log_str) {
    log_printer.Print(log_str);
}

std::string LogBase::GetTimestamp() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv,&tz);
    struct tm vtm;
    localtime_r(&tv.tv_sec, &vtm);
    char buf[32];
    snprintf(buf, 32, "%04d-%02d-%02dT%02d:%02d:%02d.%03d%+05d",
    (int)vtm.tm_year+1900, (int)vtm.tm_mon + 1, (int)vtm.tm_mday,
    (int)vtm.tm_hour, (int)vtm.tm_min, (int)vtm.tm_sec,
    (int)tv.tv_usec/1000, (int)(tz.tz_minuteswest/-60)*100);
    return buf;
}

std::string LogBase::LogHead(
        const int& lv, const char* filename,
        const int& line, const char* function) {
    std::stringstream ss;
    ss << "[" << LOG_LEVEL[lv] << "]";
    ss << "[" << GetTimestamp() << "]";
    ss << "[" << std::this_thread::get_id() << "]";
    ss << "[" << filename << ":" << line << "]";
    ss << "[" << function << "]";
    return ss.str();
}

} // namespace iter

#endif // ITER_LOG_BASE_IMPL_HPP
