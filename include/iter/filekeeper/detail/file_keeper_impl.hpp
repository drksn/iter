#ifndef ITER_FILE_KEEPER_IMPL_HPP
#define ITER_FILE_KEEPER_IMPL_HPP

#include <iter/log.hpp>
#include <sys/stat.h>
#include <cstdlib>
#include <string>
#include <memory>
#include <utility>
#include <functional>

namespace iter {

template <class LoadFunc, class Buffer>
template <class ...Types>
FileKeeper <LoadFunc, Buffer>::FileKeeper(
        const std::string& filename, Types&& ...args) {
    filename_ = filename;
    buffer_mgr_ptr_ = std::unique_ptr <BufferMgr> (new BufferMgr());
    load_func_ptr_ = std::unique_ptr <LoadFunc> (
            new LoadFunc(std::forward <Types> (args)...));

    CheckFile(); // NOTICE

    bool load_ret = Load();
    if (load_ret) {
        ITER_INFO_KV(MSG("Initial-loading success."), KV(filename));
    }
    else {
        ITER_WARN_KV(MSG("Initial-loading failed."), KV(filename));
    }

    using namespace std::placeholders;
    auto callback = std::bind(&FileKeeper <LoadFunc, Buffer>::Callback, this, _1);
    node_ = {filename_, ITER_FILE_KEEPER_EVENT_MASK, callback};
    owner_id_ = g_file_monitor.Register(node_);
    if (owner_id_ == -1) {
        ITER_WARN_KV(MSG("FileKeeper register failed."), KV(filename));
    }
}

template <class LoadFunc, class Buffer>
FileKeeper <LoadFunc, Buffer>::~FileKeeper() {
    g_file_monitor.Remove(owner_id_);
}

template <class LoadFunc, class Buffer>
auto FileKeeper <LoadFunc, Buffer>::Get() -> decltype(buffer_mgr_ptr_->Get()) {
    return buffer_mgr_ptr_->Get();
}

template <class LoadFunc, class Buffer>
FileKeeper <LoadFunc, Buffer>::operator bool() {
    return owner_id_ != -1;
}

template <class LoadFunc, class Buffer>
bool FileKeeper <LoadFunc, Buffer>::CheckFile() {
    struct stat f_stat;
    if (stat(filename_.c_str(), &f_stat) != 0) {
        std::string cmd = "touch " + filename_;
        std::system(cmd.c_str());
        ITER_WARN_KV(
            MSG("File not exist and touching a new file."),
            KV("filename", filename_));
        return false;
    }
    return true;
}

template <class LoadFunc, class Buffer>
bool FileKeeper <LoadFunc, Buffer>::Load() {
    if (!buffer_mgr_ptr_->Released()) return false;

    Buffer buffer_tmp;
    bool load_ret = (*load_func_ptr_)(filename_, buffer_tmp);
    if (!load_ret) return false;

    bool update_ret = buffer_mgr_ptr_->Update(std::move(buffer_tmp));
    if (!update_ret) {
        ITER_WARN_KV(MSG("Load failed, previous buffer not released."));
        return false;
    }
    return true;
}

template <class LoadFunc, class Buffer>
void FileKeeper <LoadFunc, Buffer>::Callback(const FileEvent& file_event) {
    if (file_event.mask & (IN_DELETE_SELF | IN_MOVE_SELF)) {
        CheckFile();
        g_file_monitor.Remove(owner_id_);
        owner_id_ = g_file_monitor.Register(node_);
    }
    bool ret = Load();
    if (ret) {
        ITER_INFO_KV(MSG("Auto-loading success."), KV("filename", filename_));
    }
    else {
        ITER_WARN_KV(MSG("Auto-loading failed."), KV("filename", filename_));
    }
}

} // namespace iter

#endif // ITER_FILE_KEEPER_IMPL_HPP
