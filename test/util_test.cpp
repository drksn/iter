#include <iter/util.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <memory>
#include <map>

using namespace iter;

TEST(UtilTest, Split) {
    std::string tmp = ",1,12,,,1234,";
    std::vector <std::string> ret = Split(tmp, ",");
    EXPECT_EQ(ret.size(), 7);
    EXPECT_EQ(ret[0], "");
    EXPECT_EQ(ret[1], "1");
    EXPECT_EQ(ret[2], "12");
    EXPECT_EQ(ret[3], "");
    EXPECT_EQ(ret[4], "");
    EXPECT_EQ(ret[5], "1234");
    EXPECT_EQ(ret[6], "");

    std::vector <std::string> ret2 = Split(tmp, ",,");
    EXPECT_EQ(ret2.size(), 2);
    EXPECT_EQ(ret2[0], ",1,12");
    EXPECT_EQ(ret2[1], ",1234,");
}

inline bool Equal (double a, double b, double eps) {
     return fabs(a - b) <= eps;
}

TEST(UtilTest, TimeKeeper) {
    TimeKeeper tk;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int ms_i1 = tk.GetElapsedTime();
    EXPECT_TRUE(Equal(ms_i1, 100, 20));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int ms_i2 = tk.GetElapsedTime();
    EXPECT_TRUE(Equal(ms_i2, 200, 20));

    tk.Reset(); // Reset time keeper.

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    double s_i1 = tk.GetElapsedTime <double, std::ratio <1, 1>>();
    EXPECT_TRUE(Equal(s_i1, 0.1, 0.02));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    double s_i2 = tk.GetElapsedTime <double, std::ratio <1, 1>>();
    EXPECT_TRUE(Equal(s_i2, 0.2, 0.02));
}

TEST(UtilTest, UniqueQueue) {
    UniqueQueue <int> uq;
    uq.Push(1);
    uq.Push(2);
    auto t1 = uq.Get();

    EXPECT_EQ(t1->front(), 1);
    t1->pop();
    EXPECT_EQ(t1->front(), 2);
    t1->pop();
    EXPECT_TRUE(t1->empty());

    uq.Push(3);
    auto t2 = uq.Get();
    EXPECT_EQ(t2->front(), 3);
    t2->pop();
    EXPECT_TRUE(t2->empty());
}

