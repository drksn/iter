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

    // If the reserved buffer is released by all users, return true;
    bool IsReleased();

    // Get the const shared pointer of the active buffer.
    std::shared_ptr <typename std::add_const<Buffer>::type> Get();

    // NOTICE, you can directly operate the reserved buffer, take care.
    Buffer* GetReservedBuffer();

    // Update double buffer,
    // If the reserved buffer is not released by all users, return false.
    // None-param means using reserved buffer.
    bool Update();
    bool Update(const Buffer& buffer);
    bool Update(Buffer&& buffer);
    bool Update(std::unique_ptr <Buffer>&& buffer_ptr);

    // Disable copy constructor and copy assignment operator.
    DoubleBuffer(const DoubleBuffer&) = delete;
    DoubleBuffer& operator = (const DoubleBuffer&) = delete;
    // Enable move copy constructor and move copy assignment operator.
    DoubleBuffer(DoubleBuffer&&) = default;
    DoubleBuffer& operator = (DoubleBuffer&&) = default;

private:
    // The index of active buffer.
    int active_idx_;
    // The shared pointer of the two buffer.
    std::shared_ptr <Buffer> buffer_ptr_[2];
    std::mutex mtx_;
};

template <class Buffer>
DoubleBuffer <Buffer>::DoubleBuffer() : active_idx_(0) {
    buffer_ptr_[0] = std::make_shared <Buffer>();
    buffer_ptr_[1] = std::make_shared <Buffer>();
}

template <class Buffer>
bool DoubleBuffer <Buffer>::IsReleased() {
    return buffer_ptr_[active_idx_ ^ 1].unique();
}

template <class Buffer>
std::shared_ptr <typename std::add_const <Buffer>::type> DoubleBuffer <Buffer>::Get() {
    return buffer_ptr_[active_idx_];
}

template <class Buffer>
Buffer* DoubleBuffer <Buffer>::GetReservedBuffer() {
    return buffer_ptr_[active_idx_ ^ 1].get();
}

template <class Buffer>
bool DoubleBuffer <Buffer>::Update() {
    std::lock_guard <std::mutex> lck(mtx_);
    if (!IsReleased()) return false;
    active_idx_ ^= 1;
    return true;
}

template <class Buffer>
bool DoubleBuffer <Buffer>::Update(const Buffer& buffer) {
    std::lock_guard <std::mutex> lck(mtx_);
    if (!IsReleased()) return false;
    *buffer_ptr_[active_idx_ ^ 1] = buffer;
    active_idx_ ^= 1;
    return true;
}

template <class Buffer>
bool DoubleBuffer <Buffer>::Update(Buffer&& buffer) {
    std::lock_guard <std::mutex> lck(mtx_);
    if (!IsReleased()) return false;
    *buffer_ptr_[active_idx_ ^ 1] = std::move(buffer);
    active_idx_ ^= 1;
    return true;
}

template <class Buffer>
bool DoubleBuffer <Buffer>::Update(std::unique_ptr <Buffer>&& buffer_ptr) {
    std::lock_guard <std::mutex> lck(mtx_);
    if (!IsReleased()) return false;
    buffer_ptr_[active_idx_ ^ 1] = std::move(buffer_ptr);
    active_idx_ ^= 1;
    return true;
}

} // namespace iter

#endif // ITER_DOUBLE_BUFFER_HPP
