#ifndef ITER_FILE_KEEPER_IMPL_HPP
#define ITER_FILE_KEEPER_IMPL_HPP

#include <string>
#include <memory>
#include <utility>

namespace iter {

template <class LoadFunc, class Buffer>
template <class LoadFuncInit, class ...Types>
FileKeeper <LoadFunc, Buffer>::FileKeeper(const std::string& filename,
        LoadFuncInit&& load_func_init, Types&& ...args) {
    filename_ = filename;
    load_func_ptr_ = std::unique_ptr <LoadFunc> (
        new LoadFunc(std::forward <LoadFuncInit> (load_func_init)));
    buffer_mgr_ptr_ = std::unique_ptr <BufferMgr> (
        new BufferMgr(std::forward <Types> (args)...));
}

template <class LoadFunc, class Buffer>
template <class ...Types>
FileKeeper <LoadFunc, Buffer>::FileKeeper(
        const std::string& filename, Types&& ...args) {
    filename_ = filename;
    load_func_ptr_ = std::unique_ptr <LoadFunc> (new LoadFunc(args...));
    buffer_mgr_ptr_ = std::unique_ptr <BufferMgr> (new BufferMgr(args...));
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

} // namespace iter

#endif // ITER_FILE_KEEPER_IMPL_HPP
