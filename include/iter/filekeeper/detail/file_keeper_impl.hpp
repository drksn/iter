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
FileKeeper <LoadFunc, Buffer>::FileKeeper(
        const std::string& filename,
        LoadFunc load_func,
        std::shared_ptr <FileMonitor> file_monitor_ptr) {
    filename_ = filename;
    load_func_ptr_ = std::unique_ptr <LoadFunc> (new LoadFunc(load_func));

    // If NULL, using glob file monitor.
    if (!file_monitor_ptr) {
        // Check the velidation of g_file_monitor.
        using namespace file_keeper;
        if (!g_file_monitor_ptr || !(*g_file_monitor_ptr)) {
            std::lock_guard <std::mutex> lck(g_mtx); // Glob critical region.
            if (!g_file_monitor_ptr || !(*g_file_monitor_ptr)) {
                // Init g_file_monitor.
                g_file_monitor_ptr = std::make_shared <FileMonitor> ();
            }
        }
        file_monitor_ptr_ = g_file_monitor_ptr;
    }
    else {
        file_monitor_ptr_ = file_monitor_ptr;
    }

    CheckFile(); // NOTICE

    bool load_ret = Load();
    if (load_ret) {
        ITER_INFO_KV(MSG("Initial-loading success."), KV("filename", filename_));
    }
    else {
        ITER_WARN_KV(MSG("Initial-loading failed."), KV("filename", filename_));
    }

    // Gen Node.
    using namespace std::placeholders;
    auto callback = std::bind(&FileKeeper <LoadFunc, Buffer>::Callback, this, _1);
    node_ = {filename_, ITER_FILE_KEEPER_EVENT_MASK, callback};

    // Register to file monitor.
    owner_id_ = file_monitor_ptr_->Register(node_);
    if (owner_id_ == -1) {
        ITER_WARN_KV(MSG("FileKeeper register failed."), KV("filename", filename_));
    }
}

template <class LoadFunc, class Buffer>
FileKeeper <LoadFunc, Buffer>::~FileKeeper() {
    file_monitor_ptr_->Remove(owner_id_);
}

template <class LoadFunc, class Buffer>
auto FileKeeper <LoadFunc, Buffer>::Get() -> decltype(buffer_mgr_.Get()) {
    return buffer_mgr_.Get();
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
    if (!buffer_mgr_.Released()) return false;

    Buffer buffer_tmp;
    bool load_ret = (*load_func_ptr_)(filename_, buffer_tmp);
    if (!load_ret) return false;

    bool update_ret = buffer_mgr_.Update(std::move(buffer_tmp));
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
        file_monitor_ptr_->Remove(owner_id_);
        owner_id_ = file_monitor_ptr_->Register(node_);
        if (owner_id_ == -1) {
            ITER_WARN_KV(MSG("FileKeeper register failed."), KV("filename", filename_));
        }
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
