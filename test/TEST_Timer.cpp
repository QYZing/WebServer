//
// Created by dying on 10/16/23.
//

#include <gtest/gtest.h>
#include <unistd.h>
#include <chrono>
#include <Timer.h>

TEST(Timer , TIMEWHEEL)
{
    int share = 0;
    auto timer = std::make_shared<dying::Timer>(dying::Timer::TIME_WHEEL);
    for(int i = 0; i < 10; i++){
        timer->add(i , 1000 * (i + 1), [i,&share](){
            share++;
            std::cout<<"tick now time = "<< time(0) <<std::endl;
        });
    }
    for(int i = 0; i < 20; i++){
        timer->tick();
        if( i < 10){
            EXPECT_EQ(i , share);
        }else{
            EXPECT_EQ(10 , share);
        }
        sleep(1);
    }
}

TEST(Timer , HEAPTIMER)
{
    int share = 0;
    auto timer = std::make_shared<dying::Timer>(dying::Timer::HEAP_TIMER);
    for(int i = 0; i < 10; i++){
        timer->add(i , 1000 * (i + 1), [i,&share](){
            share++;
            std::cout<<"tick now time = "<< time(0) <<std::endl;
        });
    }
    for(int i = 0; i < 20; i++){
        timer->tick();
        if( i < 10){
            EXPECT_EQ(i , share);
        }else{
            EXPECT_EQ(10 , share);
        }
        sleep(1);
    }
}
GTEST_API_ int main(int argc, char **argv)
{
    dying::SingletonLoggerManager::getInstance().close();
    testing::InitGoogleTest(&argc , argv);
    return RUN_ALL_TESTS();
}