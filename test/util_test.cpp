#include <iter/split.hpp>
#include <iter/time_keeper.hpp>
#include <iter/double_buffer.hpp>
#include <iter/kvstr.hpp>
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

TEST(UtilTest, DoubleBuffer) {
    DoubleBuffer <std::string> db;
    std::string a = "girigiri", b = "bilibili";

    bool ret1 = db.Update(a);
    EXPECT_TRUE(ret1);

    auto ap = db.Get();
    EXPECT_EQ(*ap, a);

    std::unique_ptr <std::string> up(new std::string(b));
    bool ret2 = db.Update(std::move(up));
    EXPECT_TRUE(ret2);

    auto bp = db.Get();
    EXPECT_EQ(*bp, b);

    auto ptr = db.GetReservedBuffer();
    *ptr = a + b;
    bool ret3 = db.Update();
    EXPECT_FALSE(ret3);

    ap.reset();
    bool ret4 = db.Update();
    EXPECT_TRUE(ret4);

    auto abp = db.Get();
    EXPECT_EQ(*abp, *ptr);
}

TEST(UtilTest, KvStr) {
    std::map <std::string, int> mp;
    mp["a"] = 1;
    mp["b"] = 2;
    auto p = std::make_pair("key", "value");
    std::string ret1 = KVSTR(mp, p);
    EXPECT_EQ(ret1, "a=1||b=2||key=value");

    std::vector <std::pair <std::string, std::string>> vec;
    for (int i = 0; i < 3; i++) vec.emplace_back(p);
    std::string ret2 = KVSTR(vec);
    EXPECT_EQ(ret2, "key=value||key=value||key=value");
}
