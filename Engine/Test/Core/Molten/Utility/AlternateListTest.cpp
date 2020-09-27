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
#include "Molten/Utility/AlternateList.hpp"
#include <vector>

struct TestData1
{
    size_t value;

    bool operator == (const TestData1& rhs) const
    {
        return value == rhs.value;
    }
};

template<typename TValueType, typename TPathType>
bool CompareListContent(
    typename Molten::AlternateList<TValueType>::template Iterator<TPathType> begin,
    typename Molten::AlternateList<TValueType>::template Iterator<TPathType> end,
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
    TEST(Utility, AlternateList_Empty)
    {
        using ListType = AlternateList<TestData1>;
        ListType list;

        EXPECT_EQ(list.GetMainSize(), size_t(0));
        EXPECT_EQ(list.GetSubSize(), size_t(0));

        auto mainPath = list.GetMainPath();
        auto subPath = list.GetSubPath();

        ASSERT_TRUE(mainPath.begin() == mainPath.end());
        ASSERT_TRUE(subPath.begin() == subPath.end());

        for (auto& node : mainPath)
        {
            ASSERT_TRUE(false);
            EXPECT_EQ(node.value, size_t(0));
        }

        for (auto& node : subPath)
        {
            ASSERT_TRUE(false);
            EXPECT_EQ(node.value, size_t(0));
        }
    }

    TEST(Utility, AlternateListIterator_IsEmpty)
    {
        using ListType = AlternateList<TestData1>;

        {
            ListType::Iterator<ListType::MainPath> it1;
            EXPECT_TRUE(it1.IsEmpty());

            ListType::Iterator<ListType::SubPath> it2;
            EXPECT_TRUE(it2.IsEmpty());

            ListType::ConstIterator<ListType::MainPath> it3;
            EXPECT_TRUE(it3.IsEmpty());

            ListType::ConstIterator<ListType::SubPath> it4;
            EXPECT_TRUE(it4.IsEmpty());
        }
        {
            ListType list;
            ListType::IteratorPath<ListType::MainPath> path = list.GetMainPath();
            EXPECT_FALSE(path.begin().IsEmpty());
            EXPECT_FALSE(path.end().IsEmpty());
            EXPECT_FALSE(path.begin().IsEmpty());
            EXPECT_FALSE(path.end().IsEmpty());

            const ListType& constList = list;
            ListType::ConstIteratorPath<ListType::MainPath> constPath = constList.GetMainPath();
            EXPECT_FALSE(constPath.begin().IsEmpty());
            EXPECT_FALSE(constPath.end().IsEmpty());
            EXPECT_FALSE(constPath.begin().IsEmpty());
            EXPECT_FALSE(constPath.end().IsEmpty());
        }
    }

    TEST(Utility, AlternateList_PushBack)
    {
        using ListType = AlternateList<TestData1>;

        {
            ListType list;
            list.PushBack(true, TestData1{ 1 });
            list.PushBack(false, TestData1{ 2 });
            list.PushBack(true, TestData1{ 3 });
            list.PushBack(false, TestData1{ 4 });
            list.PushBack(true, TestData1{ 5 });
            list.PushBack(true, TestData1{ 6 });
            list.PushBack(false, TestData1{ 7 });
            list.PushBack(false, TestData1{ 8 });
            list.PushBack(true, TestData1{ 9 });

            EXPECT_EQ(list.GetSize<AlternateListMainPath>(), size_t(9));
            EXPECT_EQ(list.GetMainSize(), size_t(9));
            EXPECT_EQ(list.GetSize<AlternateListSubPath>(), size_t(5));
            EXPECT_EQ(list.GetSubSize(), size_t(5));

            if (!CompareListContent<TestData1, AlternateListMainPath>(
                list.GetMainPath().begin(), 
                list.GetMainPath().end(), 
                { {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9} } ))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1, AlternateListSubPath>(
                list.GetSubPath().begin(),
                list.GetSubPath().end(),
                { {1}, {3}, {5}, {6}, {9} }))
            {
                ADD_FAILURE();
            }
        }
    }

    TEST(Utility, AlternateList_PushFront)
    {
        using ListType = AlternateList<TestData1>;

        {
            ListType list;
            list.PushFront(true, TestData1{ 1 });
            list.PushFront(false, TestData1{ 2 });
            list.PushFront(true, TestData1{ 3 });
            list.PushFront(false, TestData1{ 4 });
            list.PushFront(true, TestData1{ 5 });
            list.PushFront(true, TestData1{ 6 });
            list.PushFront(false, TestData1{ 7 });
            list.PushFront(false, TestData1{ 8 });
            list.PushFront(true, TestData1{ 9 });

            EXPECT_EQ(list.GetSize<AlternateListMainPath>(), size_t(9));
            EXPECT_EQ(list.GetMainSize(), size_t(9));
            EXPECT_EQ(list.GetSize<AlternateListSubPath>(), size_t(5));
            EXPECT_EQ(list.GetSubSize(), size_t(5));

            if (!CompareListContent<TestData1, AlternateListMainPath>(
                list.GetMainPath().begin(),
                list.GetMainPath().end(),
                { {9}, {8}, {7}, {6}, {5}, {4}, {3}, {2}, {1} }))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1, AlternateListSubPath>(
                list.GetSubPath().begin(),
                list.GetSubPath().end(),
                { {9}, {6}, {5}, {3}, {1} }))
            {
                ADD_FAILURE();
            }
        }
    }
    
    TEST(Utility, AlternateList_Insert)
    {
        using ListType = AlternateList<TestData1>;

        { // End insert.
            ListType list;
            auto mainPath = list.GetMainPath();

            auto it1 = list.Insert(mainPath.end(), true, TestData1{ 1 });
            auto it2 = list.Insert(mainPath.end(), true, TestData1{ 2 });
            auto it3 = list.Insert(mainPath.end(), true, TestData1{ 3 });

            EXPECT_EQ(mainPath.GetSize(), size_t(3));

            auto it = mainPath.begin();
            EXPECT_EQ((*it).value, size_t(1));
            EXPECT_EQ((*it1).value, size_t(1));

            std::advance(it, 1);
            EXPECT_EQ((*it).value, size_t(2));
            EXPECT_EQ((*it2).value, size_t(2));

            std::advance(it, 1);
            EXPECT_EQ((*it).value, size_t(3));
            EXPECT_EQ((*it3).value, size_t(3));
        }
        { // Shuffle insert.
            ListType list;
            auto mainPath = list.GetMainPath();

            auto it1 = list.Insert(mainPath.end(), true, TestData1{ 1 });
            auto it2 = list.Insert(mainPath.begin(), true, TestData1{ 2 });
            auto it3 = list.Insert(it1, true, TestData1{ 3 });

            EXPECT_EQ(mainPath.GetSize(), size_t(3));

            auto it = mainPath.begin();
            EXPECT_EQ((*it).value, size_t(2));
            EXPECT_EQ((*it1).value, size_t(1));

            std::advance(it, 1);
            EXPECT_EQ((*it).value, size_t(3));
            EXPECT_EQ((*it2).value, size_t(2));

            std::advance(it, 1);
            EXPECT_EQ((*it).value, size_t(1));
            EXPECT_EQ((*it3).value, size_t(3));
        }
        { // No sub path insert.
            ListType list;
            auto mainPath = list.GetMainPath();
            auto subPath = list.GetSubPath();

            auto it1 = list.Insert(mainPath.end(), false, TestData1{ 1 });
            auto it2 = list.Insert(mainPath.end(), false, TestData1{ 2 });
            auto it3 = list.Insert(mainPath.end(), false, TestData1{ 3 });
            auto it4 = list.Insert(mainPath.end(), false, TestData1{ 4 });

            EXPECT_EQ(mainPath.GetSize(), size_t(4));
            EXPECT_EQ(subPath.GetSize(), size_t(0));

            auto it5 = list.Insert(it3, true, TestData1{ 5 });

            EXPECT_EQ(mainPath.GetSize(), size_t(5));
            EXPECT_EQ(subPath.GetSize(), size_t(1));

            if (!CompareListContent<TestData1, AlternateListMainPath>(
                list.GetMainPath().begin(),
                list.GetMainPath().end(),
                { {1}, {2}, {5}, {3}, {4} }))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1, AlternateListSubPath>(
                list.GetSubPath().begin(),
                list.GetSubPath().end(),
                { {5} }))
            {
                ADD_FAILURE();
            }
        }
        { // Some subs before.
            ListType list;
            auto mainPath = list.GetMainPath();
            auto subPath = list.GetSubPath();

            auto it1 = list.Insert(mainPath.end(), true, TestData1{ 1 });
            auto it2 = list.Insert(mainPath.end(), true, TestData1{ 2 });
            auto it3 = list.Insert(mainPath.end(), false, TestData1{ 3 });
            auto it4 = list.Insert(mainPath.end(), false, TestData1{ 4 });

            EXPECT_EQ(mainPath.GetSize(), size_t(4));
            EXPECT_EQ(subPath.GetSize(), size_t(2));

            auto it5 = list.Insert(it3, true, TestData1{ 5 });

            EXPECT_EQ(mainPath.GetSize(), size_t(5));
            EXPECT_EQ(subPath.GetSize(), size_t(3));

            if (!CompareListContent<TestData1, AlternateListMainPath>(
                list.GetMainPath().begin(),
                list.GetMainPath().end(),
                { {1}, {2}, {5}, {3}, {4} }))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1, AlternateListSubPath>(
                list.GetSubPath().begin(),
                list.GetSubPath().end(),
                { {1}, {2}, {5} }))
            {
                ADD_FAILURE();
            }
        }
        { // Some subs after.
            ListType list;
            auto mainPath = list.GetMainPath();
            auto subPath = list.GetSubPath();

            auto it1 = list.Insert(mainPath.end(), false, TestData1{ 1 });
            auto it2 = list.Insert(mainPath.end(), false, TestData1{ 2 });
            auto it3 = list.Insert(mainPath.end(), true, TestData1{ 3 });
            auto it4 = list.Insert(mainPath.end(), true, TestData1{ 4 });

            EXPECT_EQ(mainPath.GetSize(), size_t(4));
            EXPECT_EQ(subPath.GetSize(), size_t(2));

            auto it5 = list.Insert(it3, true, TestData1{ 5 });

            EXPECT_EQ(mainPath.GetSize(), size_t(5));
            EXPECT_EQ(subPath.GetSize(), size_t(3));

            if (!CompareListContent<TestData1, AlternateListMainPath>(
                list.GetMainPath().begin(),
                list.GetMainPath().end(),
                { {1}, {2}, {5}, {3}, {4} }))
            {
                ADD_FAILURE();
            }

            if (!CompareListContent<TestData1, AlternateListSubPath>(
                list.GetSubPath().begin(),
                list.GetSubPath().end(),
                { {5}, {3}, {4} }))
            {
                ADD_FAILURE();
            }
        }
    }

    TEST(Utility, AlternateList_EraseAllMain)
    {
        using ListType = AlternateList<TestData1>;

        {
            { // 1 item.
                ListType list;

                list.PushBack(true, TestData1{ 1 });
                EXPECT_EQ(list.GetMainSize(), size_t(1));
                EXPECT_EQ(list.GetSubSize(), size_t(1));

                auto it = list.GetPath<AlternateListMainPath>().begin();
                list.Erase(it);

                EXPECT_EQ(list.GetMainSize(), size_t(0));
                EXPECT_EQ(list.GetSubSize(), size_t(0));

                EXPECT_EQ(list.GetPath<AlternateListMainPath>().begin(), list.GetPath<AlternateListMainPath>().end());
                EXPECT_EQ(list.GetPath<AlternateListSubPath>().begin(), list.GetPath<AlternateListSubPath>().end());
            }
            { // 2 items.
                { // Forward
                    ListType list;
                    auto mainPath = list.GetPath<AlternateListMainPath>();
                    auto subPath = list.GetPath<AlternateListSubPath>();

                    list.PushBack(true, TestData1{ 1 });
                    list.PushBack(true, TestData1{ 2 });
                    EXPECT_EQ(list.GetMainSize(), size_t(2));
                    EXPECT_EQ(list.GetSubSize(), size_t(2));

                    auto it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(1));
                    EXPECT_EQ(list.GetSubSize(), size_t(1));

                    EXPECT_EQ((*mainPath.begin()).value, size_t(2));
                    EXPECT_EQ(++mainPath.begin(), mainPath.end());
                    EXPECT_EQ(++subPath.begin(), subPath.end());

                    it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(0));
                    EXPECT_EQ(list.GetSubSize(), size_t(0));

                    EXPECT_EQ(mainPath.begin(), mainPath.end());
                    EXPECT_EQ(subPath.begin(), subPath.end());
                }
                { // Backward.
                    ListType list;
                    auto mainPath = list.GetPath<AlternateListMainPath>();
                    auto subPath = list.GetPath<AlternateListSubPath>();

                    list.PushBack(true, TestData1{ 1 });
                    list.PushBack(true, TestData1{ 2 });
                    EXPECT_EQ(list.GetMainSize(), size_t(2));
                    EXPECT_EQ(list.GetSubSize(), size_t(2));

                    auto it = ++mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(1));
                    EXPECT_EQ(list.GetSubSize(), size_t(1));

                    EXPECT_EQ((*mainPath.begin()).value, size_t(1));
                    EXPECT_EQ(++mainPath.begin(), mainPath.end());
                    EXPECT_EQ(++subPath.begin(), subPath.end());

                    it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(0));
                    EXPECT_EQ(list.GetSubSize(), size_t(0));

                    EXPECT_EQ(mainPath.begin(), mainPath.end());
                    EXPECT_EQ(subPath.begin(), subPath.end());
                }
            }
            { // 3 items.
                { // Forward
                    ListType list;
                    auto mainPath = list.GetPath<AlternateListMainPath>();
                    auto subPath = list.GetPath<AlternateListSubPath>();

                    list.PushBack(true, TestData1{ 1 });
                    list.PushBack(true, TestData1{ 2 });
                    list.PushBack(true, TestData1{ 3 });
                    EXPECT_EQ(list.GetMainSize(), size_t(3));
                    EXPECT_EQ(list.GetSubSize(), size_t(3));

                    auto it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(2));
                    EXPECT_EQ(list.GetSubSize(), size_t(2));

                    EXPECT_EQ((*mainPath.begin()).value, size_t(2));
                    EXPECT_EQ((*(++mainPath.begin())).value, size_t(3));
                    EXPECT_EQ((++(++mainPath.begin())), mainPath.end());
                    EXPECT_EQ((++(++subPath.begin())), subPath.end());

                    it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(1));
                    EXPECT_EQ(list.GetSubSize(), size_t(1));

                    EXPECT_EQ((*mainPath.begin()).value, size_t(3));
                    EXPECT_EQ(++mainPath.begin(), mainPath.end());
                    EXPECT_EQ(++subPath.begin(), subPath.end());

                    it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(0));
                    EXPECT_EQ(list.GetSubSize(), size_t(0));

                    EXPECT_EQ(mainPath.begin(), mainPath.end());
                    EXPECT_EQ(subPath.begin(), subPath.end());
                }
                { // Backward
                    ListType list;
                    auto mainPath = list.GetPath<AlternateListMainPath>();
                    auto subPath = list.GetPath<AlternateListSubPath>();

                    list.PushBack(true, TestData1{ 1 });
                    list.PushBack(true, TestData1{ 2 });
                    list.PushBack(true, TestData1{ 3 });
                    EXPECT_EQ(list.GetMainSize(), size_t(3));
                    EXPECT_EQ(list.GetSubSize(), size_t(3));

                    auto it = ++(++mainPath.begin());
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(2));
                    EXPECT_EQ(list.GetSubSize(), size_t(2));

                    EXPECT_EQ((*mainPath.begin()).value, size_t(1));
                    EXPECT_EQ((*(++mainPath.begin())).value, size_t(2));
                    EXPECT_EQ((++(++mainPath.begin())), mainPath.end());
                    EXPECT_EQ((++(++subPath.begin())), subPath.end());
                    
                    it = ++mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(1));
                    EXPECT_EQ(list.GetSubSize(), size_t(1));

                    EXPECT_EQ((*mainPath.begin()).value, size_t(1));
                    EXPECT_EQ(++mainPath.begin(), mainPath.end());
                    EXPECT_EQ(++subPath.begin(), subPath.end());

                    it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(0));
                    EXPECT_EQ(list.GetSubSize(), size_t(0));

                    EXPECT_EQ(mainPath.begin(), mainPath.end());
                    EXPECT_EQ(subPath.begin(), subPath.end());
                }
                { // Middle
                    ListType list;
                    auto mainPath = list.GetPath<AlternateListMainPath>();
                    auto subPath = list.GetPath<AlternateListSubPath>();

                    list.PushBack(true, TestData1{ 1 });
                    list.PushBack(true, TestData1{ 2 });
                    list.PushBack(true, TestData1{ 3 });
                    EXPECT_EQ(list.GetMainSize(), size_t(3));
                    EXPECT_EQ(list.GetSubSize(), size_t(3));

                    auto it = ++mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(2));
                    EXPECT_EQ(list.GetSubSize(), size_t(2));

                    EXPECT_EQ((*mainPath.begin()).value, size_t(1));
                    EXPECT_EQ((*(++mainPath.begin())).value, size_t(3));
                    EXPECT_EQ((++(++mainPath.begin())), mainPath.end());
                    EXPECT_EQ((++(++subPath.begin())), subPath.end());

                    it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(1));
                    EXPECT_EQ(list.GetSubSize(), size_t(1));

                    EXPECT_EQ((*mainPath.begin()).value, size_t(3));
                    EXPECT_EQ(++mainPath.begin(), mainPath.end());
                    EXPECT_EQ(++subPath.begin(), subPath.end());

                    it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(0));
                    EXPECT_EQ(list.GetSubSize(), size_t(0));

                    EXPECT_EQ(mainPath.begin(), mainPath.end());
                    EXPECT_EQ(subPath.begin(), subPath.end());
                }
            }
        }
        
    }

    TEST(Utility, AlternateList_EraseSomeSub)
    {
        using ListType = AlternateList<TestData1>;

        {
            { // 1 item.
                ListType list;
                auto mainPath = list.GetPath<AlternateListMainPath>();
                auto subPath = list.GetPath<AlternateListSubPath>();

                list.PushBack(false, TestData1{ 1 });
                EXPECT_EQ(list.GetMainSize(), size_t(1));
                EXPECT_EQ(list.GetSubSize(), size_t(0));

                auto it = mainPath.begin();
                list.Erase(it);

                EXPECT_EQ(list.GetMainSize(), size_t(0));
                EXPECT_EQ(list.GetSubSize(), size_t(0));

                EXPECT_EQ(mainPath.begin(), mainPath.end());
                EXPECT_EQ(subPath.begin(), subPath.end());
            }
            { // 2 items.
                { // 0 subs.
                    ListType list;
                    auto mainPath = list.GetPath<AlternateListMainPath>();
                    auto subPath = list.GetPath<AlternateListSubPath>();

                    list.PushBack(false, TestData1{ 1 });
                    list.PushBack(false, TestData1{ 2 });
                    EXPECT_EQ(list.GetMainSize(), size_t(2));
                    EXPECT_EQ(list.GetSubSize(), size_t(0));

                    auto it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(1));
                    EXPECT_EQ(list.GetSubSize(), size_t(0));

                    EXPECT_EQ(++mainPath.begin(), mainPath.end());
                    EXPECT_EQ(subPath.begin(), subPath.end());

                    it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(0));
                    EXPECT_EQ(list.GetSubSize(), size_t(0));

                    EXPECT_EQ(mainPath.begin(), mainPath.end());
                    EXPECT_EQ(subPath.begin(), subPath.end());
                }
                { // 1 sub.
                    ListType list;
                    auto mainPath = list.GetPath<AlternateListMainPath>();
                    auto subPath = list.GetPath<AlternateListSubPath>();

                    list.PushBack(false, TestData1{ 1 });
                    list.PushBack(true, TestData1{ 2 });
                    EXPECT_EQ(list.GetMainSize(), size_t(2));
                    EXPECT_EQ(list.GetSubSize(), size_t(1));

                    auto it = mainPath.begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(1));
                    EXPECT_EQ(list.GetSubSize(), size_t(1));

                    EXPECT_EQ((*subPath.begin()).value, size_t(2));
                    EXPECT_EQ(++mainPath.begin(), mainPath.end());
                    EXPECT_EQ(++subPath.begin(), subPath.end());

                    it = list.GetPath<AlternateListMainPath>().begin();
                    list.Erase(it);

                    EXPECT_EQ(list.GetMainSize(), size_t(0));
                    EXPECT_EQ(list.GetSubSize(), size_t(0));

                    EXPECT_EQ(mainPath.begin(), mainPath.end());
                    EXPECT_EQ(subPath.begin(), subPath.end());
                }
            }
        }
    }

    TEST(Utility, AlternateList_IteratorTraverse)
    {
        using ListType = AlternateList<TestData1>;

        { // std::next
            ListType list;

            list.PushBack(true, TestData1{ 1 });
            list.PushBack(true, TestData1{ 2 });
            list.PushBack(true, TestData1{ 3 });

            auto mainPath = list.GetPath<AlternateListMainPath>();
            auto it = mainPath.begin();

            auto itNext = it;
            EXPECT_EQ((*itNext).value, size_t(1));

            itNext = std::next(itNext);
            EXPECT_EQ((*itNext).value, size_t(2));

            itNext = std::next(itNext);
            EXPECT_EQ((*itNext).value, size_t(3));

            itNext = std::next(itNext);
            EXPECT_EQ(itNext, mainPath.end());
        }
        { // std::prev
            ListType list;

            list.PushBack(true, TestData1{ 1 });
            list.PushBack(true, TestData1{ 2 });
            list.PushBack(true, TestData1{ 3 });

            auto mainPath = list.GetPath<AlternateListMainPath>();
            auto it = mainPath.end();

            auto itPrev = std::prev(it);
            EXPECT_EQ((*itPrev).value, size_t(3));

            itPrev = std::prev(itPrev);
            EXPECT_EQ((*itPrev).value, size_t(2));

            itPrev = std::prev(itPrev);
            EXPECT_EQ((*itPrev).value, size_t(1));
        }
        
    }

}