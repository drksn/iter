#ifndef ITER_FILE_KEEPER_HPP
#define ITER_FILE_KEEPER_HPP

#include <iter/file_monitor.hpp>
#include <iter/util/double_buffer.hpp>
#include <string>
#include <memory>
#include <mutex>
#include <type_traits>

#define ITER_FILE_KEEPER_EVENT_MASK   (IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF)

namespace iter {

template <class Buffer>
class FileKeeper {
private:
    typedef std::function <bool(const std::string&, Buffer*)> Loader;
    typedef std::unique_ptr <DoubleBuffer <Buffer>> BufferPtr;
    BufferPtr buffer_ptr_;

public:
    FileKeeper(
        const std::string& filename, const Loader& loader,
        const std::shared_ptr <FileMonitor>& file_monitor_ptr);

    ~FileKeeper();

    // Get the const shared pointer of 'Buffer'.
    auto Get() -> decltype(buffer_ptr_->Get());

    // Load immediately, without wait modification events occurred.
    bool Load();

    // Validation check.
    operator bool ();

private:
    void FixFile();
    void Callback(const FileEvent& file_event);

    std::string filename_;
    Loader loader_;

    int owner_id_;
    FileMonitor::Node node_;
    std::shared_ptr <FileMonitor> file_monitor_ptr_;

    std::mutex mtx_;
};

} // namespace iter

#include <iter/filekeeper/detail/file_keeper_impl.hpp>

#endif // ITER_FILE_KEEPER_HPP
