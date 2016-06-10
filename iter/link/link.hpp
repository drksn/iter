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
    typedef typename First::FirstArg FirstArg;
    typedef typename Second::SecondArg SecondArg;
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
    typedef typename First::FirstArg FirstArg;
    typedef typename First::SecondArg SecondArg;

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
