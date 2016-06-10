#ifndef ITER_FILE_LOADER_MANAGER_HPP
#define ITER_FILE_LOADER_MANAGER_HPP

#include <iter/datamanager/detail/loader.hpp>
#include <iter/log.hpp>
#include <map>
#include <mutex>
#include <string>

namespace iter {

class FileLoaderManager {
public:
    void InsertFileLoader(Loader* loader_ptr, const std::string& filename);
    void DeleteFileLoader(Loader* loader_ptr);

private:
    typedef struct {
        std::string filename;
    } Node;

    std::map <Loader*, Node> file_map_;
    std::mutex mtx_;
};

} // namespace iter

#include <iter/datamanager/detail/file_loader_manager_impl.hpp>

#endif // ITER_FILE_LOADER_MANAGER_HPP
