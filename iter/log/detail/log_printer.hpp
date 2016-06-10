#ifndef ITER_LOG_PRINTER_HPP
#define ITER_LOG_PRINTER_HPP

#include <cstdint>
#include <cstdio>
#include <mutex>
#include <string>

namespace iter {

class LogPrinter{
public:
    LogPrinter();
    ~LogPrinter();
    bool Init(const std::string& filename);
    void Print(const std::string& log_str);

private:
    uint64_t GetFileNode();
    bool OpenFile();

    FILE* ptr_;
    std::string filename_;
    std::mutex mtx_;
    uint64_t file_node_;
};

} // namespace iter

#include <iter/log/detail/log_printer_impl.hpp>

#endif // ITER_LOG_PRINTER_HPP
