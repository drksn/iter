#ifndef ITER_FILE_KEEPER_IMPL_HPP
#define ITER_FILE_KEEPER_IMPL_HPP

#include <string>
#include <memory>
#include <utility>
#include <functional>

namespace iter {

template <class LoadFunc, class Buffer>
void FileKeeper <LoadFunc, Buffer>::Init() {
    std::function <bool(const std::string&)> loader(std::bind(
        &iter::FileKeeper <LoadFunc, Buffer>::Load, this, std::placeholders::_1));
    register_.Register(loader, filename_);
    auto ret = register_.PushTask(loader, filename_);
    if (ret.valid()) ret.get(); // Initial load.
}

template <class LoadFunc, class Buffer>
template <class LoadFuncInit>
FileKeeper <LoadFunc, Buffer>::FileKeeper(const std::string& filename,
        LoadFuncInit&& load_func_init) {
    filename_ = filename;
    buffer_mgr_ptr_ = std::unique_ptr <BufferMgr> (new BufferMgr());
    load_func_ptr_ = std::unique_ptr <LoadFunc> (
        new LoadFunc(std::forward <LoadFuncInit> (load_func_init)));
    Init();
}

template <class LoadFunc, class Buffer>
template <class ...Types>
FileKeeper <LoadFunc, Buffer>::FileKeeper(
        const std::string& filename, Types&& ...args) {
    filename_ = filename;
    buffer_mgr_ptr_ = std::unique_ptr <BufferMgr> (new BufferMgr());
    load_func_ptr_ = std::unique_ptr <LoadFunc> (new LoadFunc(args...));
    Init();
}

template <class LoadFunc, class Buffer>
typename FileKeeper <LoadFunc, Buffer>::ConstPtrType
    FileKeeper <LoadFunc, Buffer>::Get() {
    return buffer_mgr_ptr_->Get();
}

template <class LoadFunc, class Buffer>
bool FileKeeper <LoadFunc, Buffer>::Load(const std::string& filename) {
    if (!buffer_mgr_ptr_->Released()) return false;

    Buffer buffer_tmp;
    bool load_ret = (*load_func_ptr_)(filename, buffer_tmp);
    if (!load_ret) return false;

    bool update_ret = buffer_mgr_ptr_->Update(std::move(buffer_tmp));
    if (!update_ret) return false;

    return true;
}

} // namespace iter

#endif // ITER_FILE_KEEPER_IMPL_HPP
