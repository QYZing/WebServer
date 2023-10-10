//
// Created by dying on 10/9/23.
//

#include <Timer.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <cstring>

dying::Timer timer(dying::Timer::TIME_WHEEL);

void  tick(int num){
    timer.tick();
    alarm(1);
}

void test1(){
    std::function<void()> cycle = [&](){
        std::cout<<"hello Timer one"<<std::endl;
        try {
            timer.add(1, 1000, cycle);
        }catch (...){
            std::cout<<"add error"<<std::endl;
        }
    };
    std::function<void()> cycle2 = [&](){
        std::cout<<"hello Timer two"<<std::endl;
        try {
            timer.add(2, 2000, cycle2);
        }catch (...){
            std::cout<<"add error"<<std::endl;
        }
    };
    timer.add(2 , 2000  , cycle2);
    timer.add(1 , 1000  , cycle);
}

void test2()
{

    std::function<void()> cycle = [&](){
        std::cout<<"hello Timer one"<<std::endl;
        try {
            timer.add(1, 1000, cycle);
        }catch (...){
            std::cout<<"add error"<<std::endl;
        }
    };
    std::function<void()> cycle2 = [&](){
        std::cout<<"hello Timer two"<<std::endl;
        try {
            timer.add(2, 2000, cycle2);
        }catch (...){
            std::cout<<"add error"<<std::endl;
        }
    };
    timer.add(2 , 2000  , cycle2);
    timer.add(1 , 1000  , cycle);
    timer.adjust(3 , 2000);
    timer.adjust(2 , 1000);
}

int main()
{
    test2();
    struct sigaction s;
    s.sa_flags = 0;
    memset(&s.sa_mask , 0 , sizeof(s.sa_mask));
    s.sa_handler = tick;
    sigaction(SIGALRM , &s , NULL);
    alarm(1);

    while(1);
}

