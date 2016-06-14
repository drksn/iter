#include <iter/util/kvstr.hpp>
#include <iter/util/fmtstr.hpp>
#include <iter/log.hpp>
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

const int TOTAL = 14, SLEEP_TIME = 5e5;

void gao(){
     for (int i = 0; i < TOTAL; i ++) {
        ITER_DEBUG_KV(MSG("Write file test."), KV(i));
        usleep(SLEEP_TIME);
    }
}

TEST(LogTest, Kv) {
    double value = 2.0 / 7;
    ITER_DEBUG_KV("DLTAG", MSG("Call 110."), KV("value", value));
    ITER_INFO_KV("DLTAG", MSG("Call 110."), KV(value));
    ITER_WARN_KV(MSG("Call 110."), KV(value));
    ITER_ERROR_KV(MSG("Call 110."), KV(value));
    ITER_FATAL_KV(MSG("Call 110."), KV(value));
}

TEST(LogTest, Fmt) {
    ITER_DEBUG_FMT("Test: #%d: %s", 0, "girigiri eye.");
    ITER_INFO_FMT("Test: #%d: %s", 1, "girigiri eye.");
    ITER_WARN_FMT("Test: #%d: %s", 2, "girigiri eye.");
    ITER_ERROR_FMT("Test: #%d: %s", 3, "girigiri eye.");
    ITER_FATAL_FMT("Test: #%d: %s", 4, "girigiri eye.");
}

TEST(LogTest, WriteFile){
    ITER_LOG_INIT(".");
    const int THREAD_NUM = 10;
    std::shared_ptr <std::thread> thread_ptr_list[100];
    for (int i = 0; i < THREAD_NUM; i++)
        thread_ptr_list[i] = std::shared_ptr <std::thread> (new std::thread(gao));

    for (int i = 0; i < TOTAL; i ++) {
        usleep(SLEEP_TIME);
        fprintf(stderr, ".");
        fflush(stderr);
    }
    fprintf(stderr, "\n");

    for (int i=0; i < THREAD_NUM; i++)
        thread_ptr_list[i]->join();
}
