#ifndef ITER_LOGGER_HPP
#define ITER_LOGGER_HPP

#include <cstdint>
#include <cstdio>
#include <mutex>
#include <string>

namespace iter {

class Logger{
public:
    Logger();
    ~Logger();
    bool Init(const std::string& filename);
    void Print(const std::string& log_str);

private:
    uint64_t GetFileNode();
    bool OpenFile();
    void CloseFile();
    void WriteFile(const std::string& log_str);

    int fd_;
    uint64_t inode_;
    std::string filename_;
    std::mutex mtx_;
};

} // namespace iter

#include <iter/log/detail/logger_impl.hpp>

#endif // ITER_LOGGER_HPP
