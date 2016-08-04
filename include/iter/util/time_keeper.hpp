#ifndef ITER_TIME_KEEPER_HPP
#define ITER_TIME_KEEPER_HPP

#include <chrono>

namespace iter {

class TimeKeeper {
public:
    TimeKeeper();
    void Reset();
    template <class Rep = int,
        class Period = std::milli> Rep GetElapsedTime();

private:
    std::chrono::high_resolution_clock::time_point begin_;
};

TimeKeeper::TimeKeeper() {
    Reset();
}

void TimeKeeper::Reset() {
    begin_ = std::chrono::high_resolution_clock::now();
}

template <class Rep, class Period> Rep TimeKeeper::GetElapsedTime() {
    using namespace std::chrono;
    high_resolution_clock::time_point now = high_resolution_clock::now();
    return duration_cast <duration <Rep, Period>>(now - begin_).count();
}

} // namespace iter

#endif // ITER_TIME_KEEPER_HPP
