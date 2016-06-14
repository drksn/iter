#include <iter/util/kvstr.hpp>
#include <iter/util/fmtstr.hpp>
#include <iter/util/split.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <cstdio>
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
