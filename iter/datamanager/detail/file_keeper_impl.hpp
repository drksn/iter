#ifndef ITER_FILE_KEEPER_IMPL_HPP
#define ITER_FILE_KEEPER_IMPL_HPP

#include <iter/datamanager/file_loader_manager.hpp>
#include <iter/log.hpp>
#include <string>
#include <memory>
#include <utility>

namespace iter {

template <class LoadFunc, class Buffer>
FileKeeper <LoadFunc, Buffer>::~FileKeeper() {
    FileLoaderManager::GetInstance()->DeleteFileLoader(this, filename_);
}

template <class LoadFunc, class Buffer>
template <class LoadFuncInit, class ...Types>
FileKeeper <LoadFunc, Buffer>::FileKeeper(const std::string& filename,
        LoadFuncInit&& load_func_init, Types&& ...args) {
    filename_ = filename;
    load_func_ptr_ = std::unique_ptr <LoadFunc> (
        new LoadFunc(std::forward <LoadFuncInit> (load_func_init)));
    buffer_mgr_ptr_ = std::unique_ptr <BufferMgr> (
        new BufferMgr(std::forward <Types> (args)...));

    FileLoaderManager::GetInstance()->InsertFileLoader(this, filename);
    InitialLoad();
}

template <class LoadFunc, class Buffer>
template <class ...Types>
FileKeeper <LoadFunc, Buffer>::FileKeeper(
        const std::string& filename, Types&& ...args) {
    filename_ = filename;
    load_func_ptr_ = std::unique_ptr <LoadFunc> (new LoadFunc(args...));
    buffer_mgr_ptr_ = std::unique_ptr <BufferMgr> (new BufferMgr(args...));

    FileLoaderManager::GetInstance()->InsertFileLoader(this, filename);
    InitialLoad();
}

template <class LoadFunc, class Buffer>
bool FileKeeper <LoadFunc, Buffer>::GetBuffer(std::shared_ptr <Buffer>* ptr) {
    return buffer_mgr_ptr_->GetBuffer(ptr);
}

template <class LoadFunc, class Buffer>
bool FileKeeper <LoadFunc, Buffer>::Load() {
    std::shared_ptr <Buffer> ptr;
    bool get_ret = buffer_mgr_ptr_->GetNextBuffer(&ptr); // NOTICE
    if (!get_ret) return false;
    std::lock_guard <std::mutex> lck(mtx_);
    bool load_ret = (*load_func_ptr_)(filename_, *ptr);
    if (!load_ret) return false;
    buffer_mgr_ptr_->SwapBuffer();
    return true;
}

template <class LoadFunc, class Buffer>
void FileKeeper <LoadFunc, Buffer>::InitialLoad() {
    if (Load()) {
        ITER_INFO_KV(MSG("Initial load success."),
            KV("filename", filename_));
    }
    else {
        ITER_WARN_KV(MSG("Initial load failed."),
            KV("filename", filename_));
    }
}

} // namespace iter

#endif // ITER_FILE_KEEPER_IMPL_HPP
