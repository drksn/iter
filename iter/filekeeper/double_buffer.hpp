#ifndef ITER_DOUBLE_BUFFER_HPP
#define ITER_DOUBLE_BUFFER_HPP

#include <memory>
#include <mutex>

namespace iter {

template <class Buffer>
class DoubleBuffer{
public:
    template <class ...Types> DoubleBuffer(Types&& ...args);
    // Get the active buffer, if double buffer is not empty, return true.
    bool GetBuffer(std::shared_ptr <Buffer>* ptr);
    // Get the inactive buffer, if its shared_ptr is unique, return true.
    bool GetNextBuffer(std::shared_ptr <Buffer>* ptr);
    // Swap the active and inactive buffer.
    void SwapBuffer();
    // If the shared_ptr of inactive buffer is unique,
    // update the inactive and swap. Otherwise, return false.
    bool UpdateAndSwapBuffer(const std::shared_ptr <Buffer>& ptr);

private:
    // The index of active buffer.
    int active_index_;
    // Indicate whether the buffer is ready.
    bool is_buffer_ready_;
    // The pointer of the two buffer.
    std::shared_ptr <Buffer> buffer_ptr_[2];
    // Mutex lock, guard the atomicity of the Load() method
    std::mutex mutex;
};

} // namespace iter

#include <iter/filekeeper/detail/double_buffer_impl.hpp>

#endif // ITER_DOUBLE_BUFFER_HPP
