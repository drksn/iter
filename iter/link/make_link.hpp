#ifndef ITER_MAKE_LINK_HPP
#define ITER_MAKE_LINK_HPP

#include <iter/link/link.hpp>
#include <utility>

namespace iter {

template <class First, class ...Types>
Link <First, Types...> MakeLink (First&& f, Types&& ...args) {
    return Link <First, Types...> (
        std::forward <First> (f),
        std::forward <Types> (args)...);
}

} // namespace iter

#endif // ITER_MAKE_LINK_HPP
