#ifndef ITER_FILE_LOADER_MANAGER_IMPL_HPP
#define ITER_FILE_LOADER_MANAGER_IMPL_HPP

#include <iter/log.hpp>
#include <sys/inotify.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <functional>

namespace iter {

FileLoaderManager::FileLoaderManager(size_t pool_size) {
    shutdown_ = false;
    thread_pool_size_ = pool_size;
    inotify_fd_ = inotify_init();
    if (inotify_fd_ == -1) {
        ITER_ERROR_KV(MSG("Inotify init failed."), KV(errno));
        return;
    }
    thread_pool_ptr_ = std::make_shared <ThreadPool> (thread_pool_size_);
    thread_pool_ptr_->PushTask(
        std::bind(&iter::FileLoaderManager::WatcherCallback, this));
}

FileLoaderManager::~FileLoaderManager() {
    shutdown_ = true;
    int ret = close(inotify_fd_);
    if (ret == -1) {
        ITER_ERROR_KV(MSG("Inotify fd close failed."), KV(errno));
    }
}

std::future <bool> FileLoaderManager::PushTask(
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename) {
    return thread_pool_ptr_->PushTask(
            [=]() {
                bool ret = loader(filename);
                if (ret)
                    ITER_INFO_KV(MSG("Auto load success."), KV(filename));
                else
                    ITER_WARN_KV(MSG("Auto load failed."), KV(filename));
                return ret;
            }
        );
}

FileLoaderManager* FileLoaderManager::GetInstance() {
    static FileLoaderManager file_loader_manager(
        ITER_FILE_LOADER_MANAGER_THREAD_POOL_SIZE); // NOTICE
    return &file_loader_manager;
}

bool FileLoaderManager::InsertFileLoader(
        const std::string& owner_id,
        const std::function <bool(const std::string&)>& loader,
        const std::string& filename) {
    // If file not exist, touch a new empty file.
    struct stat f_stat;
    if (stat(filename.c_str(), &f_stat) != 0){
        std::string cmd = "touch " + filename;
        system(cmd.c_str());
        ITER_WARN_KV(
            MSG("File not found, touched a new empty file."),
            KV(filename));
    }
    // Add watcher.
    std::lock_guard <std::mutex> lck(mtx_);
    int watcher_fd = inotify_add_watch(
        inotify_fd_, filename.c_str(), ITER_INOTIFY_MASK);
    if (watcher_fd == -1) {
        ITER_WARN_KV(MSG("Add watcher failed."), KV(errno), KV(filename));
        return false;
    }
    // Maintain dicts.
    Node node = {owner_id, watcher_fd, filename, loader};
    watcher_map_[watcher_fd] = node;
    owner_map_[owner_id] = node;
    return true;
}

bool FileLoaderManager::DeleteFileLoader(
    const std::string& owner_id) {
    std::lock_guard <std::mutex> lck(mtx_);
    auto it = owner_map_.find(owner_id);
    if (it == owner_map_.end()) return false;

    Node node = it->second;
    owner_map_.erase(owner_id);
    watcher_map_.erase(node.watcher_fd);
    int rm_ret = inotify_rm_watch(inotify_fd_, node.watcher_fd);
    if (rm_ret == -1) {
        ITER_WARN_KV(MSG("Call inotify_rm_watch failed."),
            KV(errno), KV("watcher_fd", node.watcher_fd),
            KV("filename", node.filename));
        return false;
    }
    return true;
}

void FileLoaderManager::WatcherCallback() {
    // 'select' settings.
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(inotify_fd_, &rfds);
    struct timeval tv;
    tv.tv_sec = ITER_FILE_LOADER_MANAGER_SELECT_TIMEOUT_SEC;
    tv.tv_usec = 0;
    int fdmax = inotify_fd_ + 1;
    // 'inotify' buffer.
    char buffer[ITER_INOTIFY_BUF_LEN];
    while (!shutdown_) {
        int retval = select(fdmax, &rfds, NULL, NULL, &tv);
        if (retval == -1) {
            ITER_WARN_KV(MSG("Select error."), KV(errno));
            continue;
        }
        // The number of fd in select, and we have only one.
        if (retval != 1) continue;
        int length = read(inotify_fd_, buffer, ITER_INOTIFY_BUF_LEN);
        if (length < 0) continue;
        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event*) &buffer[i];
            bool trigger = false;
            Node node;
            if (event->mask & ITER_INOTIFY_MASK) {
                std::lock_guard <std::mutex> lck(mtx_); // NOTICE
                auto it = watcher_map_.find(event->wd);
                if (it == watcher_map_.end()) {
                    ITER_WARN_KV(MSG("No such file watcher."),
                        KV("watcher_fd", event->wd), KV("event", event->mask));
                }
                else {
                    node = it->second;
                    trigger = true;
                }
            }
            // Watched event happened.
            if (trigger) {
                ITER_INFO_KV(MSG("Event triggered."),
                    KV("filename", node.filename), KV("event_mask", event->mask));
                if (event->mask & (IN_DELETE_SELF | IN_MOVE_SELF)) {
                    DeleteFileLoader(node.owner_id);
                    InsertFileLoader(node.owner_id, node.loader, node.filename);
                }
                PushTask(node.loader, node.filename);
            }
            i += ITER_INOTIFY_EVENT_SIZE + event->len;
        }
    }
}

} // namespace iter

#endif // ITER_FILE_LOADER_MANAGER_IMPL_HPP
