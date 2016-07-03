#ifndef ITER_FILE_LOADER_REGISTER_HPP
#define ITER_FILE_LOADER_REGISTER_HPP

#include <iter/datamanager/file_loader_manager.hpp>
#include <iter/log.hpp>
#include <functional>
#include <future>
#include <string>

namespace iter {

class FileLoaderRegister {
public:
    ~FileLoaderRegister();

    bool Deregister();

    bool Register(const std::string& filename,
        const std::function <bool()>& loader);

    std::future <bool> PushTask(const std::string& filename,
        const std::function <bool()>& loader);
};

FileLoaderRegister::~FileLoaderRegister() {
    Deregister();
}

bool FileLoaderRegister::Deregister() {
    return FileLoaderManager::GetInstance()->DeleteFileLoader((void*)this);
}

bool FileLoaderRegister::Register(const std::string& filename,
        const std::function <bool()>& loader) {
    Deregister();
    return FileLoaderManager::GetInstance()->InsertFileLoader(
            (void*)this, filename, loader);
}

std::future <bool> FileLoaderRegister::PushTask(const std::string& filename,
        const std::function <bool()>& loader) {
    return FileLoaderManager::GetInstance()->PushTask(filename, loader);
}

} // namespace iter

#endif // ITER_FILE_LOADER_REGISTER_HPP
