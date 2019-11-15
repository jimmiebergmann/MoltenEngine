/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#include "gtest/gtest.h"
#include "Curse/Memory/Reference.hpp"
#include <type_traits>

#include <Curse/System/Clock.hpp>
#include <memory>

namespace Curse
{
    TEST(Memory, Reference)
    {
        EXPECT_TRUE((std::is_same<Reference<int>, Ref<int> >::value));   
    }
    TEST(Memory, Reference_Create)
    {
        Ref<int> ref = Ref<int>::Create();
    }

    static void RefTest(const size_t loops)
    {
        Ref<int> ref_1 = Ref<int>::Create();
        Ref<int> ref_9;

        {
            Clock clock;

            for (size_t i = 0; i < loops; i++)
            {
                auto ref_2 = ref_1;
                {
                    auto ref_3 = ref_2;
                    {
                        auto ref_4 = ref_3;
                        {
                            auto ref_5 = ref_4;
                            {
                                auto ref_6 = ref_5;
                                {
                                    auto ref_7 = ref_6;
                                    {
                                        auto ref_8 = ref_7;
                                        {
                                            ref_9 = ref_8;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            auto time = clock.GetTime().AsSeconds<float>();
            std::cout << "Ref(" << &ref_9 << ") time:" << time << std::endl;

        }
    }

    static void StdRefTest(const size_t loops)
    {
        std::shared_ptr<int> ref_1 = std::make_shared<int>(1);
        std::shared_ptr<int> ref_9;

        {
            Clock clock;

            for (size_t i = 0; i < loops; i++)
            {
                auto ref_2 = ref_1;
                {
                    auto ref_3 = ref_2;
                    {
                        auto ref_4 = ref_3;
                        {
                            auto ref_5 = ref_4;
                            {
                                auto ref_6 = ref_5;
                                {
                                    auto ref_7 = ref_6;
                                    {
                                        auto ref_8 = ref_7;
                                        {
                                            ref_9 = ref_8;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            auto time = clock.GetTime().AsSeconds<float>();
            std::cout << "Std Ref(" << &ref_9 << ") time:" << time << std::endl;

        }
    }

    TEST(Memory, Reference_Speed)
    {
        const size_t loops = 1000000;
        RefTest(loops);
        StdRefTest(loops);
    }

}
