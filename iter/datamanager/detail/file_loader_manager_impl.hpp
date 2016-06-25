#ifndef ITER_FILE_LOADER_MANAGER_IMPL_HPP
#define ITER_FILE_LOADER_MANAGER_IMPL_HPP

#include <iter/log.hpp>
#include <sys/inotify.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <functional>
#include <cassert>

namespace iter {

FileLoaderManager::FileLoaderManager() {
    inotify_fd_ = inotify_init();
    shutdown_ = false;
    thread_ptr_ =
        std::shared_ptr <std::thread> (new std::thread(
            std::bind(&iter::FileLoaderManager::Callback, this)));
    assert(thread_ptr_); // NOTICE
    ITER_INFO_KV(MSG("Thread start."),
        KV("inotify_fd", inotify_fd_));
}

FileLoaderManager::~FileLoaderManager() {
    shutdown_ = true;
    if (thread_ptr_) {
        thread_ptr_->join();
        ITER_INFO_KV(MSG("Thread stop."),
            KV("inotify_fd", inotify_fd_));
    }
    int ret = close(inotify_fd_);
    if (ret == -1) {
        ITER_ERROR_KV(MSG("Inotify fd close failed."), KV(errno));
    }
}

FileLoaderManager* FileLoaderManager::GetInstance() {
    static FileLoaderManager file_loader_manager;
    return &file_loader_manager;
}

void FileLoaderManager::InsertFileLoader(
        Loader* loader_ptr, const std::string& filename) {
    // If file not exist, touch a new empty file.
    struct stat f_stat;
    if (stat(filename.c_str(), &f_stat) != 0){
        std::string cmd = "touch " + filename;
        system(cmd.c_str());
        ITER_WARN_KV(
            MSG("File not found and touched a new empty file."),
            KV(filename));
    }

    std::lock_guard <std::mutex> lck(mtx_);
    int watcher_fd = inotify_add_watch(
        inotify_fd_, filename.c_str(), ITER_INOTIFY_MASK);
    if (watcher_fd == -1) {
        ITER_ERROR_KV(MSG("Add watcher failed."), KV(errno), KV(filename));
        return;
    }
    Node node;
    node.loader_ptr = loader_ptr;
    node.filename = filename;
    file_map_[watcher_fd] = node;
    loader_map_[loader_ptr] = watcher_fd;
    ITER_INFO_KV(MSG("Insert success."), KV(watcher_fd), KV(filename));
}

void FileLoaderManager::DeleteFileLoader(
        Loader* loader_ptr, const std::string& filename) {
    std::lock_guard <std::mutex> lck(mtx_);
    int watcher_fd = -1;
    auto loader_it = loader_map_.find(loader_ptr);
    if (loader_it != loader_map_.end()) watcher_fd = loader_it->second;
    if (watcher_fd == -1) {
        ITER_WARN_KV(MSG("No such file loader."), KV(filename));
        return;
    }
    file_map_.erase(watcher_fd);
    loader_map_.erase(loader_ptr);

    int rm_ret = inotify_rm_watch(inotify_fd_, watcher_fd);
    if (rm_ret == -1) {
        ITER_ERROR_KV(MSG("Rm watcher failed."),
            KV(errno), KV(watcher_fd), KV(filename));
        return;
    }
    ITER_INFO_KV(MSG("Delete success."), KV(watcher_fd), KV(filename));
}

void FileLoaderManager::Callback() {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(inotify_fd_, &rfds);
    struct timeval tv;
    tv.tv_sec = ITER_FILE_LOADER_MANAGER_SELECT_TIMEOUT_SEC;
    tv.tv_usec = 0;
    int fdmax = inotify_fd_ + 1;

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
            Node node;
            bool trigger = false;
            if (event->mask & ITER_INOTIFY_MASK) {
                std::lock_guard <std::mutex> lck(mtx_); // NOTICE
                if (file_map_.find(event->wd) == file_map_.end()) {
                    ITER_WARN_KV(MSG("No such file watcher."),
                        KV("watcher_fd", event->wd), KV("event", event->mask));
                }
                else {
                    node = file_map_[event->wd];
                    trigger = true;
                }
            }
            // Watched event happened.
            if (trigger) {
                if (event->mask & (IN_DELETE_SELF | IN_MOVE_SELF)) {
                    DeleteFileLoader(node.loader_ptr, node.filename);
                    InsertFileLoader(node.loader_ptr, node.filename);
                }
                bool load_ret = node.loader_ptr->Load();
                if (!load_ret) {
                    ITER_WARN_KV(
                        MSG("Auto load failed."),
                        KV("filename", node.filename), KV("event", event->mask));                    }
                else {
                    ITER_INFO_KV(
                        MSG("Auto load success."),
                        KV("filename", node.filename), KV("event", event->mask));
                }
            }
            i += ITER_INOTIFY_EVENT_SIZE + event->len;
        }
    }
}

} // namespace iter

#endif // ITER_FILE_LOADER_MANAGER_IMPL_HPP
