#ifndef ITER_FILE_LOADER_REGISTER_HPP
#define ITER_FILE_LOADER_REGISTER_HPP

#include <iter/filekeeper/detail/file_loader_manager.hpp>
#include <iter/log.hpp>
#include <uuid/uuid.h>
#include <functional>
#include <future>
#include <string>

namespace iter {

class FileLoaderRegister {
public:
    FileLoaderRegister();
    ~FileLoaderRegister();

    bool Deregister();

    bool Register(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename);

    std::future <bool> PushTask(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename);

private:
    std::string uuid_;
};

FileLoaderRegister::FileLoaderRegister() {
     uuid_t uuid;
     char str[36];
     uuid_generate(uuid);
     uuid_unparse(uuid, str);
     uuid_ = str;
}

FileLoaderRegister::~FileLoaderRegister() {
    Deregister();
}

bool FileLoaderRegister::Deregister() {
    return FileLoaderManager::GetInstance()->DeleteFileLoader(uuid_);
}

bool FileLoaderRegister::Register(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename) {
    Deregister();
    return FileLoaderManager::GetInstance()->InsertFileLoader(
            uuid_, loader, filename);
}

std::future <bool> FileLoaderRegister::PushTask(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename) {
    return FileLoaderManager::GetInstance()->PushTask(loader, filename);
}

} // namespace iter

#endif // ITER_FILE_LOADER_REGISTER_HPP
