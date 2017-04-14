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
    std::unique_ptr<ThreadPool> pool(new ThreadPool(4));
    SafeQueue<int> safe_queue;

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
                    std::lock_guard<std::mutex> lck(mtx);
                    safe_queue.Wait();
                    bool succ = safe_queue.Pop(&ret);
                    EXPECT_TRUE(succ);
                }
                queue.push(ret);
                std::this_thread::sleep_for(
                    std::chrono::microseconds(10));
            }

            std::lock_guard<std::mutex> lck(mtx);
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

TEST(PopAllTest, SafeQueue) {
    std::unique_ptr<ThreadPool> pool(new ThreadPool(4));
    SafeQueue<int> safe_queue;

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
                auto tmp_queue = safe_queue.PopAll();
                while (!tmp_queue->empty()) {
                    queue.push(tmp_queue->front());
                    tmp_queue->pop();
                }
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

TEST(FrontTest, SafeQueue) {
    SafeQueue<int> int_queue;
    int_queue.Push(100);
    uint64_t int_r = -1;
    bool ret1 = int_queue.Front(&int_r);
    EXPECT_TRUE(ret1);
    EXPECT_EQ(int_r, 100);

    SafeQueue<std::thread> t_queue;
    t_queue.Push(std::thread());
    std::thread t;
    /*
     * NOTICE: Uncomment the following line will cause compile error,
     * because the type std::thread is not copy assignable.
     */

    // t_queue.Front(&t);
}

TEST(CopyTest, SafeQueue) {
    SafeQueue<std::unique_ptr<int>> unique_int_queue;
    std::unique_ptr<int> ptr(new int(10));
    unique_int_queue.Push(std::move(ptr));
    EXPECT_TRUE(ptr == nullptr);

    std::unique_ptr<int> ppp;
    unique_int_queue.Pop(&ppp);
    EXPECT_EQ(*ppp, 10);
}
