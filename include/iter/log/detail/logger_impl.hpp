#ifndef ITER_LOGGER_IMPL_HPP
#define ITER_LOGGER_IMPL_HPP

#include <unistd.h>
#include <fcntl.h>
#include <cstdio>

namespace iter {

inline Logger::Logger() {
    fd_ = -1;
}

inline Logger::~Logger() {
    Close();
}

inline bool Logger::Init(const std::string& filename) {
    std::lock_guard <std::mutex> lck(mtx_);
    filename_ = filename;
    return Open() && GetIno(ino_);
}

inline void Logger::Print(const std::string& log) {
    std::lock_guard <std::mutex> lck(mtx_);
    Fix();
    Write(log);
}

inline void Logger::Fix() {
    if (filename_.size() == 0) return; // Not init.
    ino_t ino_tmp;
    bool ret = GetIno(ino_tmp);
    if (!ret || ino_ != ino_tmp) { // File not exist or be replaced.
        Close();
        Open(); // Reopen.
        GetIno(ino_);
    }
}

inline bool Logger::GetIno(ino_t& ino) {
    struct stat statbuf;
    if (stat(filename_.c_str(), &statbuf) != 0) return false;
    ino = statbuf.st_ino;
    return true;
}

inline bool Logger::Open() {
    fd_ = open(filename_.c_str(),
            O_CREAT | O_APPEND | O_WRONLY | O_NONBLOCK,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (fd_ == -1) {
        fprintf(stderr, "Logger open failed, errno=%d, filename=%s\n",
            errno, filename_.c_str());
        return false;
    }
    return true;
}

inline void Logger::Close() {
    if (fd_ == -1) return;
    if (close(fd_) != 0) {
        fprintf(stderr, "Logger close failed, errno=%d, fd=%d\n",
            fd_, errno);
    }
    fd_ = -1;
}

inline void Logger::Write(const std::string& log) {
    if (fd_ == -1) { // If fd is invalid, write to stderr.
        fprintf(stderr, "%s", log.c_str());
        return;
    }
    int ret = write(fd_, log.c_str(), log.size());
    if (ret == -1) {
        fprintf(stderr, "Logger write failed, errno=%d, filename=%s\n",
            errno, filename_.c_str());
    }
}

} // namespace iter

#endif // ITER_LOGGER_IMPL_HPP
