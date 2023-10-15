//
// Created by dying on 10/10/23.
//
#include <gtest/gtest.h>
#include <ThreadPool.h>
#include <iostream>

class Test_ThreadPool
{
public:
    Test_ThreadPool() = default;
    void operator() (){
        std::lock_guard<std::mutex> lock(mutex);
      share++;
    }

public:
    static inline  int share = 0;
    static inline std::mutex mutex = {};
};

TEST(ThreadPool , RANGE_SCHEDULE)
{
    std::shared_ptr<dying::ThreadPool<Test_ThreadPool>> test_pool;
    {
        test_pool = std::make_shared<dying::ThreadPool<Test_ThreadPool>>(10 , 20);
        for(int i = 0 ;i < 1000; i++){
            test_pool->append({});
        }
        sleep(5);
        EXPECT_EQ(10 , test_pool->getThreadNumbers());
    }
    EXPECT_EQ(1000 , Test_ThreadPool::share);
}

GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc , argv);
    return RUN_ALL_TESTS();
}