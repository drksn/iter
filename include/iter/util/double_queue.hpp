#ifndef ITER_DOUBLE_QUEUE_HPP
#define ITER_DOUBLE_QUEUE_HPP

#include <mutex>
#include <deque>
#include <utility>

namespace iter {

template <class ValueType, class Container = std::deque <ValueType>>
class DoubleQueue {
public:
    DoubleQueue();
    Container* Get();
    void Push(const ValueType& val);
    void Push(ValueType&& val);
    // Return false if the active queue is empty.
    bool Pop();
    void Switch();

private:
    Container queue_[2];
    int active_idx_;
    std::mutex mtx_;
};

template <class ValueType, class Container>
DoubleQueue <ValueType, Container>::DoubleQueue():active_idx_(0){}

template <class ValueType, class Container>
Container* DoubleQueue <ValueType, Container>::Get() {
    return &queue_[active_idx_];
}

template <class ValueType, class Container>
void DoubleQueue <ValueType, Container>::Push(const ValueType& val) {
    std::lock_guard <std::mutex> lck(mtx_);
    queue_[active_idx_].push(val);
}

template <class ValueType, class Container>
void DoubleQueue <ValueType, Container>::Push(ValueType&& val) {
    std::lock_guard <std::mutex> lck(mtx_);
    queue_[active_idx_].push(std::move(val));
}

template <class ValueType, class Container>
bool DoubleQueue <ValueType, Container>::Pop() {
    std::lock_guard <std::mutex> lck(mtx_);
    if (queue_[active_idx_].size() == 0) return false;
    queue_[active_idx_].pop();
    return true;
}

template <class ValueType, class Container>
void DoubleQueue <ValueType, Container>::Switch() {
    std::lock_guard <std::mutex> lck(mtx_);
    active_idx_ ^= 1;
}

} // namespace iter

#endif // ITER_DOUBLE_QUEUE_HPP
