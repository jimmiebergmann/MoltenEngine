#include "gtest/gtest.h"
#include "Curse/Math/Vector.hpp"
#include "Curse/System/Semaphore.hpp"

TEST(Math, Vector)
{

}

TEST(System, Semaphore)
{
    Semaphore sem;
}


int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}