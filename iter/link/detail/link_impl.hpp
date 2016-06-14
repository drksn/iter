#ifndef ITER_LINK_IMPL_HPP
#define ITER_LINK_IMPL_HPP

#include <memory>
#include <utility>
#include <type_traits>

namespace iter {

template <class First, class ...Functor>
template <class ...Types>
Link <First, Functor...>::Link(Types&& ...args) {
    first_ptr_ = std::shared_ptr <First> (new First(args...));
    second_ptr_ = std::shared_ptr <Second> (new Second(args...));
}

template <class First, class ...Functor>
template <class FirstInit, class ...Types>
Link <First, Functor...>::Link(
        FirstInit&& first_init, Types&& ...args) {
    first_ptr_ = std::shared_ptr <First> (
        new First(std::forward <FirstInit> (first_init)));
    second_ptr_ = std::shared_ptr <Second> (
        new Second(std::forward <Types> (args)...));
}

template <class First, class ...Functor>
template <class Source, class Target>
bool Link <First, Functor...>::operator () (
        Source&& source, Target&& target) {
    typename std::remove_reference <typename First::second_argument_type>::type mid;
    bool first_ret = (*first_ptr_)(
        std::forward <Source> (source), mid);
    if (!first_ret) return false;
    bool second_ret = (*second_ptr_)(
        std::move(mid), target);
    return second_ret;
}

template <class First>
template <class ...Types>
Link <First>::Link(Types&& ...args) {
     first_ptr_ = std::shared_ptr <First>(
        new First(std::forward <Types> (args)...));
}

template <class First>
template <class Source, class Target>
bool Link <First>::operator () (Source&& source, Target&& target) {
     return (*first_ptr_)(source, target);
}

} // namespace iter

#endif // ITER_LINK_IMPL_HPP
