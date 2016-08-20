#ifndef ITER_REGISTRY_HPP
#define ITER_REGISTRY_HPP

#include <map>
#include <mutex>
#include <utility>

namespace iter {

template <class Node> class Registry {
public:
    int Register(const Node& node);
    int Register(Node&& node);
    void Remove(int register_id);

    // Check whether it is registered.
    bool IsRegistered(int register_id);

    // Get the node correspounding to the register id.
    Node Get(int register_id);

private:
    std::map <int, Node> register_map_;
    int register_id_nxt_;
    std::mutex mtx_;
};

template <class Node>
int Registry <Node>::Register(const Node& node) {
    std::lock_guard <std::mutex> lck(mtx_);
    register_id_nxt_ ++;
    register_map_.emplace(register_id_nxt_, node);
    return register_id_nxt_;
}

template <class Node>
int Registry <Node>::Register(Node&& node) {
    std::lock_guard <std::mutex> lck(mtx_);
    register_id_nxt_ ++;
    register_map_.emplace(register_id_nxt_, std::move(node));
    return register_id_nxt_;
}

template <class Node>
void Registry <Node>::Remove(int register_id) {
    std::lock_guard <std::mutex> lck(mtx_);
    register_map_.erase(register_id);
}

template <class Node>
bool Registry <Node>::IsRegistered(int register_id) {
    return register_map_.find(register_id) != register_map_.end();
}

template <class Node>
Node Registry <Node>::Get(int register_id) {
    return register_map_.at(register_id);
}

} // namespace iter

#endif // ITER_REGISTRY_HPP
