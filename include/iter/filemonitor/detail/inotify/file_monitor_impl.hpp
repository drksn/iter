/*
 *  Inotify version.
 */
#ifndef ITER_FILE_MONITOR_IMPL_HPP
#define ITER_FILE_MONITOR_IMPL_HPP

#include <iter/util/registry.hpp>
#include <iter/log.hpp>
#include <sys/inotify.h>
#include <sys/select.h>
#include <sys/time.h>

#define ITER_FILE_MONITOR_SELECT_TIMEOUT_SEC 1

#define ITER_INOTIFY_EVENT_SIZE (sizeof(struct inotify_event))
#define ITER_INOTIFY_BUF_LEN    (1024 * (ITER_INOTIFY_EVENT_SIZE + 16))

namespace iter {

class FileMonitor::Impl {
public:
    Impl(const std::shared_ptr <ThreadPool>& thread_pool_ptr);
    ~Impl();

    int Register(const Node& node);
    void Remove(int owner_id);

    bool AddWatcher(int owner_id, const std::string& filename, uint32_t event_mask);
    void Callback();

    Registry <Node> registry_;

    std::map <int, int> oid_wfd_map_;
    std::map <int, int> wfd_oid_map_;
    int inotify_fd_;

    std::shared_ptr <ThreadPool> thread_pool_ptr_;
    std::mutex mtx_;
    bool shutdown_;
};

FileMonitor::FileMonitor(size_t thread_pool_size) {
    if (thread_pool_size < 2) thread_pool_size = 2; // NOTICE
    impl_ = std::unique_ptr <Impl> (new Impl(std::make_shared <ThreadPool> (thread_pool_size)));
}

FileMonitor::FileMonitor(const std::shared_ptr <ThreadPool>& thread_pool_ptr) {
    impl_ = std::unique_ptr <Impl> (new Impl(thread_pool_ptr));
}

bool FileMonitor::IsRegistered(int owner_id) {
    return impl_->registry_.IsRegistered(owner_id);
}

int FileMonitor::Register(const Node& node) {
    return impl_->Register(node);
}

void FileMonitor::Remove(int owner_id) {
    impl_->Remove(owner_id);
}

FileMonitor::operator bool() {
    return impl_->inotify_fd_ != -1;
}

FileMonitor::Impl::Impl(const std::shared_ptr <ThreadPool>& thread_pool_ptr)
        : thread_pool_ptr_(thread_pool_ptr) {
    shutdown_ = false;
    inotify_fd_ = inotify_init();
    if (inotify_fd_ == -1) {
        ITER_ERROR_KV(MSG("Inotify init failed."), KV(errno));
        return;
    }
    thread_pool_ptr_->PushTask(
        std::bind(&FileMonitor::Impl::Callback, this));
}

FileMonitor::Impl::~Impl() {
    shutdown_ = true;
    int ret = close(inotify_fd_);
    if (ret == -1) {
        ITER_ERROR_KV(MSG("Inotify close failed."), KV(errno));
    }
}

bool FileMonitor::Impl::AddWatcher(
        int owner_id, const std::string& filename, uint32_t event_mask) {
    std::lock_guard <std::mutex> lck(mtx_);
    int watcher_fd = inotify_add_watch(
        inotify_fd_, filename.c_str(), event_mask);
    if (watcher_fd == -1) {
        ITER_WARN_KV(MSG("Add watcher failed."), KV(errno), KV(filename));
        return false;
    }
    oid_wfd_map_.emplace(owner_id, watcher_fd);
    wfd_oid_map_.emplace(watcher_fd, owner_id);
    return true;
}

int FileMonitor::Impl::Register(const Node& node) {
    int owner_id = registry_.Register(node);
    bool ret = AddWatcher(owner_id, node.filename, node.event_mask);
    return ret ? owner_id : -1;
}

void FileMonitor::Impl::Remove(int owner_id) {
    std::lock_guard <std::mutex> lck(mtx_);
    auto it = oid_wfd_map_.find(owner_id);
    if (it == oid_wfd_map_.end()) return;
    int watcher_fd = it->second;
    oid_wfd_map_.erase(owner_id);
    wfd_oid_map_.erase(watcher_fd);

    int rm_ret = inotify_rm_watch(inotify_fd_, watcher_fd);
    if (rm_ret == -1) {
        ITER_WARN_KV(MSG("Remove watcher failed."), KV(errno), KV(watcher_fd));
    }
}

void FileMonitor::Impl::Callback() {
    // 'select' settings.
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(inotify_fd_, &rfds);
    struct timeval tv;
    tv.tv_sec = ITER_FILE_MONITOR_SELECT_TIMEOUT_SEC;
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

        for (int i = 0; i < length; ) {
            struct inotify_event *event = (struct inotify_event*) &buffer[i];
            i += ITER_INOTIFY_EVENT_SIZE + event->len;
            Node node;
            { // Critical region.
                std::lock_guard <std::mutex> lck(mtx_);
                int watcher_fd = event->wd;
                auto wit = wfd_oid_map_.find(watcher_fd);
                if (wit == wfd_oid_map_.end()) continue;

                int owner_id = wit->second;
                if (!registry_.IsRegistered(owner_id)) continue;
                node = registry_.Get(owner_id);
            }
            if (!(node.event_mask & event->mask)) continue;

            FileEvent file_event;
            file_event.mask = event->mask;
            file_event.cookie = event->cookie;
            for (size_t j = 0; j < event->len; j++) file_event.name += event->name[j];

            thread_pool_ptr_->PushTask(node.callback, file_event);
        }
    }
}

} // namespace iter

#endif // ITER_FILE_MONITOR_IMPL_HPP
