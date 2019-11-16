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

#include "Test.hpp"
#include "Curse/Memory/Reference.hpp"
#include <Curse/System/Clock.hpp>
#include <memory>
#include <type_traits>


namespace Curse
{
    TEST(Memory, Reference)
    {
        EXPECT_TRUE((std::is_same<Reference<int>, Ref<int> >::value));   
    }


    static void RefCopyTest(const size_t loops)
    {
        Ref<int> ref = Ref<int>::Create(123);
        Ref<int> ref_10;
        {       
            Clock clock;

            for (size_t i = 0; i < loops; i++)
            {
                auto ref_1 = ref;
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
                                                auto ref_9 = ref_8;
                                                {
                                                    ref_10 = ref_9;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            auto time = clock.GetTime().AsNanoseconds<uint64_t>();
            Test::PrintInfo("Curse::Reference - Copy time: " + std::to_string(time) + " ns.    Value: " + std::to_string(*ref_10) );
        }
    }

    static void StdCopyRefTest(const size_t loops)
    {
        std::shared_ptr<int> ref = std::make_shared<int>(123);
        std::shared_ptr<int> ref_10;

        Clock clock;

        for (size_t i = 0; i < loops; i++)
        {
            auto ref_1 = ref;
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
                                            auto ref_9 = ref_8;
                                            {
                                                ref_10 = ref_9;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        auto time = clock.GetTime().AsNanoseconds<uint64_t>();
        Test::PrintInfo("std::shared_ptr  - Copy time: " + std::to_string(time) + " ns.    Value: " + std::to_string(*ref_10));
    }
    TEST(Memory, Reference_CopySpeedTest)
    { 
        const size_t loops = 1000;
        Curse::Test::PrintInfo("------------------");

    #if defined(CURSE_BUILD_DEBUG)
        Curse::Test::PrintInfo("Debug - Unoptimized.");
    #elif defined(CURSE_BUILD_RELEASE)
        Curse::Test::PrintInfo("Debug - Optimized.");
    #endif

        EXPECT_NO_THROW(RefCopyTest(loops));
        EXPECT_NO_THROW(StdCopyRefTest(loops));
        Curse::Test::PrintInfo("------------------");
    }

    static void RefMoveTest(const size_t loops)
    {
        Ref<int> ref_10 = Ref<int>::Create(123);

        Clock clock;

        for (size_t i = 0; i < loops; i++)
        {
            auto ref = std::move(ref_10);
            auto ref_1 = std::move(ref);
            auto ref_2 = std::move(ref_1);
            auto ref_3 = std::move(ref_2);
            auto ref_4 = std::move(ref_3);
            auto ref_5 = std::move(ref_4);
            auto ref_6 = std::move(ref_5);
            auto ref_7 = std::move(ref_6);
            auto ref_8 = std::move(ref_7);
            auto ref_9 = std::move(ref_8);
            ref_10 = std::move(ref_9);
        }

        auto time = clock.GetTime().AsNanoseconds<uint64_t>();
        Test::PrintInfo("Curse::Reference  - Move time: " + std::to_string(time) + " ns.    Value: " + std::to_string(*ref_10));
    }

    static void StdMoveRefTest(const size_t loops)
    {
        std::shared_ptr<int> ref_10 = std::make_shared<int>(123);
        
        Clock clock;

        for (size_t i = 0; i < loops; i++)
        {
            auto ref = std::move(ref_10);
            auto ref_1 = std::move(ref);
            auto ref_2 = std::move(ref_1);
            auto ref_3 = std::move(ref_2);
            auto ref_4 = std::move(ref_3);
            auto ref_5 = std::move(ref_4);
            auto ref_6 = std::move(ref_5);
            auto ref_7 = std::move(ref_6);
            auto ref_8 = std::move(ref_7);
            auto ref_9 = std::move(ref_8);
            ref_10 = std::move(ref_9);
        }

        auto time = clock.GetTime().AsNanoseconds<uint64_t>();
        Test::PrintInfo("std::shared_ptr   - Move time: " + std::to_string(time) + " ns.    Value: " + std::to_string(*ref_10));      
    }

    TEST(Memory, Reference_MoveSpeedTest)
    {
        const size_t loops = 1000;
        Curse::Test::PrintInfo("------------------");

    #if defined(CURSE_BUILD_DEBUG)
        Curse::Test::PrintInfo("Debug - Unoptimized.");
    #elif defined(CURSE_BUILD_RELEASE)
        Curse::Test::PrintInfo("Debug - Optimized.");
    #endif

        EXPECT_NO_THROW(RefMoveTest(loops));
        EXPECT_NO_THROW(StdMoveRefTest(loops));
        Curse::Test::PrintInfo("------------------");
    }

}
