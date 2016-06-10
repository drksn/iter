#include <iter/link.hpp>
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

class AddStar {
public:
    typedef std::string FirstArg;
    typedef std::string SecondArg;

    AddStar(const std::string& star = "*"):star_(star){}

    bool operator () (const std::string& a, std::string& b) {
        b = a + star_;
        return true;
    }

private:
    std::string star_;
};

TEST(LinkTest, SimpleTest) {
    Link <AddStar> a1;
    Link <AddStar, AddStar> a2;
    Link <AddStar, AddStar, AddStar> a3;
    std::string star = "CountingStar:", s1, s2, s3;
    a1(star, s1);
    a2(star, s2);
    a3(star, s3);
    EXPECT_EQ(s1, "CountingStar:*");
    EXPECT_EQ(s2, "CountingStar:**");
    EXPECT_EQ(s3, "CountingStar:***");
}

TEST(LinkTest, InitTest) {
    Link <AddStar> a1("@");
    Link <AddStar, AddStar> a2("#", "$");
    Link <AddStar, AddStar, AddStar> a3("%", "^", "&");
    std::string star = "CountingStar:", s1, s2, s3;
    a1(star, s1);
    a2(star, s2);
    a3(star, s3);
    EXPECT_EQ(s1, "CountingStar:@");
    EXPECT_EQ(s2, "CountingStar:#$");
    EXPECT_EQ(s3, "CountingStar:%^&");
}

TEST(LinkTest, NestTest) {
    Link <Link <AddStar>, Link <AddStar, AddStar> > a1("@", "#", "$");
    Link <Link <AddStar, AddStar>, Link <AddStar, AddStar> > a2("@", "#", "$");
    std::string star = "CountingStar:", s1, s2;
    a1(star, s1);
    a2(star, s2);
    EXPECT_EQ(s1, "CountingStar:@#$");
    EXPECT_EQ(s2, "CountingStar:@*#$");
}
