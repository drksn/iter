#ifndef ITER_FILE_KEEPER_IMPL_HPP
#define ITER_FILE_KEEPER_IMPL_HPP

#include <iter/log.hpp>
#include <sys/stat.h>
#include <cstdlib>
#include <string>
#include <memory>
#include <utility>

namespace iter {

template <class Buffer>
FileKeeper <Buffer>::FileKeeper(const Loader& loader, const std::string& filename,
        const std::shared_ptr <FileMonitor>& file_monitor_ptr) : loader_(loader), filename_(filename) {
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
    auto callback = std::bind(&FileKeeper <Buffer>::Callback, this, _1);
    node_ = {filename_, ITER_FILE_KEEPER_EVENT_MASK, callback};

    // Register to file monitor.
    owner_id_ = file_monitor_ptr_->Register(node_);
    if (owner_id_ == -1) {
        ITER_WARN_KV(MSG("FileKeeper register failed."), KV("filename", filename_));
    }
}

template <class Buffer>
FileKeeper <Buffer>::~FileKeeper() {
    file_monitor_ptr_->Remove(owner_id_);
}

template <class Buffer>
auto FileKeeper <Buffer>::Get() -> decltype(double_buffer_.Get()) {
    return double_buffer_.Get();
}

template <class Buffer>
FileKeeper <Buffer>::operator bool() {
    return owner_id_ != -1;
}

template <class Buffer>
bool FileKeeper <Buffer>::CheckFile() {
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

template <class Buffer>
bool FileKeeper <Buffer>::Load() {
    if (!double_buffer_.Released()) return false;

    Buffer* reserved = double_buffer_.GetReservedBuffer();
    bool load_ret = loader_(filename_, reserved);
    if (!load_ret) return false;

    bool update_ret = double_buffer_.Update();
    if (!update_ret) {
        ITER_WARN_KV(MSG("Load failed, reserved buffer not released."));
        return false;
    }
    return true;
}

template <class Buffer>
void FileKeeper <Buffer>::Callback(const FileEvent& file_event) {
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
