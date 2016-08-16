#ifndef ITER_FILE_MONITOR_HPP
#define ITER_FILE_MONITOR_HPP

#include <iter/util/thread_pool.hpp>
#include <functional>
#include <memory>
#include <string>

#define ITER_FILE_MONITOR_POOL_SIZE 4

namespace iter {

struct FileEvent {
    uint32_t mask;
    uint32_t cookie;
    std::string name;
};

class FileMonitor {
public:
    typedef struct {
        std::string filename;
        uint32_t event_mask;
        std::function <void(const FileEvent&)> callback;
    } Node;

    FileMonitor(size_t thread_pool_size = ITER_FILE_MONITOR_POOL_SIZE);
    FileMonitor(std::shared_ptr <ThreadPool> thread_pool_ptr);

    bool IsRegistered(int owner_id);
    int Register(const Node& node);
    void Remove(int owner_id);

    // Validation check.
    operator bool();

private:
    class Impl;
    std::unique_ptr <Impl> impl_;
};

} // namespace iter

#include <iter/filemonitor/detail/inotify/file_monitor_impl.hpp>

#endif // ITER_FILE_MONITOR_HPP
