#ifndef ITER_SAFE_QUEUE_HPP
#define ITER_SAFE_QUEUE_HPP

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>
#include <type_traits>

namespace iter {

namespace _helper {
    // Adapt the API of queue and priority_queue.
    template <class T> struct HasFront {
        template <class U, class R> static void Traits(R (U::*) ()) {}
        template <class> struct Helper;
        template <class U> static bool Match(Helper <decltype(Traits(&U::front))>*) {}
        template <class U> static int Match(...) {}

        static const bool value = sizeof(Match <T> (NULL)) == sizeof(bool);
    };

    template <class T> struct HasTop {
        template <class U, class R> static void Traits(R (U::*) ()) {}
        template <class> struct Helper;
        template <class U> static bool Match(Helper <decltype(Traits(&U::top))>*) {}
        template <class U> static int Match(...) {}

        static const bool value = sizeof(Match <T> (NULL)) == sizeof(bool);
    };

    template <template <typename...> class Q, class V,
            class = typename std::enable_if <HasFront <Q <V>>::value>::type>
    static V GetElement(Q <V>& queue) {
        return queue.front();
    }

    template <template <typename...> class Q, class V,
            class = typename std::enable_if <!HasFront <Q <V>>::value && HasTop <Q <V>>::value>::type>
    static V GetElement(Q <V>& queue) {
        return queue.top();
    }

} // namespace _helper

template <class ValueType, class Queue = std::queue <ValueType>>
class SafeQueue {
public:
    typedef Queue QueueType;

    SafeQueue() : shutdown_(false), queue_ptr_(new Queue()) {}

    ~SafeQueue() {
        { // Critical region.
            std::lock_guard <std::mutex> lck(mtx_);
            shutdown_ = true;
        }
        cv_.notify_all();
    }

    int Size() {
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
        if (result != NULL) *result = _helper::GetElement(*queue_ptr_);
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
        if (result != NULL) *result = _helper::GetElement(*queue_ptr_);
        queue_ptr_->pop();
        return true;
    }

    // Get with timeout.
    template <class Rep, class Period>
    bool Get(ValueType* result, const std::chrono::duration <Rep, Period>& timeout) {
        std::unique_lock <std::mutex> lck(mtx_);
        cv_.wait(lck, timeout, [this] { return shutdown_ || !Empty(); });
        if (Empty()) return false;
        if (result != NULL) *result = _helper::GetElement(*queue_ptr_);
        queue_ptr_->pop();
        return true;
    }

private:
    bool shutdown_;
    std::unique_ptr <Queue> queue_ptr_;
    std::mutex mtx_;
    std::condition_variable cv_;
};

} // namespace iter

#endif // ITER_SAFE_QUEUE_HPP
