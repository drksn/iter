#ifndef ITER_LINK_HPP
#define ITER_LINK_HPP

#include <memory>

namespace iter {

template <class First, class ...Functor> class Link;
template <class First> class Link <First>;

template <class First, class ...Functor>
class Link {
private:
    typedef Link <Functor...> Second;

public:
    typedef bool result_type;
    typedef typename First::first_argument_type first_argument_type;
    typedef typename Second::second_argument_type second_argument_type;
    // This function can be matched only if there is no argument.
    template <class ...Types>
    Link(Types&& ...args);

    template <class FirstInit, class ...Types>
    Link(FirstInit&& first_init, Types&& ...args);

    template <class Source, class Target>
    bool operator () (Source&& source, Target&& target);

private:
    std::shared_ptr <First> first_ptr_;
    std::shared_ptr <Second> second_ptr_;
};

template <class First>
class Link <First> {
public:
    typedef bool result_type;
    typedef typename First::first_argument_type first_argument_type;
    typedef typename First::second_argument_type second_argument_type;

    template <class ...Types>
    Link(Types&& ...args);

    template <class Source, class Target>
    bool operator () (Source&& source, Target&& target);

private:
    std::shared_ptr <First> first_ptr_;
};

} // namespace iter

#include <iter/link/detail/link_impl.hpp>

#endif // ITER_LINK_HPP
