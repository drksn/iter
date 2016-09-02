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
FileKeeper <Buffer>::FileKeeper(
        const std::string& filename, const Loader& loader,
        const std::shared_ptr <FileMonitor>& file_monitor_ptr) :
        filename_(filename), loader_(loader), file_monitor_ptr_(file_monitor_ptr) {
    // Gen Node.
    using namespace std::placeholders;
    auto callback = std::bind(&FileKeeper <Buffer>::Callback, this, _1);
    node_ = {filename_, ITER_FILE_KEEPER_EVENT_MASK, callback};

    FixFile();

    // Register to file monitor.
    owner_id_ = file_monitor_ptr_->Register(node_);
    // Failed, exit constructor.
    if (owner_id_ == -1) return;

    // Create DoubleBuffer.
    buffer_ptr_ = BufferPtr(new DoubleBuffer <Buffer>());
}

template <class Buffer>
FileKeeper <Buffer>::~FileKeeper() {
    if (file_monitor_ptr_) {
        file_monitor_ptr_->Remove(owner_id_);
    }
}

template <class Buffer>
auto FileKeeper <Buffer>::Get() -> decltype(buffer_ptr_->Get()) {
    return buffer_ptr_->Get();
}

template <class Buffer>
FileKeeper <Buffer>::operator bool() {
    return owner_id_ != -1;
}

template <class Buffer>
void FileKeeper <Buffer>::FixFile() {
    struct stat f_stat;
    if (stat(filename_.c_str(), &f_stat) != 0) {
        std::string cmd = "touch " + filename_;
        std::system(cmd.c_str());
        ITER_WARN_KV(
            MSG("File not exist and touching a new file."),
            KV("filename", filename_));
    }
}

template <class Buffer>
bool FileKeeper <Buffer>::Load() {
    if (!buffer_ptr_->Released()) return false;

    Buffer* reserved_buffer_ptr = buffer_ptr_->GetReservedBuffer();
    bool load_ret = loader_(filename_, reserved_buffer_ptr);
    if (!load_ret) return false;

    bool update_ret = buffer_ptr_->Update();
    if (!update_ret) {
        ITER_WARN_KV(MSG("Load failed, reserved buffer not released."));
        return false;
    }
    return true;
}

template <class Buffer>
void FileKeeper <Buffer>::Callback(const FileEvent& file_event) {
    if (file_event.mask & (IN_DELETE_SELF | IN_MOVE_SELF)) {
        FixFile();
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
