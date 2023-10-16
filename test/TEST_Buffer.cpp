//
// Created by dying on 10/10/23.
//

#include <gtest/gtest.h>
#include <Buffer.h>
#include <string>


TEST(Buffer , read)
{
    std::string buf;
    const int LENGTH = 1024;
    for(int i = 0; i < LENGTH; i++){
        buf += i % 10 + '0';
    }
    int fd[2];
    int ret = pipe(fd);
    int len = write(fd[1] , buf.c_str() , buf.size());
    EXPECT_EQ(len , LENGTH);

    dying::Buffer buffer;
    int Errno = 0;
    auto s_ret = buffer.readFd(fd[0] , &Errno);
    EXPECT_EQ(s_ret  , LENGTH);
    EXPECT_EQ(buffer.readableBytes() , LENGTH);

    std::string peek(buffer.peek() , LENGTH);
    EXPECT_EQ(peek , buf);
    EXPECT_EQ(LENGTH , buffer.readableBytes());
    std::string readAll = buffer.retrieveAllToStr();
    EXPECT_EQ(readAll , buf);
    EXPECT_EQ(0 , buffer.readableBytes());
}

GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc , argv);
    return RUN_ALL_TESTS();
}