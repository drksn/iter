#ifndef ITER_FILE_MONITOR_HPP
#define ITER_FILE_MONITOR_HPP

#include <iter/filemonitor/file_monitor_base.hpp>
#include <iter/util/thread_pool.hpp>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#define ITER_FILE_MONITOR_EVENT_MASK 0xffffffff
#define ITER_FILE_MONITOR_GLOB_THREAD_POOL_SIZE 3

namespace iter {

class FileMonitor : public FileMonitorBase{
public:
    FileMonitor(size_t thread_pool_size);
    FileMonitor(const std::shared_ptr <ThreadPool>& thread_pool_ptr);

    virtual int Register(
        const std::string& filename,
        const std::function <void(const FileEvent&)>& callback,
        uint32_t event_mask = ITER_FILE_MONITOR_EVENT_MASK);

    virtual void Remove(int owner_id);

private:
    class Impl;
    std::unique_ptr <Impl> impl_;
};

static FileMonitor g_file_monitor(ITER_FILE_MONITOR_GLOB_THREAD_POOL_SIZE);

} // namespace iter

#include <iter/filemonitor/detail/inotify/file_monitor_impl.hpp>

#endif // ITER_FILE_MONITOR_HPP
