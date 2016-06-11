#ifndef ITER_FILE_LOADER_MANAGER_IMPL_HPP
#define ITER_FILE_LOADER_MANAGER_IMPL_HPP

#include <iter/log.hpp>
#include <sys/inotify.h>
#include <functional>
#include <cassert>

namespace iter {

FileLoaderManager::FileLoaderManager() {
    inotify_fd_ = inotify_init();
    shutdown_ = false;
    thread_ptr_ =
        std::shared_ptr <std::thread> (new std::thread(
            std::bind(&iter::FileLoaderManager::Callback, this)));
    assert(thread_ptr_);
    ITER_INFO_KV(MSG("FileLoaderManager thread start."));
}

FileLoaderManager::~FileLoaderManager() {
    shutdown_ = true;
    if (thread_ptr_) {
        thread_ptr_->join();
        ITER_INFO_KV(MSG("FileLoaderManager thread stop."));
    }
}

FileLoaderManager* FileLoaderManager::GetInstance() {
    static FileLoaderManager file_loader_manager;
    return &file_loader_manager;
}

void FileLoaderManager::InsertFileLoader(
        Loader* loader_ptr, const std::string& filename) {
    bool ret = loader_ptr->Load();
    if (!ret) {
        ITER_WARN_KV(MSG("Load failed."), KV(filename));
    }
    else {
        ITER_INFO_KV(MSG("Load success."), KV(filename));
    }

    Node node;
    node.filename = filename;
    node.watcher_fd = inotify_add_watch(
        inotify_fd_, filename.c_str(), IN_MODIFY);
    if (node.watcher_fd == -1) {
        ITER_ERROR_KV(MSG("Add watcher failed."), KV(errno), KV(filename));
        return;
    }
    std::lock_guard <std::mutex> lck(mtx_);
    file_map_[loader_ptr] = node;
    ITER_INFO_KV(MSG("Insert success."), KV(filename));
}

void FileLoaderManager::DeleteFileLoader(
        Loader* loader_ptr, const std::string& filename) {
    if (file_map_.find(loader_ptr) == file_map_.end()) {
         ITER_WARN_KV(MSG("No such file loader."), KV(filename));
         return;
    }
    Node tmp = file_map_[loader_ptr];
    std::lock_guard <std::mutex> lck(mtx_);
    file_map_.erase(loader_ptr);
    int rm_ret = inotify_rm_watch(inotify_fd_, tmp.watcher_fd);
    if (rm_ret == -1) {
        ITER_ERROR_KV(MSG("Rm watcher failed."), KV(errno), KV(filename));
    }
    ITER_INFO_KV(MSG("Delete success."), KV(filename));
}

void FileLoaderManager::Callback() {

}

} // namespace iter

#endif // ITER_FILE_LOADER_MANAGER_IMPL_HPP
