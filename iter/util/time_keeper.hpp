#ifndef ITER_TIME_KEEPER_HPP
#define ITER_TIME_KEEPER_HPP

#include <chrono>

namespace iter {

class TimeKeeper {
public:
    TimeKeeper();
    void Reset();
    template <class Rep = int,
        class Period = std::milli> Rep GetTotal();
    template <class Rep = int,
        class Period = std::milli> Rep GetInterval();

private:
    std::chrono::high_resolution_clock::time_point begin_, last_;
};

TimeKeeper::TimeKeeper() {
    Reset();
}

void TimeKeeper::Reset() {
    begin_ = last_ = std::chrono::high_resolution_clock::now();
}

template <class Rep, class Period> Rep TimeKeeper::GetTotal() {
    using namespace std::chrono;
    high_resolution_clock::time_point now = high_resolution_clock::now();
    return duration_cast <duration <Rep, Period>>(now - begin_).count();
}

template <class Rep, class Period> Rep TimeKeeper::GetInterval() {
    using namespace std::chrono;
    high_resolution_clock::time_point now = high_resolution_clock::now();
    Rep ret = duration_cast <duration <Rep, Period>>(now - last_).count();
    last_ = now;
    return ret;
}

} // namespace iter

#endif // ITER_TIME_KEEPER_HPP
