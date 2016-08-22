#include <iter/log.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <memory>
#include <functional>
#include <map>
#include <list>
#include <tuple>
using namespace iter;

const int OUTER = 6, INNER = 100, SYS_NUM = 3, SLEEP_TIME = 100;
const int THREAD_NUM = 10;

void log_loop(const std::string& log_str){
    for (int i = 0; i < OUTER; i ++) {
        for (int j = 0; j < INNER; j++) {
            ITER_DEBUG_KV(MSG(log_str), KV(i), KV(j));
        }
        std::this_thread::yield();
    }
}

TEST(LogTest, Kv) {
    double value = 2.0 / 7;
    ITER_DEBUG_KV("DLTAG", MSG("Call 110."), KV("value", value));
    ITER_INFO_KV("DLTAG", MSG("Call 110."), KV(value));
    ITER_WARN_KV(MSG("Call 110."), KV(value));
    ITER_ERROR_KV(MSG("Call 110."), KV(value));
    ITER_FATAL_KV(MSG("Call 110."), KV(value));

    std::map <std::string, int> mp =
        {std::make_pair("map_1", 1), std::make_pair("map_2", 2)};
    std::vector <std::pair <std::string, double>> vec =
        {std::make_pair("vec_1", 3.3), std::make_pair("vec_2", 4.4)};
    std::list <std::tuple <std::string, int>> lst =
        {std::make_tuple("list_1", 5), std::make_tuple("list_2", 6)};
    ITER_INFO_KV(MSG("map, vector and list test."), mp, vec, lst);

    ITER_INFO_KV(MSG("Test one."));
}

TEST(LogTest, Fmt) {
    ITER_DEBUG("Test: #%d: %s", 0, "girigiri eye.");
    ITER_INFO("Test: #%d: %s", 1, "girigiri eye.");
    ITER_WARN("Test: #%d: %s", 2, "girigiri eye.");
    ITER_ERROR("Test: #%d: %s", 3, "girigiri eye.");
    ITER_FATAL("Test: #%d: %s", 4, "girigiri eye.");
}

TEST(LogTest, WriteFile) {
    ITER_LOG_INIT("iter.log");

    std::string text[2];
    text[0] = "Write file test";
    text[1] = "girigiri eye~";
    for (int i = 0; i < 11; i++) text[1] += text[1];

    std::shared_ptr <std::thread> thread_ptr_list[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        thread_ptr_list[i] = std::shared_ptr <std::thread> (new std::thread(
            std::bind(log_loop, text[i < THREAD_NUM / 2 ? 0 : 1])));
    }

    std::string log_names[SYS_NUM];
    for (int i = 0; i < SYS_NUM; i ++) {
        std::this_thread::yield();
        log_names[i] = (std::string)"iter" + char(i+'0') + ".log";
        std::string cmd = (std::string)"mv iter.log " + log_names[i];
        system(cmd.c_str());
        if (i&1) system("touch iter.log");
    }

    for (int i = 0; i < THREAD_NUM; i++)
        thread_ptr_list[i]->join();
}
