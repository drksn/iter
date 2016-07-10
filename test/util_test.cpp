#include <iter/util/kvstr.hpp>
#include <iter/util/fmtstr.hpp>
#include <iter/util/split.hpp>
#include <iter/util/time_keeper.hpp>
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
    int ms_i1 = tk.GetInterval();
    int ms_t1 = tk.GetTotal();
    EXPECT_TRUE(Equal(ms_i1, 100, 20));
    EXPECT_TRUE(Equal(ms_t1, 100, 20));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int ms_i2 = tk.GetInterval();
    int ms_t2 = tk.GetTotal();
    EXPECT_TRUE(Equal(ms_i2, 100, 20));
    EXPECT_TRUE(Equal(ms_t2, 200, 20));

    tk.Reset(); // Reset time keeper.

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    double s_i1 = tk.GetInterval <double, std::ratio <1, 1>>();
    double s_t1 = tk.GetTotal <double, std::ratio <1, 1>>();
    EXPECT_TRUE(Equal(s_i1, 0.1, 0.02));
    EXPECT_TRUE(Equal(s_t1, 0.1, 0.02));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    double s_i2 = tk.GetInterval <double, std::ratio <1, 1>>();
    double s_t2 = tk.GetTotal <double, std::ratio <1, 1>>();
    EXPECT_TRUE(Equal(s_i2, 0.1, 0.02));
    EXPECT_TRUE(Equal(s_t2, 0.2, 0.02));
}
