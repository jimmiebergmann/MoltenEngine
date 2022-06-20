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
#include "Molten/Memory/Pointer.hpp"


template<typename T>
class TestBase
{

public:

    virtual ~TestBase() = default;

    virtual T Get() const = 0;
    virtual void Set(const T value) = 0;

};

template<typename T>
class TestDerived : public TestBase<T>
{

public:

    TestDerived(const T value = 0) :
        value(value)
    { }

    ~TestDerived()
    {}

    T Get() const
    {
        return value;
    }

    void Set(const T my_value)
    {
        value = my_value;
    }

    T value;

};

TEST(Memory, Pointer_Creation)
{
    {
        {
            Molten::Ptr<uint32_t> ptr = Molten::Ptr<uint32_t>::Create(100);
            EXPECT_NE(ptr.Get(), nullptr);
            EXPECT_EQ(*ptr, (uint32_t)100);
            EXPECT_NE(*ptr, (uint32_t)99); 
        }
        {
            Molten::Ptr<TestDerived<uint32_t> > ptr = Molten::Ptr<TestDerived<uint32_t> >::Create(200);
            EXPECT_NE(ptr.Get(), nullptr);
            EXPECT_EQ(ptr->Get(), (uint32_t)200);
            EXPECT_NE(ptr->Get(), (uint32_t)199);

            EXPECT_EQ((*ptr).Get(), (uint32_t)200);
            EXPECT_EQ(ptr.Get()->Get(), (uint32_t)200);
        }
        {
            Molten::Ptr<TestBase<uint32_t> > ptr = Molten::Ptr<TestDerived<uint32_t> >::Create(300);
            EXPECT_NE(ptr.Get(), nullptr);
            EXPECT_EQ(ptr->Get(), (uint32_t)300);
            EXPECT_NE(ptr->Get(), (uint32_t)299);
        } 
    }
    {
        {
            Molten::Ptr<uint32_t[]> ptr = Molten::Ptr<uint32_t[]>::Create(2);
            EXPECT_NE(ptr.Get(), nullptr);
            ptr[0] = 101;
            ptr[1] = 201;
            EXPECT_EQ(ptr[0], (uint32_t)101);
            EXPECT_EQ(ptr[1], (uint32_t)201);
        }
        {
            Molten::Ptr<TestDerived<uint32_t>[] > ptr = Molten::Ptr<TestDerived<uint32_t>[] >::Create(2);
            EXPECT_NE(ptr.Get(), nullptr);
            ptr[0].value = 102;
            ptr[1].value = 202;
            EXPECT_EQ(ptr[0].Get(), (uint32_t)102);
            EXPECT_EQ(ptr[1].Get(), (uint32_t)202);
        }
    }
}

TEST(Memory, Pointer_Assignment)
{
    {
        Molten::Ptr<uint32_t> ptr;
        EXPECT_EQ(ptr.Get(), nullptr);
        ptr = Molten::Ptr<uint32_t>::Create(100);
        EXPECT_NE(ptr.Get(), nullptr);
    }
    {
        Molten::Ptr<uint32_t[]> ptr;
        EXPECT_EQ(ptr.Get(), nullptr);
        ptr = Molten::Ptr<uint32_t[]>::Create(100);
        EXPECT_NE(ptr.Get(), nullptr);
    }
}

TEST(Memory, Pointer_Move)
{
    {
        Molten::Ptr<uint32_t> ptr_first = Molten::Ptr<uint32_t>::Create(100);
        uint32_t* raw_ptr = ptr_first.Get();
        Molten::Ptr<uint32_t> ptr_moved_1(std::move(ptr_first));
        EXPECT_EQ(ptr_first.Get(), nullptr);
        EXPECT_NE(ptr_moved_1.Get(), nullptr);
        EXPECT_EQ(ptr_moved_1.Get(), raw_ptr);
    }
    {
        Molten::Ptr<TestDerived<uint32_t> > ptr_der;
        EXPECT_EQ(ptr_der.Get(), nullptr);
        ptr_der = Molten::Ptr<TestDerived<uint32_t> >::Create(100);
        EXPECT_NE(ptr_der.Get(), nullptr);
        Molten::Ptr<TestBase<uint32_t> > ptr_base(std::move(ptr_der));
        EXPECT_EQ(ptr_der.Get(), nullptr);
        EXPECT_NE(ptr_base.Get(), nullptr);
    }
}