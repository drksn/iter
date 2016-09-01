#ifndef ITER_REGISTRY_HPP
#define ITER_REGISTRY_HPP

#include <unordered_map>
#include <mutex>
#include <type_traits>

namespace iter {

template <class Node, class Handle = int,
        class Map = std::unordered_map <Handle, Node>>
class Registry {
public:
    static_assert(std::is_integral <Handle>::value,
        "Type template parameter 'Handle' must be integral.");

    // Return the handle of this node.
    Handle Register(const Node& node);
    Handle Register(Node&& node);
    void Remove(Handle handle);

    // Check whether it is registered.
    bool IsRegistered(Handle handle);

    // Get the node corresponding to the handle.
    Node Get(Handle handle);

protected:
    Map register_map_;
    Handle register_handle_counter_;
    std::mutex mtx_;
};

template <class Node, class Handle, class Map>
Handle Registry <Node, Handle, Map>::Register(const Node& node) {
    std::lock_guard <std::mutex> lck(mtx_);
    register_handle_counter_ ++;
    register_map_.emplace(register_handle_counter_, node);
    return register_handle_counter_;
}

template <class Node, class Handle, class Map>
Handle Registry <Node, Handle, Map>::Register(Node&& node) {
    std::lock_guard <std::mutex> lck(mtx_);
    register_handle_counter_ ++;
    register_map_.emplace(register_handle_counter_, std::move(node));
    return register_handle_counter_;
}

template <class Node, class Handle, class Map>
void Registry <Node, Handle, Map>::Remove(Handle handle) {
    std::lock_guard <std::mutex> lck(mtx_);
    register_map_.erase(handle);
}

template <class Node, class Handle, class Map>
bool Registry <Node, Handle, Map>::IsRegistered(Handle handle) {
    return register_map_.find(handle) != register_map_.end();
}

template <class Node, class Handle, class Map>
Node Registry <Node, Handle, Map>::Get(Handle handle) {
    return register_map_.at(handle);
}

} // namespace iter

#endif // ITER_REGISTRY_HPP
