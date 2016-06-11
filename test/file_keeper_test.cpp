#include <iter/file_io.hpp>
#include <iter/file_keeper.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <memory>
#include <map>

using namespace iter;

class FileKeeperTest : public testing::Test{
public:
    virtual void SetUp() {
        content = "girigiri eye~\ngirigiri mind~\n";
        filename = "girigiri.test";
        bool write_ret = FileWriter()(content, filename);
        EXPECT_TRUE(write_ret);
        file_keeper_ptr = std::shared_ptr <FileKeeper <FileReader> >(
            new FileKeeper <FileReader>(filename));
    }

    std::string content;
    std::string filename;
    std::shared_ptr <FileKeeper <FileReader> > file_keeper_ptr;
};

TEST_F(FileKeeperTest, SimpleTest) {
    std::shared_ptr <std::string> ptr;
    bool get_ret = file_keeper_ptr->GetBuffer(&ptr);
    EXPECT_TRUE(get_ret);
    EXPECT_EQ(*ptr, content);
}


TEST_F(FileKeeperTest, ModifyTest) {
    std::string newtext = "wowuwo~ wowowo~";
    usleep(1e5);
    bool write_ret = FileWriter()(newtext, filename);
    EXPECT_TRUE(write_ret);
    usleep(1e5);
    std::shared_ptr <std::string> ptr;
    bool get_ret = file_keeper_ptr->GetBuffer(&ptr);
    EXPECT_TRUE(get_ret);
    EXPECT_EQ(*ptr, newtext);
    usleep(1e5);
}

