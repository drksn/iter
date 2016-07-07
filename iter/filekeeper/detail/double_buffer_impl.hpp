#ifndef ITER_DOUBLE_BUFFER_IMPL_HPP
#define ITER_DOUBLE_BUFFER_IMPL_HPP

#include <memory>
#include <mutex>
#include <utility>

namespace iter {

template <class Buffer>
template <class ...Types>
DoubleBuffer <Buffer>::DoubleBuffer(Types&& ...args) {
    active_index_ = 0;
    is_buffer_ready_ = false;
    buffer_ptr_[0] = std::shared_ptr <Buffer> (
        new Buffer(std::forward <Types> (args)...));
    buffer_ptr_[1] = std::shared_ptr <Buffer> (
        new Buffer(std::forward <Types> (args)...));
}

template <class Buffer>
bool DoubleBuffer <Buffer>::GetBuffer(std::shared_ptr <Buffer>* ptr) {
    if (!is_buffer_ready_) return false;
    *ptr = buffer_ptr_[active_index_];
    return true;
}

template <class Buffer>
bool DoubleBuffer <Buffer>::GetNextBuffer(std::shared_ptr <Buffer>* ptr) {
    int next_active_index = active_index_ ^ 1;
    if (!buffer_ptr_[next_active_index].unique()) return false;
    *ptr = buffer_ptr_[next_active_index];
    return true;
}

template <class Buffer>
void DoubleBuffer <Buffer>::SwapBuffer() {
    std::lock_guard <std::mutex> load_lock(mutex);
    active_index_ ^= 1;
    is_buffer_ready_ = true;
}

template <class Buffer>
bool DoubleBuffer <Buffer>::UpdateAndSwapBuffer(
        const std::shared_ptr <Buffer>& ptr) {
    int next_active_index = active_index_ ^ 1;
    if (!buffer_ptr_[next_active_index].unique()) return false;
    std::lock_guard <std::mutex> load_lock(mutex);
    buffer_ptr_[next_active_index] = ptr;
    active_index_ ^= 1;
    is_buffer_ready_ = true;
    return true;
}

} // namespace iter

#endif // ITER_DOUBLE_BUFFER_IMPL_HPP
