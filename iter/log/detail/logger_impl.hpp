#ifndef ITER_LOGGER_IMPL_HPP
#define ITER_LOGGER_IMPL_HPP

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <cstdio>
#include <string>
#include <thread>

namespace iter {

Logger::Logger() {
    fd_ = -1;
}

Logger::~Logger() {
    CloseFile();
}

bool Logger::Init(const std::string& filename) {
    mtx_.lock();
    filename_ = filename;
    mtx_.unlock();
    return OpenFile();
}

void Logger::Print(const std::string& log_str) {
    // If file is moved or removed, reopen.
    if (fd_ != -1 && inode_ != GetFileNode()) {
        CloseFile();
        OpenFile();
    }
    WriteFile(log_str);
}

uint64_t Logger::GetFileNode() {
    struct stat statbuf;
    if (stat(filename_.c_str(), &statbuf) == 0) return statbuf.st_ino;
    return 0;
}

bool Logger::OpenFile() {
    std::lock_guard <std::mutex> lck(mtx_);
    fd_ = open(filename_.c_str(),
            O_CREAT | O_APPEND | O_WRONLY | O_NONBLOCK,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd_ == -1) {
        fprintf(stderr, "Open failed, errno=%d, filename=%s\n",
            errno, filename_.c_str());
        return false;
    }
    inode_ = GetFileNode();
    return true;
}

void Logger::CloseFile() {
    std::lock_guard <std::mutex> lck(mtx_);
    if (fd_ == -1) return;
    if (close(fd_) != 0) {
        fprintf(stderr, "Close failed, errno=%d, fd=%d\n",
            fd_, errno);
    }
    fd_ = -1;
}

void Logger::WriteFile(const std::string& log_str) {
    // If fd is invalid, write to stderr.
    if (fd_ == -1) {
        fprintf(stderr, "%s\n", log_str.c_str());
        return;
    }
    std::string tmp = log_str + "\n";
    if (write(fd_, tmp.c_str(), tmp.size()) == -1) {
        fprintf(stderr, "Write failed, errno=%d, filename=%s, log=%s\n",
            errno, filename_.c_str(), log_str.c_str());
    }
}

} // namespace iter

#endif // ITER_LOGGER_IMPL_HPP
