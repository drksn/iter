#ifndef ITER_UNIQUE_QUEUE_HPP
#define ITER_UNIQUE_QUEUE_HPP

#include <mutex>
#include <queue>
#include <deque>
#include <memory>
#include <utility>

namespace iter {

template <class ValueType, class Container = std::deque <ValueType>>
class UniqueQueue {
private:
    typedef std::queue <ValueType, Container> Queue;

public:
    UniqueQueue();
    std::unique_ptr <Queue> Get();
    void Push(const ValueType& val);
    void Push(ValueType&& val);

private:
    std::unique_ptr <Queue> queue_ptr_;
    std::mutex mtx_;
};

template <class ValueType, class Container>
UniqueQueue <ValueType, Container>::UniqueQueue()
        :queue_ptr_(std::unique_ptr <Queue>(new Queue())){}

template <class ValueType, class Container>
std::unique_ptr <typename UniqueQueue <ValueType, Container>::Queue> UniqueQueue <ValueType, Container>::Get() {
    auto tmp = std::move(queue_ptr_);
    queue_ptr_ = std::unique_ptr <Queue>(new Queue());
    return tmp;
}

template <class ValueType, class Container>
void UniqueQueue <ValueType, Container>::Push(const ValueType& val) {
    std::lock_guard <std::mutex> lck(mtx_);
    queue_ptr_->push(val);
}

template <class ValueType, class Container>
void UniqueQueue <ValueType, Container>::Push(ValueType&& val) {
    std::lock_guard <std::mutex> lck(mtx_);
    queue_ptr_->push(std::move(val));
}

} // namespace iter

#endif // ITER_UNIQUE_QUEUE_HPP
