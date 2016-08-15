#ifndef ITER_FILE_KEEPER_HPP
#define ITER_FILE_KEEPER_HPP

#include <iter/file_monitor.hpp>
#include <iter/util/double_buffer.hpp>
#include <string>
#include <memory>
#include <mutex>
#include <type_traits>

#define ITER_FILE_KEEPER_EVENT_MASK   (IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF)
#define ITER_FILE_KEEPER_THREAD_POOL_SIZE 3

namespace iter {

template <class LoadFunc, class Buffer =
    typename std::remove_reference <typename LoadFunc::second_argument_type>::type>
class FileKeeper {
private:
    typedef DoubleBuffer <Buffer> BufferMgr;
    std::unique_ptr <BufferMgr> buffer_mgr_ptr_;

public:
    template <class ...Types>
    FileKeeper(const std::string& filename, Types&& ...args);

    ~FileKeeper();
    // Get the const shared pointer of buffer,
    // if buffer is empty, return NULL.
    auto Get() -> decltype(buffer_mgr_ptr_->Get());

    // Validation check.
    operator bool ();

private:
    bool CheckFile();
    bool Load();
    void Callback(const FileEvent& file_event);

    FileMonitor::Node node_;

    std::string filename_;
    std::unique_ptr <LoadFunc> load_func_ptr_;
    std::mutex mtx_;
    int owner_id_;

    static FileMonitor g_file_monitor; // NOTICE
};

template <class LoadFunc, class Buffer>
FileMonitor FileKeeper <LoadFunc, Buffer>::g_file_monitor(ITER_FILE_KEEPER_THREAD_POOL_SIZE);

} // namespace iter

#include <iter/filekeeper/detail/file_keeper_impl.hpp>

#endif // ITER_FILE_KEEPER_HPP

