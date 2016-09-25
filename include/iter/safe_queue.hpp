#ifndef ITER_SAFE_QUEUE_HPP
#define ITER_SAFE_QUEUE_HPP

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>

namespace iter {

template <class ValueType, class Container = std::deque <ValueType>>
class SafeQueue {
private:
    typedef std::queue <ValueType, Container> Queue;

    std::unique_ptr <Queue> queue_ptr_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool shutdown_;

public:
    SafeQueue() : shutdown_(false),
        queue_ptr_(std::unique_ptr <Queue>(new Queue())) {}

    ~SafeQueue() {
        { // Critical region.
            std::lock_guard <std::mutex> lck(mtx_);
            shutdown_ = true;
        }
        cv_.notify_all();
    }

    decltype(Queue::size()) Size() {
        return queue_ptr_->size();
    }

    bool Empty() { return queue_ptr_->empty(); }

    void Push(const ValueType& val) {
        std::lock_guard <std::mutex> lck(mtx_);
        queue_ptr_->push(val);
        cv_.notify_one();
    }

    void Push(ValueType&& val) {
        std::lock_guard <std::mutex> lck(mtx_);
        queue_ptr_->push(std::move(val));
        cv_.notify_one();
    }

    bool Front(ValueType* result) {
        if (result == NULL) return false;

        std::lock_guard <std::mutex> lck(mtx_);
        if (Empty()) return false;
        *result = queue_ptr_->front();
        return true;
    }

    // Get the element in tyyhe front of the queue and pop it.
    bool Pop(ValueType* result = NULL) {
        std::lock_guard <std::mutex> lck(mtx_);
        if (Empty()) return false;
        if (result != NULL) *result = std::move(queue_ptr_->front());
        queue_ptr_->pop();
        return true;
    }

    // Get the whole queue.
    std::unique_ptr <Queue> PopAll() {
        std::lock_guard <std::mutex> lck(mtx_);
        auto result = std::move(queue_ptr_);
        queue_ptr_ = std::unique_ptr <Queue>(new Queue());
        return result;
    }

    // Get the element in tyyhe front of the queue and pop it.
    // It will be blocked until the queue is not empty or shutdown.
    bool Get(ValueType* result) {
        std::unique_lock <std::mutex> lck(mtx_);
        cv_.wait(lck, [this] { return shutdown_ || !Empty(); });
        if (Empty()) return false;
        if (result != NULL) *result = std::move(queue_ptr_->front());
        queue_ptr_->pop();
        return true;
    }
};

} // namespace iter

#endif // ITER_SAFE_QUEUE_HPP
