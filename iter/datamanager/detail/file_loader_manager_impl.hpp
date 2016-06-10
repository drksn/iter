#ifndef ITER_FILE_LOADER_MANAGER_IMPL_HPP
#define ITER_FILE_LOADER_MANAGER_IMPL_HPP

namespace iter {

void FileLoaderManager::InsertFileLoader(
        Loader* loader_ptr, const std::string& filename) {
    Node node;
    node.filename = filename;

    std::lock_guard <std::mutex> lck(mtx_);
    file_map_[loader_ptr] = node;
    bool ret = loader_ptr->Load();
    if (!ret) {
        ITER_WARN_KV(MSG("Load failed."), KV(filename));
    }
    else {
        ITER_INFO_KV(MSG("Load success."), KV(filename));
    }
}

void FileLoaderManager::DeleteFileLoader(Loader* loader_ptr) {
    std::lock_guard <std::mutex> lck(mtx_);
    file_map_.erase(loader_ptr);
}

} // namespace iter

#endif // ITER_FILE_LOADER_MANAGER_IMPL_HPP
