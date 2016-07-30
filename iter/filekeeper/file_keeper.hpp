#ifndef ITER_FILE_KEEPER_HPP
#define ITER_FILE_KEEPER_HPP

#include <iter/filekeeper/detail/file_loader_register.hpp>
#include <iter/util/double_buffer.hpp>
#include <string>
#include <memory>
#include <mutex>
#include <type_traits>

namespace iter {

template <class LoadFunc, class Buffer =
    typename std::remove_reference <typename LoadFunc::second_argument_type>::type>
class FileKeeper {
private:
    typedef DoubleBuffer <Buffer> BufferMgr;
    std::unique_ptr <BufferMgr> buffer_mgr_ptr_;

public:
    template <class LoadFuncInit>
    FileKeeper(const std::string& filename,
        LoadFuncInit&& load_func_init);
    // It will be matched only if the size of args pack is zero.
    template <class ...Types>
    FileKeeper(const std::string& filename, Types&& ...args);
    // Get the const shared pointer of buffer,
    // if buffer is empty, return NULL.
    auto Get() -> decltype(buffer_mgr_ptr_->Get());

private:
    void Init();
    // If the corresponding file is modified,
    // FileLoaderManager will call this function automatically.
    bool Load(const std::string& filename);

    std::string filename_;
    std::unique_ptr <LoadFunc> load_func_ptr_;
    std::mutex mtx_;
    FileLoaderRegister register_;
};

} // namespace iter

#include <iter/filekeeper/detail/file_keeper_impl.hpp>

#endif // ITER_FILE_KEEPER_HPP

