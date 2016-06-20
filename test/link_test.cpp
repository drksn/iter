#include <iter/link.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <typeinfo>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <memory>
#include <map>
#include <functional>

using namespace iter;

class AddStar {
public:
    typedef const std::string& first_argument_type;
    typedef std::string& second_argument_type;

    AddStar(const std::string& star = "*"):star_(star){}

    bool operator () (const std::string& a, std::string& b) {
        b = a + star_;
        return true;
    }

private:
    std::string star_;
};

bool addone(std::string& a, std::string& b) {
     b = a + "1";
     return true;
}

typedef std::function <bool (std::string&, std::string&)> BSS;

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

TEST(LinkTest, FunctionTest) {
    Link <AddStar, BSS> a4("@", addone);
    Link <BSS, AddStar> a5(addone);
    Link <BSS, BSS, AddStar> a6(addone, addone);
    std::string star = "CountingStar:", s4, s5, s6;
    a4(star, s4);
    a5(star, s5);
    a6(star, s6);
    EXPECT_EQ(s4, "CountingStar:@1");
    EXPECT_EQ(s5, "CountingStar:1*");
    EXPECT_EQ(s6, "CountingStar:11*");

    Link <BSS> func(addone);
    std::string sf;
    func(star, sf);
    EXPECT_EQ(sf, "CountingStar:1");
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
