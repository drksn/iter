#ifndef ITER_FILE_MONITOR_BASE_HPP
#define ITER_FILE_MONITOR_BASE_HPP

#include <functional>
#include <map>
#include <mutex>
#include <string>

namespace iter {

struct FileEvent {
    uint32_t mask;
    uint32_t cookie;
    std::string name;
};

class FileMonitorBase {
public:
    bool IsRegistered(int owner_id);

    virtual int Register(
        const std::string& filename,
        const std::function <void(const FileEvent&)>& callback,
        uint32_t event_mask);

    virtual void Remove(int owner_id);

protected:
    typedef struct {
        std::string filename;
        std::function <void(const FileEvent&)> callback;
        uint32_t event_mask;
    } Node;

    std::map <int, Node> owner_map_;
    int owner_id_nxt_;
    std::mutex mtx_;
};

bool FileMonitorBase::IsRegistered(int owner_id) {
    return owner_map_.find(owner_id) != owner_map_.end();
}

int FileMonitorBase::Register(
        const std::string& filename,
        const std::function <void(const FileEvent&)>& callback,
        uint32_t event_mask) {
    std::lock_guard <std::mutex> lck(mtx_);
    owner_id_nxt_ ++;
    owner_map_.emplace(owner_id_nxt_, Node {filename, callback, event_mask});
    return owner_id_nxt_;
}

void FileMonitorBase::Remove(int owner_id) {
    std::lock_guard <std::mutex> lck(mtx_);
    owner_map_.erase(owner_id);
}

} // namespace iter

#endif // ITER_FILE_MONITOR_BASE_HPP
