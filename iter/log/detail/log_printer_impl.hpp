#ifndef ITER_LOG_PRINTER_IMPL_HPP
#define ITER_LOG_PRINTER_IMPL_HPP

#include <sys/time.h>
#include <sys/stat.h>

#include <cstdio>
#include <string>
#include <thread>
#include <mutex>

namespace iter {

LogPrinter::LogPrinter() {
    ptr_ = NULL;
}

LogPrinter::~LogPrinter() {
    fclose(ptr_);
}

uint64_t LogPrinter::GetFileNode() {
    struct stat statbuf;
    if (stat(filename_.c_str(), &statbuf) == 0) return statbuf.st_ino;
    return 0;
}

bool LogPrinter::OpenFile() {
    std::lock_guard <std::mutex> lck(mtx_);
    if (ptr_ != NULL) fclose(ptr_);

    ptr_ = fopen(filename_.c_str(), "a+");
    if (ptr_ == NULL) {
        fprintf(stderr, "Open failed.||filename=%s\n",
            filename_.c_str());
        return false;
    }

    file_node_ = GetFileNode();
    return true;
}

bool LogPrinter::Init(const std::string& filename) {
    filename_ = filename;
    return OpenFile();
}

void LogPrinter::Print(const std::string& log_str) {
    if (ptr_ == NULL) {
        fprintf(stderr, "%s\n", log_str.c_str());
    }
    else if (file_node_ == GetFileNode()) {
        fprintf(ptr_, "%s\n", log_str.c_str());
        fflush(ptr_);
    }
    else if (OpenFile()) {
        fprintf(ptr_, "%s\n", log_str.c_str());
        fflush(ptr_);
        return;
    }
    else {
        fprintf(stderr, "%s\n", log_str.c_str());
    }
}

} // namespace iter

#endif // ITER_LOG_PRINTER_IMPL_HPP
