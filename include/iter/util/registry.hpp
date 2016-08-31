#ifndef ITER_REGISTRY_HPP
#define ITER_REGISTRY_HPP

#include <unordered_map>
#include <mutex>
#include <utility>

namespace iter {

template <class Node, class Map = std::unordered_map <int, Node>>
class Registry {
public:
    int Register(const Node& node);
    int Register(Node&& node);
    void Remove(int register_handle);

    // Check whether it is registered.
    bool IsRegistered(int register_handle);

    // Get the node correspounding to the register id.
    Node Get(int register_handle);

private:
    Map register_map_;
    int register_handle_nxt_;
    std::mutex mtx_;
};

template <class Node, class Map>
int Registry <Node, Map>::Register(const Node& node) {
    std::lock_guard <std::mutex> lck(mtx_);
    register_handle_nxt_ ++;
    register_map_.emplace(register_handle_nxt_, node);
    return register_handle_nxt_;
}

template <class Node, class Map>
int Registry <Node, Map>::Register(Node&& node) {
    std::lock_guard <std::mutex> lck(mtx_);
    register_handle_nxt_ ++;
    register_map_.emplace(register_handle_nxt_, std::move(node));
    return register_handle_nxt_;
}

template <class Node, class Map>
void Registry <Node, Map>::Remove(int register_handle) {
    std::lock_guard <std::mutex> lck(mtx_);
    register_map_.erase(register_handle);
}

template <class Node, class Map>
bool Registry <Node, Map>::IsRegistered(int register_handle) {
    return register_map_.find(register_handle) != register_map_.end();
}

template <class Node, class Map>
Node Registry <Node, Map>::Get(int register_handle) {
    return register_map_.at(register_handle);
}

} // namespace iter

#endif // ITER_REGISTRY_HPP
