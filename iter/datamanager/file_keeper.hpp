#ifndef ITER_FILE_KEEPER_HPP
#define ITER_FILE_KEEPER_HPP

#include <iter/datamanager/double_buffer.hpp>
#include <iter/datamanager/file_loader_register.hpp>
#include <string>
#include <memory>
#include <mutex>
#include <type_traits>

namespace iter {

template <class LoadFunc, class Buffer =
    typename std::remove_reference <typename LoadFunc::second_argument_type>::type>
class FileKeeper {
public:
    template <class LoadFuncInit, class ...Types>
    FileKeeper(const std::string& filename,
        LoadFuncInit&& load_func_init, Types&& ...args);

    template <class ...Types>
    FileKeeper(const std::string& filename, Types&& ...args);

    bool GetBuffer(std::shared_ptr <Buffer>* ptr);

private:
    void Init();
    // If the corresponding file is modified,
    // FileLoaderManager will call this function automatically.
    bool Load(const std::string& filename);

    typedef DoubleBuffer <Buffer> BufferMgr;
    std::string filename_;
    std::unique_ptr <BufferMgr> buffer_mgr_ptr_;
    std::unique_ptr <LoadFunc> load_func_ptr_;
    std::mutex mtx_;
    FileLoaderRegister register_;
};

} // namespace iter

#include <iter/datamanager/detail/file_keeper_impl.hpp>

#endif // ITER_FILE_KEEPER_HPP

