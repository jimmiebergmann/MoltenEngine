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

    TEST(Memory, Reference_Constuct)
    {
        {
            Ref<int32_t> ref;
            EXPECT_EQ(ref.Get(), nullptr);
        }
        {
            Ref<int32_t> ref(new int(6543));
            EXPECT_NE(ref.Get(), nullptr);
            EXPECT_EQ(*ref, int32_t(6543));
        }
        {
            bool destroyed = false;

            {
                Ref<int32_t> ref(new int(6543), [&destroyed](int32_t* ptr)
                    {
                        destroyed = true;
                        delete ptr;
                    });
                EXPECT_NE(ref.Get(), nullptr);
                EXPECT_EQ(*ref, int32_t(6543));
                EXPECT_FALSE(destroyed);
            }
            EXPECT_TRUE(destroyed);
        }
    }

    TEST(Memory, Reference_Create)
    {
        {
            auto ref = Ref<int32_t>::Create(12345);
            EXPECT_EQ(*ref, int32_t(12345));

            ref = Ref<int32_t>::Create(5432);
            EXPECT_EQ(*ref, int32_t(5432));
        }
        {
            struct TestStruct
            {
                TestStruct() :
                    value1(333),
                    value2("Test string")
                { }

                TestStruct(const int32_t value1, const std::string & value2 = "Default string") :
                    value1(value1),
                    value2(value2)
                { }

                int32_t value1;
                std::string value2;
            };

            {
                Ref<TestStruct> ref = Ref<TestStruct>::Create(112233, "My string");
                TestStruct* ref_ptr = ref.Get();
                TestStruct & ref_ref = *ref;

                EXPECT_TRUE(ref_ptr != nullptr);
                EXPECT_EQ(ref_ptr, &ref_ref);
                EXPECT_EQ(ref_ptr->value1, int32_t(112233));
                EXPECT_EQ(ref_ref.value1, int32_t(112233));
                EXPECT_STREQ(ref_ptr->value2.c_str(), "My string");            
                EXPECT_STREQ(ref_ref.value2.c_str(), "My string");
            }
            {
                Ref<TestStruct> ref = Ref<TestStruct>::Create(112233);
                TestStruct* ref_ptr = ref.Get();
                TestStruct& ref_ref = *ref;

                EXPECT_TRUE(ref_ptr != nullptr);
                EXPECT_EQ(ref_ptr, &ref_ref);
                EXPECT_EQ(ref_ptr->value1, int32_t(112233));
                EXPECT_EQ(ref_ref.value1, int32_t(112233));
                EXPECT_STREQ(ref_ptr->value2.c_str(), "Default string");
                EXPECT_STREQ(ref_ref.value2.c_str(), "Default string");
            }
            {
                Ref<TestStruct> ref = Ref<TestStruct>::Create();
                TestStruct* ref_ptr = ref.Get();
                TestStruct& ref_ref = *ref;

                EXPECT_TRUE(ref_ptr != nullptr);
                EXPECT_EQ(ref_ptr, &ref_ref);
                EXPECT_EQ(ref.GetUseCount(), size_t(1));
                EXPECT_EQ(ref_ptr->value1, int32_t(333));
                EXPECT_EQ(ref_ref.value1, int32_t(333));
                EXPECT_STREQ(ref_ptr->value2.c_str(), "Test string");
                EXPECT_STREQ(ref_ref.value2.c_str(), "Test string");
            }         
        }
        {
            struct TestClassA
            {
                TestClassA(const int32_t valueA) :
                    valueA(valueA)
                {}

                virtual ~TestClassA()
                { }

                int32_t valueA;
            };

            struct TestClassB : public TestClassA
            {
                TestClassB(const int32_t valueA, const int32_t valueB) :
                    TestClassA(valueA),
                    valueB(valueB)
                {}

                virtual ~TestClassB()
                { }

                int32_t valueB;
            };

            struct TestClassC : public TestClassB
            {
                TestClassC(const int32_t valueA, const int32_t valueB, const int32_t valueC) :
                    TestClassB(valueA, valueB),
                    valueC(valueC)
                {}

                int32_t valueC;
            };

            {
                Ref<TestClassA> ref = Ref<TestClassA>::Create(100);
                EXPECT_EQ(ref->valueA, int32_t(100));
                EXPECT_EQ(ref.Get()->valueA, int32_t(100));
            }
            {
                Ref<TestClassB> ref = Ref<TestClassB>::Create(200, 300);
                EXPECT_EQ(ref->valueA, int32_t(200));
                EXPECT_EQ(ref.Get()->valueA, int32_t(200));
                EXPECT_EQ(ref->valueB, int32_t(300));
                EXPECT_EQ(ref.Get()->valueB, int32_t(300));
            }
            {
                Ref<TestClassC> ref_A = Ref<TestClassC>::Create(400, 500, 600);
                EXPECT_EQ(ref_A->valueA, int32_t(400));
                EXPECT_EQ(ref_A.Get()->valueA, int32_t(400));

                Ref<TestClassB> ref_B = ref_A;
                EXPECT_EQ(ref_B->valueA, int32_t(400));
                EXPECT_EQ(ref_B.Get()->valueA, int32_t(400));
                EXPECT_EQ(ref_B->valueB, int32_t(500));
                EXPECT_EQ(ref_B.Get()->valueB, int32_t(500));

                Ref<TestClassC> ref_C1 = ref_A;
                EXPECT_EQ(ref_C1->valueA, int32_t(400));
                EXPECT_EQ(ref_C1.Get()->valueA, int32_t(400));
                EXPECT_EQ(ref_C1->valueB, int32_t(500));
                EXPECT_EQ(ref_C1.Get()->valueB, int32_t(500));
                EXPECT_EQ(ref_C1->valueC, int32_t(600));
                EXPECT_EQ(ref_C1.Get()->valueC, int32_t(600));

                Ref<TestClassC> ref_C2 = ref_B;
                EXPECT_EQ(ref_C2->valueA, int32_t(400));
                EXPECT_EQ(ref_C2.Get()->valueA, int32_t(400));
                EXPECT_EQ(ref_C2->valueB, int32_t(500));
                EXPECT_EQ(ref_C2.Get()->valueB, int32_t(500));
                EXPECT_EQ(ref_C2->valueC, int32_t(600));
                EXPECT_EQ(ref_C2.Get()->valueC, int32_t(600));
            }
        }
    }

    TEST(Memory, Reference_Copy)
    {
        {
            Ref<int64_t> ref = Ref<int64_t>::Create(112233);
            EXPECT_EQ(ref.GetUseCount(), size_t(1));

            auto ref_copy_1 = ref;
            EXPECT_EQ(ref.GetUseCount(), size_t(2));
            Ref<int64_t> ref_copy_2;
            EXPECT_EQ(ref.GetUseCount(), size_t(2));
            ref_copy_2 = ref;
            EXPECT_EQ(ref.GetUseCount(), size_t(3));
        }
        {
            Ref<int32_t> ref_1_data_1 = Ref<int32_t>::Create(123);
            Ref<int32_t> ref_2_data_1 = ref_1_data_1;
            Ref<int32_t> ref_1_data_2 = Ref<int32_t>::Create(123);
            Ref<int32_t> ref_2_data_2 = ref_1_data_2;

            EXPECT_EQ(ref_1_data_1.GetUseCount(), size_t(2));
            EXPECT_EQ(ref_2_data_1.GetUseCount(), size_t(2));
            EXPECT_EQ(ref_1_data_2.GetUseCount(), size_t(2));
            EXPECT_EQ(ref_2_data_2.GetUseCount(), size_t(2));

            ref_2_data_2 = ref_1_data_1;

            EXPECT_EQ(ref_1_data_1.GetUseCount(), size_t(3));
            EXPECT_EQ(ref_2_data_1.GetUseCount(), size_t(3));
            EXPECT_EQ(ref_1_data_2.GetUseCount(), size_t(1));
            EXPECT_EQ(ref_2_data_2.GetUseCount(), size_t(3));
        }
    }

    TEST(Memory, Reference_Move)
    {
        Ref<int64_t> ref = Ref<int64_t>::Create(112233);
        EXPECT_EQ(ref.GetUseCount(), size_t(1));

        auto ref_copy_1 = ref;
        EXPECT_EQ(ref.GetUseCount(), size_t(2));
        EXPECT_EQ(ref_copy_1.GetUseCount(), size_t(2));

        Ref<int64_t> ref_move_1; 
        EXPECT_EQ(ref.GetUseCount(), size_t(2));
        EXPECT_EQ(ref_copy_1.GetUseCount(), size_t(2));

        ref_move_1 = std::move(ref);
        EXPECT_EQ(ref.GetUseCount(), size_t(0));      
        EXPECT_EQ(ref_copy_1.GetUseCount(), size_t(2));
        EXPECT_EQ(ref_move_1.GetUseCount(), size_t(2));

        EXPECT_EQ(ref.Get(), nullptr);
        EXPECT_NE(ref_copy_1.Get(), nullptr);
        EXPECT_NE(ref_move_1.Get(), nullptr);
    }

    TEST(Memory, Reference_Destructor)
    {

        struct MyObj
        {
            MyObj(bool& destroyed) :
                destroyed(destroyed),
                value(5000)
            {
                EXPECT_FALSE(destroyed);
            }

            ~MyObj()
            {
                EXPECT_FALSE(destroyed);
                destroyed = true;
                EXPECT_TRUE(destroyed);
            }

            int32_t value;
            bool& destroyed;
        };

        class TestDestructBase
        {
        public:

            virtual ~TestDestructBase()
            { }

        };

        class TestDestruct : public TestDestructBase
        {
        public:

            TestDestruct(MyObj& object, bool& destroyed) :
                object(object),
                value(998877),
                destroyed(destroyed)
            {
                EXPECT_FALSE(destroyed);
            }

            ~TestDestruct()
            {
                EXPECT_FALSE(destroyed);
                destroyed = true;
                EXPECT_TRUE(destroyed);
            }

            MyObj& object;
            int32_t value;
            bool& destroyed;
        };
  
        // Scope delete.
        {
            {
                bool destroyed_1 = false;
                bool destroyed_2 = false;

                {
                    MyObj obj(destroyed_1);

                    EXPECT_FALSE(destroyed_1);
                    EXPECT_FALSE(destroyed_2);

                    {
                        Ref<TestDestruct> ref = Ref<TestDestruct>::Create(obj, destroyed_2);

                        EXPECT_FALSE(destroyed_1);
                        EXPECT_FALSE(destroyed_2);
                    }

                    EXPECT_TRUE(destroyed_2);
                }
                EXPECT_TRUE(destroyed_1);
            }
            {
                bool destroyed_1 = false;
                bool destroyed_2 = false;

                {
                    MyObj obj(destroyed_1);

                    EXPECT_FALSE(destroyed_1);
                    EXPECT_FALSE(destroyed_2);

                    {
                        Ref<TestDestructBase> ref = Ref<TestDestruct>::Create(obj, destroyed_2);

                        EXPECT_FALSE(destroyed_1);
                        EXPECT_FALSE(destroyed_2);
                    }

                    EXPECT_TRUE(destroyed_2);
                }
                EXPECT_TRUE(destroyed_1);
            }
           
        }

        // Assign delete
        {
            {
                bool destroyed_dummy = false;
                bool destroyed = false;

                {
                    MyObj obj(destroyed_dummy);
                    Ref<TestDestruct> ref = Ref<TestDestruct>::Create(obj, destroyed);
                    Ref<TestDestruct> ref_none;

                    EXPECT_FALSE(destroyed);
                    ref = ref_none;
                    EXPECT_TRUE(destroyed);
                }
            }
            {
                bool destroyed_dummy = false;
                bool destroyed = false;

                {
                    MyObj obj(destroyed_dummy);
                    Ref<TestDestructBase> ref = Ref<TestDestruct>::Create(obj, destroyed);
                    Ref<TestDestruct> ref_none;

                    EXPECT_FALSE(destroyed);
                    ref = ref_none;
                    EXPECT_TRUE(destroyed);
                }
            }
            
        }

        // Move delete.
        {
            {
                bool destroyed_dummy = false;
                bool destroyed = false;

                {
                    MyObj obj(destroyed_dummy);
                    Ref<TestDestruct> ref = Ref<TestDestruct>::Create(obj, destroyed);
                    Ref<TestDestruct> ref_none;

                    EXPECT_FALSE(destroyed);
                    ref = std::move(ref_none);
                    EXPECT_TRUE(destroyed);
                }
            }
            {
                bool destroyed_dummy = false;
                bool destroyed = false;

                {
                    MyObj obj(destroyed_dummy);
                    Ref<TestDestructBase> ref = Ref<TestDestruct>::Create(obj, destroyed);
                    Ref<TestDestruct> ref_none;

                    EXPECT_FALSE(destroyed);
                    ref = std::move(ref_none);
                    EXPECT_TRUE(destroyed);
                }
            }
        }
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
        Curse::Test::PrintInfo("Ref copy test - Debug - Unoptimized.");
    #elif defined(CURSE_BUILD_RELEASE)
        Curse::Test::PrintInfo("Ref copy test - Release - Optimized.");
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
        Curse::Test::PrintInfo("Ref move test - Debug - Unoptimized.");
    #elif defined(CURSE_BUILD_RELEASE)
        Curse::Test::PrintInfo("Ref move test - Release - Optimized.");
    #endif

        EXPECT_NO_THROW(RefMoveTest(loops));
        EXPECT_NO_THROW(StdMoveRefTest(loops));
        Curse::Test::PrintInfo("------------------");
    }

}
