#include <iter/safe_queue.hpp>
#include <iter/thread_pool.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <memory>

using namespace iter;

TEST(SafeTest, SafeQueue) {
    std::unique_ptr <ThreadPool> pool(new ThreadPool(4));
    SafeQueue <int> safe_queue;

    const int PUB = 10, SUB = 10, NUM = 10000;

    for (int pub = 0; pub < PUB; pub ++) {
        pool->PushTask([&safe_queue] {
            for (int i = 0; i < NUM; i ++) {
                safe_queue.Push(i);
                std::this_thread::sleep_for(
                    std::chrono::microseconds(10));
            }
        });
    }

    int count[NUM] = {};
    std::mutex mtx;
    for (int sub = 0; sub < SUB; sub ++) {
        pool->PushTask([&safe_queue, &mtx, &count, sub]() {
            std::queue <int> queue;
            for (int i = 0; i < PUB * NUM / SUB; i ++) {
                int ret = 0;
                // Two different way to get element from safe queue.
                if (sub & 1) {
                    safe_queue.Get(&ret);
                }
                else {
                    std::lock_guard <std::mutex> lck(mtx);
                    safe_queue.Wait();
                    bool succ = safe_queue.Pop(&ret);
                    EXPECT_TRUE(succ);
                }
                queue.push(ret);
                std::this_thread::sleep_for(
                    std::chrono::microseconds(10));
            }

            std::lock_guard <std::mutex> lck(mtx);
            while (!queue.empty()) {
                count[queue.front()] ++;
                queue.pop();
            }
        });
    }

    // Wait for all task finished.
    pool.reset();

    for (int i = 0; i < NUM; i ++) {
        EXPECT_EQ(count[i], PUB);
    }
}

