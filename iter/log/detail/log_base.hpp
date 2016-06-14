#ifndef ITER_LOG_BASE_HPP
#define ITER_LOG_BASE_HPP

#include <iter/log/detail/log_printer.hpp>
#include <string>

namespace iter {

class LogBase {
public:
    static LogBase* GetInstance();
    void Init(const char* log_path, const char* log_prefix);
    void Print(const std::string& log_str);
    static std::string GetTimestamp();
    static std::string LogHead(
        const int& lv, const char* filename,
        const int& line, const char* function);

private:
    LogBase() = default;
    LogPrinter log_printer;
};

} // namespace iter

#include <iter/log/detail/log_base_impl.hpp>

#endif // ITER_LOG_BASE_HPP
