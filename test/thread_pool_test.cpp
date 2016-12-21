#include <iter/thread_pool.hpp>
#include <iter/time_keeper.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <memory>
#include <future>

using namespace iter;

TEST(SpeedTest, ThreadPool) {
    ThreadPool thread_pool(3);

    const int TASK_NUM = 2000;
    const int CALC_NUM = 2e8;
    std::future <long long> handle_list[TASK_NUM];
    auto func = [](int tot) {
        long long ret = 0;
        for (int i = 0; i < tot; i ++) ret += i;
        return ret;
    };

    TimeKeeper tk;
    long long tot = CALC_NUM / TASK_NUM;
    for (int i = 0; i < TASK_NUM; i ++) {
        handle_list[i] = thread_pool.PushTask(func, tot);
    }
    std::cout << "Push " << TASK_NUM << " task elapsed time "
        << tk.GetElapsedTime <double> () << " ms" << std::endl;

    TimeKeeper wait_tk;
    for (int i = 0; i < TASK_NUM; i ++) handle_list[i].wait();
    std::cout << "Wait elapsed time "
        << wait_tk.GetElapsedTime <double> () << " ms" << std::endl;

    std::cout << "Total elapsed time "
        << tk.GetElapsedTime <double> () << " ms" << std::endl;

    TimeKeeper one_tk;
    int ret = 0;
    for (int i = 0; i < CALC_NUM; i ++) ret += i;
    std::cout << "Same task with thread pool elapsed time "
        << one_tk.GetElapsedTime <double> () << " ms" << std::endl;

    for (int i = 0; i < TASK_NUM; i ++) {
        EXPECT_EQ(handle_list[i].get(), tot * (tot - 1) / 2);
    }
}

