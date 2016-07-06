#ifndef ITER_LOGGER_HPP
#define ITER_LOGGER_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <mutex>

namespace iter {

class Logger{
public:
    Logger();
    ~Logger();
    bool Init(const std::string& filename);
    void Print(const std::string& log);

private:
    void Fix();
    bool GetIno(ino_t& ino);
    bool Open();
    void Close();
    void Write(const std::string& log);

    int fd_;
    ino_t ino_;
    std::string filename_;
    std::mutex mtx_;
};

} // namespace iter

#include <iter/log/detail/logger_impl.hpp>

#endif // ITER_LOGGER_HPP
