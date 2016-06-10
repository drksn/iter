#ifndef ITER_TIME_KEEPER_HPP
#define ITER_TIME_KEEPER_HPP

#include <sys/time.h>
#include <string>

namespace iter {

class TimeKeeper {
public:
    TimeKeeper(){
        Restart();
    }
    void Restart(){
        gettimeofday(&tv_begin_, NULL);
        tv_last_ = tv_begin_;
    }

    long GetInterval(){
        struct timeval tv_end;
        gettimeofday(&tv_end, NULL);
        long interval_time = (tv_end.tv_sec - tv_last_.tv_sec) * 1000
            + (tv_end.tv_usec - tv_last_.tv_usec) / 1000;
        tv_last_ = tv_end;
        return interval_time;
    }

    long GetTotal(){
        struct timeval tv_end;
        gettimeofday(&tv_end, NULL);
        long total_time = (tv_end.tv_sec - tv_begin_.tv_sec) * 1000
            + (tv_end.tv_usec - tv_begin_.tv_usec) / 1000;
        return total_time;
    }

private:
    struct timeval tv_begin_, tv_last_;
};

} // namespace iter

#endif // ITER_TIME_KEEPER_HPP
