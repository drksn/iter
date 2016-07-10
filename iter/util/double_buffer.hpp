#ifndef ITER_DOUBLE_BUFFER_HPP
#define ITER_DOUBLE_BUFFER_HPP

#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

namespace iter {

// Buffer MUST have no-arguments constructor.
template <class Buffer>
class DoubleBuffer{
public:
    DoubleBuffer();
    // If double buffer is empty, return true.
    bool Empty();
    // If the inactive buffer is released by all users, return true;
    bool Released();
    // Get the const shared pointer of the active buffer,
    // if double buffer is empty, return NULL. (NOTICE)
    std::shared_ptr <typename std::add_const<Buffer>::type> Get();
    // Update double buffer,
    // if the inactive buffer is not released by all users, return false.
    bool Update(const Buffer& buffer);
    bool Update(Buffer&& buffer);
    // Disable copy constructor and copy assignment operator.
    DoubleBuffer(const DoubleBuffer&) = delete;
    DoubleBuffer& operator = (const DoubleBuffer&) = delete;
    // Enable move copy constructor and move copy assignment operator.
    DoubleBuffer(DoubleBuffer&&) = default;
    DoubleBuffer& operator = (DoubleBuffer&&) = default;

private:
    // The index of active buffer.
    int active_idx_;
    bool is_empty_;
    // The shared pointer of the two buffer.
    std::shared_ptr <Buffer> buffer_ptr_[2];
    std::mutex mtx_;
};

template <class Buffer>
DoubleBuffer <Buffer>::DoubleBuffer() {
    active_idx_ = 0;
    is_empty_ = true;
    buffer_ptr_[0] = std::make_shared <Buffer> ();
    buffer_ptr_[1] = std::make_shared <Buffer> ();
}

template <class Buffer>
bool DoubleBuffer <Buffer>::Empty() {
    return is_empty_;
}

template <class Buffer>
bool DoubleBuffer <Buffer>::Released() {
    return buffer_ptr_[active_idx_ ^ 1].unique();
}

template <class Buffer>
std::shared_ptr <typename std::add_const <Buffer>::type> DoubleBuffer <Buffer>::Get() {
    if (Empty())
        return std::shared_ptr <typename std::add_const <Buffer>::type> (); // Return NULL.
    return buffer_ptr_[active_idx_];
}

template <class Buffer>
bool DoubleBuffer <Buffer>::Update(const Buffer& buffer) {
    if (!Released()) return false;
    std::lock_guard <std::mutex> lck(mtx_);
    *buffer_ptr_[active_idx_ ^ 1] = buffer;
    active_idx_ ^= 1;
    is_empty_ = false;
    return true;
}

template <class Buffer>
bool DoubleBuffer <Buffer>::Update(Buffer&& buffer) {
    if (!Released()) return false;
    std::lock_guard <std::mutex> lck(mtx_);
    *buffer_ptr_[active_idx_ ^ 1] = std::move(buffer);
    active_idx_ ^= 1;
    is_empty_ = false;
    return true;
}

} // namespace iter

#endif // ITER_DOUBLE_BUFFER_HPP
