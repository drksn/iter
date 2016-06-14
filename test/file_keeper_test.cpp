//#define ITER_LOG_DISABLE
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
}

void MoveWrite(const std::string& content, const std::string& filename) {
    std::string filename_tmp = filename + ".tmp";
    bool write_ret = FileWriter()(content, filename_tmp);
    EXPECT_TRUE(write_ret);
    std::string cmd = "mv " + filename_tmp + " " + filename;
    system(cmd.c_str());
}

TEST_F(FileKeeperTest, MoveCoverTest) {
    std::string text_test = "GIRIGIRI_EYE\n";
    MoveWrite(text_test, filename);
    usleep(1e5);
    std::shared_ptr <std::string> ptr;
    bool get_ret = file_keeper_ptr->GetBuffer(&ptr);
    EXPECT_TRUE(get_ret);
    EXPECT_EQ(text_test, *ptr);

    text_test = "GIRIGIRI_MINE\n";
    MoveWrite(text_test, filename);
    usleep(1e5);
    get_ret = file_keeper_ptr->GetBuffer(&ptr);
    EXPECT_TRUE(get_ret);
    EXPECT_EQ(text_test, *ptr);
}

TEST_F(FileKeeperTest, MoveSelfTest) {
    std::string cmd = "mv " + filename + " " + filename + ".tmp2";
    system(cmd.c_str());
    std::string text_test = "BILIBILI_EYE\n";
    bool write_ret = FileWriter()(text_test, filename+".tmp2");
    EXPECT_TRUE(write_ret);

    cmd = "mv " + filename + ".tmp2 " + filename;
    system(cmd.c_str());
    usleep(1e5);

    std::shared_ptr <std::string> ptr;
    bool get_ret = file_keeper_ptr->GetBuffer(&ptr);
    EXPECT_TRUE(get_ret);
    EXPECT_EQ(text_test, *ptr);
}

TEST_F(FileKeeperTest, DeleteTest) {
    std::string filename_test = "bilibili.test";
    std::string text_test = "GIriGIri\n";
    FileKeeper <FileReader> fk(filename_test);
    FileWriter()(text_test, filename_test);
    usleep(1e5);
    std::shared_ptr <std::string> ptr;
    bool get_ret = fk.GetBuffer(&ptr);
    EXPECT_TRUE(get_ret);
    EXPECT_EQ(text_test, *ptr);

    std::string cmd = "rm -rf " + filename_test;
    system(cmd.c_str());
    FileWriter()(text_test, filename_test);
    usleep(1e5);
    get_ret = fk.GetBuffer(&ptr);
    EXPECT_TRUE(get_ret);
    EXPECT_EQ(text_test, *ptr);
}

