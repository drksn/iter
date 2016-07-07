#ifndef ITER_FILE_LOADER_MANAGER_HPP
#define ITER_FILE_LOADER_MANAGER_HPP

#include <iter/util/thread_pool.hpp>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#define ITER_FILE_LOADER_MANAGER_THREAD_POOL_SIZE 5
#define ITER_FILE_LOADER_MANAGER_SELECT_TIMEOUT_SEC 1
#define ITER_INOTIFY_MASK       (IN_MODIFY | IN_DELETE_SELF | IN_MOVE_SELF)
#define ITER_INOTIFY_EVENT_SIZE (sizeof(struct inotify_event))
#define ITER_INOTIFY_BUF_LEN    (1024 * (ITER_INOTIFY_EVENT_SIZE + 16))

namespace iter {

class FileLoaderManager {
public:
    static FileLoaderManager* GetInstance();

    bool InsertFileLoader(void* ptr,
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename);

    bool DeleteFileLoader(void* ptr);

    std::future <bool> PushTask(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename);

private:
    FileLoaderManager(size_t pool_size);
    ~FileLoaderManager();
    void WatcherCallback();

    typedef struct {
        void* ptr;
        int watcher_fd;
        std::string filename;
        std::function <bool(const std::string&)> loader;
    } Node;

    std::map <int, Node> watcher_map_; // The key is watcher fd.
    std::map <void*, Node> ptr_map_;

    bool shutdown_;
    size_t thread_pool_size_;
    int inotify_fd_;
    std::shared_ptr <ThreadPool> thread_pool_ptr_;
    std::mutex mtx_;
};

} // namespace iter

#include <iter/filekeeper/detail/file_loader_manager_impl.hpp>

#endif // ITER_FILE_LOADER_MANAGER_HPP
