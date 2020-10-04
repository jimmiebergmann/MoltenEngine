/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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
#include "Molten/Utility/BypassList.hpp"
#include <vector>

struct TestData1
{
    size_t value;

    bool operator == (const TestData1& rhs) const
    {
        return value == rhs.value;
    }
};

template<typename TValueType, typename TLaneType>
bool CompareListContent(
    typename Molten::BypassList<TValueType>::template Iterator<TLaneType> begin,
    typename Molten::BypassList<TValueType>::template Iterator<TLaneType> end,
    const std::vector<TValueType>& compareData)
{
    // Post increment.
    size_t i = 0;
    for (auto it = begin; it != end; it++)
    {
        EXPECT_EQ(*it, compareData[i]);

        if (!(*it == compareData[i]))
        {
            return false;
        }

        i++;
    }

    EXPECT_EQ(i, compareData.size());
    if (i != compareData.size())
    {
        return false;
    }

    // Pre increment.
    i = 0;
    auto it2 = begin;
    while(it2 != end)
    {
        EXPECT_EQ(*it2, compareData[i]);

        if (!(*it2 == compareData[i]))
        {
            return false;
        }

        ++it2;
        i++;
    }

    EXPECT_EQ(i, compareData.size());
    if (i != compareData.size())
    {
        return false;
    }
    
    return true;
}

namespace Molten
{
    using ListType = BypassList<TestData1>;
    using NormalLane = ListType::NormalLane;
    using PartialLane = ListType::PartialLane;
    using NormalConstLane = ListType::NormalConstLane;
    using PartialConstLane = ListType::PartialConstLane;

    TEST(Utility, BypassList_Empty)
    {    
        ListType list;
        NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
        PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

        EXPECT_EQ(list.GetSize(), size_t(0));
        EXPECT_EQ(normalLane.GetSize(), size_t(0));
        EXPECT_EQ(partialLane.GetSize(), size_t(0));

        ASSERT_TRUE(normalLane.begin() == normalLane.end());
        ASSERT_TRUE(partialLane.begin() == partialLane.end());

        for (auto& item : normalLane)
        {
            ASSERT_TRUE(false);
            EXPECT_EQ(item.value, size_t(0));
        }

        for (auto& item : partialLane)
        {
            ASSERT_TRUE(false);
            EXPECT_EQ(item.value, size_t(0));
        }

        list.Clear();
        EXPECT_EQ(list.GetSize(), size_t(0));
        EXPECT_EQ(normalLane.GetSize(), size_t(0));
        EXPECT_EQ(partialLane.GetSize(), size_t(0));
    }
   
    TEST(Utility, BypassList_PushBack)
    {
        { 
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            partialLane.PushBack(TestData1{ 1 });
            normalLane.PushBack(TestData1{ 2 });
            partialLane.PushBack(TestData1{ 3 });
            normalLane.PushBack(TestData1{ 4 });
            partialLane.PushBack(TestData1{ 5 });
            partialLane.PushBack(TestData1{ 6 });
            normalLane.PushBack(TestData1{ 7 });
            normalLane.PushBack(TestData1{ 8 });
            partialLane.PushBack(TestData1{ 9 });

            EXPECT_EQ(list.GetSize(), size_t(9));
            EXPECT_EQ(normalLane.GetSize(), size_t(9));
            EXPECT_EQ(partialLane.GetSize(), size_t(5));

            if (!CompareListContent<TestData1>(
                normalLane.begin(),
                normalLane.end(),
                { {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9} } ))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1>(
                partialLane.begin(),
                partialLane.end(),
                { {1}, {3}, {5}, {6}, {9} }))
            {
                ADD_FAILURE();
            }
        }
    }
    
    TEST(Utility, BypassList_PushFront)
    {
        {
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            partialLane.PushFront(TestData1{ 1 });
            normalLane.PushFront(TestData1{ 2 });
            partialLane.PushFront(TestData1{ 3 });
            normalLane.PushFront(TestData1{ 4 });
            partialLane.PushFront(TestData1{ 5 });
            partialLane.PushFront(TestData1{ 6 });
            normalLane.PushFront(TestData1{ 7 });
            normalLane.PushFront(TestData1{ 8 });
            partialLane.PushFront(TestData1{ 9 });

            EXPECT_EQ(list.GetSize(), size_t(9));
            EXPECT_EQ(normalLane.GetSize(), size_t(9));
            EXPECT_EQ(partialLane.GetSize(), size_t(5));

            if (!CompareListContent<TestData1>(
                normalLane.begin(),
                normalLane.end(),
                { {9}, {8}, {7}, {6}, {5}, {4}, {3}, {2}, {1} }))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1>(
                partialLane.begin(),
                partialLane.end(),
                { {9}, {6}, {5}, {3}, {1} }))
            {
                ADD_FAILURE();
            }
        }
    }
    
    TEST(Utility, BypassList_Insert)
    {
        { // End insert.
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            auto it1 = partialLane.Insert(partialLane.end(), TestData1{ 1 });
            auto it2 = partialLane.Insert(partialLane.end(), TestData1{ 2 });
            auto it3 = partialLane.Insert(partialLane.end(), TestData1{ 3 });

            EXPECT_EQ(list.GetSize(), size_t(3));
            EXPECT_EQ(normalLane.GetSize(), size_t(3));
            EXPECT_EQ(partialLane.GetSize(), size_t(3));

            {
                auto it = normalLane.begin();
                EXPECT_EQ((*it).value, size_t(1));
                EXPECT_EQ((*it1).value, size_t(1));

                std::advance(it, 1);
                EXPECT_EQ((*it).value, size_t(2));
                EXPECT_EQ((*it2).value, size_t(2));

                std::advance(it, 1);
                EXPECT_EQ((*it).value, size_t(3));
                EXPECT_EQ((*it3).value, size_t(3));
            }
            {
                auto it = partialLane.begin();
                EXPECT_EQ((*it).value, size_t(1));
                EXPECT_EQ((*it1).value, size_t(1));

                std::advance(it, 1);
                EXPECT_EQ((*it).value, size_t(2));
                EXPECT_EQ((*it2).value, size_t(2));

                std::advance(it, 1);
                EXPECT_EQ((*it).value, size_t(3));
                EXPECT_EQ((*it3).value, size_t(3));
            }
        }
        { // Shuffle insert.
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            auto it1 = partialLane.Insert(normalLane.end(), TestData1{ 1 });
            auto it2 = partialLane.Insert(normalLane.begin(), TestData1{ 2 });
            auto it3 = partialLane.Insert(it1, TestData1{ 3 });

            EXPECT_EQ(list.GetSize(), size_t(3));
            EXPECT_EQ(normalLane.GetSize(), size_t(3));
            EXPECT_EQ(partialLane.GetSize(), size_t(3));

            {
                auto it = normalLane.begin();
                EXPECT_EQ((*it).value, size_t(2));
                EXPECT_EQ((*it1).value, size_t(1));

                std::advance(it, 1);
                EXPECT_EQ((*it).value, size_t(3));
                EXPECT_EQ((*it2).value, size_t(2));

                std::advance(it, 1);
                EXPECT_EQ((*it).value, size_t(1));
                EXPECT_EQ((*it3).value, size_t(3));
            }
            {
                auto it = partialLane.begin();
                EXPECT_EQ((*it).value, size_t(2));
                EXPECT_EQ((*it1).value, size_t(1));

                std::advance(it, 1);
                EXPECT_EQ((*it).value, size_t(3));
                EXPECT_EQ((*it2).value, size_t(2));

                std::advance(it, 1);
                EXPECT_EQ((*it).value, size_t(1));
                EXPECT_EQ((*it3).value, size_t(3));
            }
        }
        { // 1 sub path insert.
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            normalLane.Insert(normalLane.end(), TestData1{ 1 });
            normalLane.Insert(normalLane.end(), TestData1{ 2 });
            auto it3 = normalLane.Insert(normalLane.end(), TestData1{ 3 });
            normalLane.Insert(normalLane.end(), TestData1{ 4 });

            EXPECT_EQ(list.GetSize(), size_t(4));
            EXPECT_EQ(normalLane.GetSize(), size_t(4));
            EXPECT_EQ(partialLane.GetSize(), size_t(0));

            partialLane.Insert(it3, TestData1{ 5 });

            EXPECT_EQ(list.GetSize(), size_t(5));
            EXPECT_EQ(normalLane.GetSize(), size_t(5));
            EXPECT_EQ(partialLane.GetSize(), size_t(1));

            if (!CompareListContent<TestData1>(
                normalLane.begin(),
                normalLane.end(),
                { {1}, {2}, {5}, {3}, {4} }))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1>(
                partialLane.begin(),
                partialLane.end(),
                { {5} }))
            {
                ADD_FAILURE();
            }
        }
        { // Some subs before.
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            partialLane.Insert(normalLane.end(), TestData1{ 1 });
            partialLane.Insert(normalLane.end(), TestData1{ 2 });
            auto it3 = normalLane.Insert(normalLane.end(), TestData1{ 3 });
            normalLane.Insert(normalLane.end(), TestData1{ 4 });

            EXPECT_EQ(list.GetSize(), size_t(4));
            EXPECT_EQ(normalLane.GetSize(), size_t(4));
            EXPECT_EQ(partialLane.GetSize(), size_t(2));

            partialLane.Insert(it3, TestData1{ 5 });

            EXPECT_EQ(list.GetSize(), size_t(5));
            EXPECT_EQ(normalLane.GetSize(), size_t(5));
            EXPECT_EQ(partialLane.GetSize(), size_t(3));

            if (!CompareListContent<TestData1>(
                normalLane.begin(),
                normalLane.end(),
                { {1}, {2}, {5}, {3}, {4} }))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1>(
                partialLane.begin(),
                partialLane.end(),
                { {1}, {2}, {5} }))
            {
                ADD_FAILURE();
            }
        }
        { // Some subs after.
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            normalLane.Insert(normalLane.end(), TestData1{ 1 });
            normalLane.Insert(normalLane.end(), TestData1{ 2 });
            auto it3 = partialLane.Insert(normalLane.end(), TestData1{ 3 });
            partialLane.Insert(normalLane.end(), TestData1{ 4 });

            EXPECT_EQ(list.GetSize(), size_t(4));
            EXPECT_EQ(normalLane.GetSize(), size_t(4));
            EXPECT_EQ(partialLane.GetSize(), size_t(2));

            partialLane.Insert(it3, TestData1{ 5 });

            EXPECT_EQ(list.GetSize(), size_t(5));
            EXPECT_EQ(normalLane.GetSize(), size_t(5));
            EXPECT_EQ(partialLane.GetSize(), size_t(3));

            if (!CompareListContent<TestData1>(
                normalLane.begin(),
                normalLane.end(),
                { {1}, {2}, {5}, {3}, {4} }))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1>(
                partialLane.begin(),
                partialLane.end(),
                { {5}, {3}, {4} }))
            {
                ADD_FAILURE();
            }
        }
    }
    
    TEST(Utility, BypassList_EraseAllInNormal)
    {
        { // 1 item.
            { // From lane
                ListType list;
                NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                EXPECT_EQ(list.GetSize(), size_t(1));
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                EXPECT_NE(normalLane.begin(), normalLane.end());
                EXPECT_NE(partialLane.begin(), partialLane.end());

                auto it = normalLane.begin();
                auto it2 = normalLane.Erase(it);
                EXPECT_EQ(it2, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(0));
                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
            { // From list
                ListType list;
                NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                EXPECT_EQ(list.GetSize(), size_t(1));
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                EXPECT_NE(normalLane.begin(), normalLane.end());
                EXPECT_NE(partialLane.begin(), partialLane.end());

                auto it = normalLane.begin();
                auto it2 = list.Erase(it);
                EXPECT_EQ(it2, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(0));
                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
        }
        { // 2 items.
            { // Forward
                ListType list;
                NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                partialLane.PushBack(TestData1{ 2 });
                EXPECT_EQ(list.GetSize(), size_t(2));
                EXPECT_EQ(normalLane.GetSize(), size_t(2));
                EXPECT_EQ(partialLane.GetSize(), size_t(2));

                auto it = normalLane.begin();
                auto it2 = normalLane.Erase(it);
                EXPECT_NE(it2, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(1));
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                EXPECT_EQ((*normalLane.begin()).value, size_t(2));
                EXPECT_EQ(++normalLane.begin(), normalLane.end());
                EXPECT_EQ(++partialLane.begin(), partialLane.end());

                it = normalLane.begin();
                auto it3 = normalLane.Erase(it);
                EXPECT_EQ(it3, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(0));
                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
            { // Backward.
                ListType list;
                NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                partialLane.PushBack(TestData1{ 2 });
                EXPECT_EQ(list.GetSize(), size_t(2));
                EXPECT_EQ(normalLane.GetSize(), size_t(2));
                EXPECT_EQ(partialLane.GetSize(), size_t(2));

                auto it = ++normalLane.begin();
                auto it2 = normalLane.Erase(it);
                EXPECT_EQ(it2, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(1));
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                EXPECT_EQ((*normalLane.begin()).value, size_t(1));
                EXPECT_EQ(++normalLane.begin(), normalLane.end());
                EXPECT_EQ(++partialLane.begin(), partialLane.end());

                it = normalLane.begin();
                auto it3 = normalLane.Erase(it);
                EXPECT_EQ(it3, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(0));
                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
        }
        { // 3 items.
            { // Forward
                ListType list;
                NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                partialLane.PushBack(TestData1{ 2 });
                partialLane.PushBack(TestData1{ 3 });
                EXPECT_EQ(list.GetSize(), size_t(3));
                EXPECT_EQ(normalLane.GetSize(), size_t(3));
                EXPECT_EQ(partialLane.GetSize(), size_t(3));

                auto it = normalLane.begin();
                auto it2 = normalLane.Erase(it);
                EXPECT_NE(it2, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(2));
                EXPECT_EQ(normalLane.GetSize(), size_t(2));
                EXPECT_EQ(partialLane.GetSize(), size_t(2));

                EXPECT_EQ((*normalLane.begin()).value, size_t(2));
                EXPECT_EQ((*(++normalLane.begin())).value, size_t(3));
                EXPECT_EQ((++(++normalLane.begin())), normalLane.end());
                EXPECT_EQ((++(++partialLane.begin())), partialLane.end());

                it = normalLane.begin();
                auto it3 = normalLane.Erase(it);
                EXPECT_NE(it3, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(1));
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                EXPECT_EQ((*normalLane.begin()).value, size_t(3));
                EXPECT_EQ(++normalLane.begin(), normalLane.end());
                EXPECT_EQ(++partialLane.begin(), partialLane.end());

                it = normalLane.begin();
                auto it4 = normalLane.Erase(it);
                EXPECT_EQ(it4, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(0));
                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
            { // Backward
                ListType list;
                NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                partialLane.PushBack(TestData1{ 2 });
                partialLane.PushBack(TestData1{ 3 });
                EXPECT_EQ(list.GetSize(), size_t(3));
                EXPECT_EQ(normalLane.GetSize(), size_t(3));
                EXPECT_EQ(partialLane.GetSize(), size_t(3));

                auto it = ++(++normalLane.begin());
                auto it2 = normalLane.Erase(it);
                EXPECT_EQ(it2, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(2));
                EXPECT_EQ(normalLane.GetSize(), size_t(2));
                EXPECT_EQ(partialLane.GetSize(), size_t(2));

                EXPECT_EQ((*normalLane.begin()).value, size_t(1));
                EXPECT_EQ((*(++normalLane.begin())).value, size_t(2));
                EXPECT_EQ((++(++normalLane.begin())), normalLane.end());
                EXPECT_EQ((++(++partialLane.begin())), partialLane.end());
                    
                it = ++normalLane.begin();
                auto it3 = normalLane.Erase(it);
                EXPECT_EQ(it3, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(1));
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                EXPECT_EQ((*normalLane.begin()).value, size_t(1));
                EXPECT_EQ(++normalLane.begin(), normalLane.end());
                EXPECT_EQ(++partialLane.begin(), partialLane.end());

                it = normalLane.begin();
                auto it4 = normalLane.Erase(it);
                EXPECT_EQ(it4, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(0));
                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
            { // Middle
                ListType list;
                NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                partialLane.PushBack(TestData1{ 2 });
                partialLane.PushBack(TestData1{ 3 });
                EXPECT_EQ(list.GetSize(), size_t(3));
                EXPECT_EQ(normalLane.GetSize(), size_t(3));
                EXPECT_EQ(partialLane.GetSize(), size_t(3));

                auto it = ++normalLane.begin();
                auto it2 = normalLane.Erase(it);
                EXPECT_NE(it2, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(2));
                EXPECT_EQ(normalLane.GetSize(), size_t(2));
                EXPECT_EQ(partialLane.GetSize(), size_t(2));

                EXPECT_EQ((*normalLane.begin()).value, size_t(1));
                EXPECT_EQ((*(++normalLane.begin())).value, size_t(3));
                EXPECT_EQ((++(++normalLane.begin())), normalLane.end());
                EXPECT_EQ((++(++partialLane.begin())), partialLane.end());

                it = normalLane.begin();
                auto it3 = normalLane.Erase(it);
                EXPECT_NE(it3, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(1));
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                EXPECT_EQ((*normalLane.begin()).value, size_t(3));
                EXPECT_EQ(++normalLane.begin(), normalLane.end());
                EXPECT_EQ(++partialLane.begin(), partialLane.end());

                it = normalLane.begin();
                auto it4 = normalLane.Erase(it);
                EXPECT_EQ(it4, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(0));
                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
        }
    }
   
    TEST(Utility, BypassList_EraseSomePartial)
    {

        {
            { // 1 item.
                ListType list;
                NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                normalLane.PushBack(TestData1{ 1 });
                EXPECT_EQ(list.GetSize(), size_t(1));
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                auto it = normalLane.begin();
                auto it2 = normalLane.Erase(it);
                EXPECT_EQ(it2, normalLane.end());

                EXPECT_EQ(list.GetSize(), size_t(0));
                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
            { // 2 items.
                { // 0 subs.
                    ListType list;
                    NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                    PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                    normalLane.PushBack(TestData1{ 1 });
                    normalLane.PushBack(TestData1{ 2 });
                    EXPECT_EQ(list.GetSize(), size_t(2));
                    EXPECT_EQ(normalLane.GetSize(), size_t(2));
                    EXPECT_EQ(partialLane.GetSize(), size_t(0));

                    auto it = normalLane.begin();
                    auto it2 = normalLane.Erase(it);
                    EXPECT_NE(it2, normalLane.end());

                    EXPECT_EQ(list.GetSize(), size_t(1));
                    EXPECT_EQ(normalLane.GetSize(), size_t(1));
                    EXPECT_EQ(partialLane.GetSize(), size_t(0));

                    EXPECT_EQ(++normalLane.begin(), normalLane.end());
                    EXPECT_EQ(partialLane.begin(), partialLane.end());

                    it = normalLane.begin();
                    auto it3 = normalLane.Erase(it);
                    EXPECT_EQ(it3, normalLane.end());

                    EXPECT_EQ(list.GetSize(), size_t(0));
                    EXPECT_EQ(normalLane.GetSize(), size_t(0));
                    EXPECT_EQ(partialLane.GetSize(), size_t(0));

                    EXPECT_EQ(normalLane.begin(), normalLane.end());
                    EXPECT_EQ(partialLane.begin(), partialLane.end());
                }
                { // 1 sub.
                    ListType list;
                    NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
                    PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

                    normalLane.PushBack(TestData1{ 1 });
                    partialLane.PushBack(TestData1{ 2 });
                    EXPECT_EQ(list.GetSize(), size_t(2));
                    EXPECT_EQ(normalLane.GetSize(), size_t(2));
                    EXPECT_EQ(partialLane.GetSize(), size_t(1));

                    auto it = normalLane.begin();
                    auto it2 = normalLane.Erase(it);
                    EXPECT_NE(it2, normalLane.end());

                    EXPECT_EQ(list.GetSize(), size_t(1));
                    EXPECT_EQ(normalLane.GetSize(), size_t(1));
                    EXPECT_EQ(partialLane.GetSize(), size_t(1));

                    EXPECT_EQ((*partialLane.begin()).value, size_t(2));
                    EXPECT_EQ(++normalLane.begin(), normalLane.end());
                    EXPECT_EQ(++partialLane.begin(), partialLane.end());

                    it = normalLane.begin();
                    auto it3 = normalLane.Erase(it);
                    EXPECT_EQ(it3, normalLane.end());

                    EXPECT_EQ(list.GetSize(), size_t(0));
                    EXPECT_EQ(normalLane.GetSize(), size_t(0));
                    EXPECT_EQ(partialLane.GetSize(), size_t(0));

                    EXPECT_EQ(normalLane.begin(), normalLane.end());
                    EXPECT_EQ(partialLane.begin(), partialLane.end());
                }
            }
        }
    }

    TEST(Utility, BypassListIterator_IsEmpty)
    {
        {
            ListType::Iterator<ListType::NormalLaneType> it1;
            EXPECT_TRUE(it1.IsEmpty());

            ListType::Iterator<ListType::PartialLaneType> it2;
            EXPECT_TRUE(it2.IsEmpty());

            ListType::ConstIterator<ListType::NormalLaneType> it3;
            EXPECT_TRUE(it3.IsEmpty());

            ListType::ConstIterator<ListType::PartialLaneType> it4;
            EXPECT_TRUE(it4.IsEmpty());
        }
        {
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            EXPECT_FALSE(normalLane.begin().IsEmpty());
            EXPECT_FALSE(normalLane.end().IsEmpty());
            EXPECT_FALSE(normalLane.begin().IsEmpty());
            EXPECT_FALSE(normalLane.end().IsEmpty());

            const ListType& constList = list;
            NormalConstLane constNormalLane = constList.GetLane<ListType::NormalLaneType>();
            EXPECT_FALSE(constNormalLane.begin().IsEmpty());
            EXPECT_FALSE(constNormalLane.end().IsEmpty());
            EXPECT_FALSE(constNormalLane.begin().IsEmpty());
            EXPECT_FALSE(constNormalLane.end().IsEmpty());

        }
    }

    TEST(Utility, BypassListIterator_Copy)
    {
        {
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();

            NormalLane::Iterator it = normalLane.begin();

            NormalLane::Iterator it2 = it;
            EXPECT_EQ(it2, it2);

            NormalLane::Iterator it3;
            it3 = it;

            EXPECT_EQ(it3, it3);
        }
        {
            ListType list;
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            PartialLane::Iterator it = partialLane.begin();

            PartialLane::Iterator it2 = it;
            EXPECT_EQ(it2, it2);

            PartialLane::Iterator it3;
            it3 = it;

            EXPECT_EQ(it3, it3);
        }
    }

    TEST(Utility, BypassListIterator_LaneTypeCast)
    {
        ListType list;
        PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

        PartialLane::Iterator partialIt = partialLane.begin();

        NormalLane::Iterator normalIt1 = partialIt;
        EXPECT_EQ(normalIt1, normalIt1);

        NormalLane::Iterator normalIt2;
        normalIt2 = partialIt;
        EXPECT_EQ(normalIt2, normalIt2);   
    }

    TEST(Utility, BypassListIterator_Traverse)
    {
        { // std::next
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            partialLane.PushBack(TestData1{ 1 });
            normalLane.PushBack(TestData1{ 2 });
            partialLane.PushBack(TestData1{ 3 });

            {
                auto it = normalLane.begin();

                auto itNext = it;
                EXPECT_EQ((*itNext).value, size_t(1));

                itNext = std::next(itNext);
                EXPECT_EQ((*itNext).value, size_t(2));

                itNext = std::next(itNext);
                EXPECT_EQ((*itNext).value, size_t(3));

                itNext = std::next(itNext);
                EXPECT_EQ(itNext, normalLane.end());
            }
            {
                auto it = partialLane.begin();

                auto itNext = it;
                EXPECT_EQ((*itNext).value, size_t(1));

                itNext = std::next(itNext);
                EXPECT_EQ((*itNext).value, size_t(3));

                itNext = std::next(itNext);
                EXPECT_EQ(itNext, partialLane.end());
            }
        }
        { // std::prev
            ListType list;
            NormalLane normalLane = list.GetLane<ListType::NormalLaneType>();
            PartialLane partialLane = list.GetLane<ListType::PartialLaneType>();

            partialLane.PushBack(TestData1{ 1 });
            normalLane.PushBack(TestData1{ 2 });
            partialLane.PushBack(TestData1{ 3 });

            {
                auto it = normalLane.end();

                auto itPrev = std::prev(it);
                EXPECT_EQ((*itPrev).value, size_t(3));

                itPrev = std::prev(itPrev);
                EXPECT_EQ((*itPrev).value, size_t(2));

                itPrev = std::prev(itPrev);
                EXPECT_EQ((*itPrev).value, size_t(1));
            }
           {
                auto it = partialLane.end();

                auto itPrev = std::prev(it);
                EXPECT_EQ((*itPrev).value, size_t(3));

                itPrev = std::prev(itPrev);
                EXPECT_EQ((*itPrev).value, size_t(1));
            }
           
        }
    }

}