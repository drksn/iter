#ifndef ITER_FILE_LOADER_MANAGER_HPP
#define ITER_FILE_LOADER_MANAGER_HPP

#include <iter/datamanager/detail/loader.hpp>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <memory>

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
        std::string filename;
        int watcher_fd;
    } Node;

    FileLoaderManager();
    ~FileLoaderManager();

    std::map <Loader*, Node> file_map_;
    int inotify_fd_;
    std::shared_ptr <std::thread> thread_ptr_;
    std::mutex mtx_;
    bool shutdown_;
};

} // namespace iter

#include <iter/datamanager/detail/file_loader_manager_impl.hpp>

#endif // ITER_FILE_LOADER_MANAGER_HPP
