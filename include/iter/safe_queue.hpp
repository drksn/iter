#ifndef ITER_SAFE_QUEUE_HPP
#define ITER_SAFE_QUEUE_HPP

#include <chrono>
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

    bool shutdown_;
    std::unique_ptr <Queue> queue_ptr_;
    std::mutex mtx_;
    std::condition_variable cv_;

public:
    SafeQueue() : shutdown_(false), queue_ptr_(new Queue()) {}

    ~SafeQueue() {
        { // Critical region.
            std::lock_guard <std::mutex> lck(mtx_);
            shutdown_ = true;
        }
        cv_.notify_all();
    }

    typename std::result_of <decltype(&Queue::size)(Queue)>::type Size() {
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

    // Get the element in the front of the queue and pop it.
    // Return false when the queue is empty.
    bool Pop(ValueType* result) {
        std::lock_guard <std::mutex> lck(mtx_);
        if (Empty()) return false;
        if (result != NULL) *result = std::move(queue_ptr_->front());
        queue_ptr_->pop();
        return true;
    }

    // Pop the whole queue.
    std::unique_ptr <Queue> PopAll() {
        std::lock_guard <std::mutex> lck(mtx_);
        std::unique_ptr <Queue> result(new Queue());
        std::swap(result, queue_ptr_);
        return result;
    }

    // Wait until the queue is not empty or the queue is shutdown.
    // Return false when the queue is empty.
    bool Wait() {
        std::unique_lock <std::mutex> lck(mtx_);
        cv_.wait(lck, [this] { return shutdown_ || !Empty(); });
        return !Empty();
    }

    // Wait with timeout.
    template <class Rep, class Period>
    bool WaitFor(const std::chrono::duration <Rep, Period>& timeout) {
        std::unique_lock <std::mutex> lck(mtx_);
        cv_.wait(lck, timeout, [this] { return shutdown_ || !Empty(); });
        return !Empty();
    }

    // Get the element in the front of the queue and pop it.
    // It will be BLOCKED until the queue is not empty or shutdown.
    bool Get(ValueType* result) {
        std::unique_lock <std::mutex> lck(mtx_);
        cv_.wait(lck, [this] { return shutdown_ || !Empty(); });
        if (Empty()) return false;
        if (result != NULL) *result = std::move(queue_ptr_->front());
        queue_ptr_->pop();
        return true;
    }

    // Get with timeout.
    template <class Rep, class Period>
    bool Get(ValueType* result, const std::chrono::duration <Rep, Period>& timeout) {
        std::unique_lock <std::mutex> lck(mtx_);
        cv_.wait(lck, timeout, [this] { return shutdown_ || !Empty(); });
        if (Empty()) return false;
        if (result != NULL) *result = std::move(queue_ptr_->front());
        queue_ptr_->pop();
        return true;
    }
};

} // namespace iter

#endif // ITER_SAFE_QUEUE_HPP
