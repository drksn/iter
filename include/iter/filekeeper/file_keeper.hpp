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
    DoubleBuffer <Buffer> double_buffer_;

public:
    FileKeeper(const Loader& loader, const std::string& filename,
        const std::shared_ptr <FileMonitor>& file_monitor_ptr = std::shared_ptr <FileMonitor>());

    ~FileKeeper();

    // Get the const shared pointer of buffer,
    auto Get() -> decltype(double_buffer_.Get());

    // Validation check.
    operator bool ();

private:
    bool CheckFile();
    bool Load();
    void Callback(const FileEvent& file_event);

    Loader loader_;
    std::string filename_;

    int owner_id_;
    FileMonitor::Node node_;
    std::shared_ptr <FileMonitor> file_monitor_ptr_;

    std::mutex mtx_;
};

namespace file_keeper {

static std::shared_ptr <FileMonitor> g_file_monitor_ptr; // NOTICE
static std::mutex g_mtx;

} // namespace file_keeper

} // namespace iter

#include <iter/filekeeper/detail/file_keeper_impl.hpp>

#endif // ITER_FILE_KEEPER_HPP
