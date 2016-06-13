#ifndef ITER_FILE_LOADER_MANAGER_HPP
#define ITER_FILE_LOADER_MANAGER_HPP

#include <iter/datamanager/detail/loader.hpp>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <memory>

#define ITER_FILE_LOADER_MANAGER_SELECT_TIMEOUT_SEC 2
#define ITER_INOTIFY_MASK       (IN_MODIFY | IN_DELETE_SELF | IN_MOVE)
#define ITER_INOTIFY_EVENT_SIZE (sizeof (struct inotify_event))
#define ITER_INOTIFY_BUF_LEN    (1024 * (ITER_INOTIFY_EVENT_SIZE + 16))

namespace iter {

class FileLoaderManager {
public:
    static FileLoaderManager* GetInstance();
    static bool IsDestructed();
    void InsertFileLoader(Loader* loader_ptr, const std::string& filename);
    void DeleteFileLoader(Loader* loader_ptr, const std::string& filename);
    void Callback();

private:
    typedef struct {
        Loader* loader_ptr;
        std::string filename;
    } Node;

    FileLoaderManager();
    ~FileLoaderManager();
    // The key is watcher fd.
    std::map <int, Node> file_map_;
    // Loader_ptr -> watcher fd.
    std::map <Loader*, int> loader_map_;

    int inotify_fd_;
    std::shared_ptr <std::thread> thread_ptr_;
    std::mutex mtx_;
    bool shutdown_;
};

} // namespace iter

#include <iter/datamanager/detail/file_loader_manager_impl.hpp>

#endif // ITER_FILE_LOADER_MANAGER_HPP
