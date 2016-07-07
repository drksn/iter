#ifndef ITER_FILE_LOADER_REGISTER_HPP
#define ITER_FILE_LOADER_REGISTER_HPP

#include <iter/filekeeper/detail/file_loader_manager.hpp>
#include <iter/log.hpp>
#include <functional>
#include <future>
#include <string>

namespace iter {

class FileLoaderRegister {
public:
    ~FileLoaderRegister();

    bool Deregister();

    bool Register(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename);

    std::future <bool> PushTask(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename);
};

FileLoaderRegister::~FileLoaderRegister() {
    Deregister();
}

bool FileLoaderRegister::Deregister() {
    return FileLoaderManager::GetInstance()->DeleteFileLoader((void*)this);
}

bool FileLoaderRegister::Register(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename) {
    Deregister();
    return FileLoaderManager::GetInstance()->InsertFileLoader(
            (void*)this, loader, filename);
}

std::future <bool> FileLoaderRegister::PushTask(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename) {
    return FileLoaderManager::GetInstance()->PushTask(loader, filename);
}

} // namespace iter

#endif // ITER_FILE_LOADER_REGISTER_HPP
