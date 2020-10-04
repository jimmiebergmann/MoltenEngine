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
#include "Molten/Utility/BypassTree.hpp"
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
    typename Molten::BypassTree<TValueType>::template Iterator<TPathType> begin,
    typename Molten::BypassTree<TValueType>::template Iterator<TPathType> end,
    const std::vector<TValueType>& compareData)
{
    // Post increment.
    size_t i = 0;
    for (auto it = begin; it != end; it++)
    {
        EXPECT_EQ((*it).GetValue(), compareData[i]);

        if (!((*it).GetValue() == compareData[i]))
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
    while (it2 != end)
    {
        EXPECT_EQ((*it2).GetValue(), compareData[i]);

        if (!((*it2).GetValue() == compareData[i]))
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
    using TreeType = BypassTree<TestData1>;
    using NormalLane = TreeType::NormalLane;
    using PartialLane = TreeType::PartialLane;
    using NormalConstLane = TreeType::NormalConstLane;
    using PartialConstLane = TreeType::PartialConstLane;

    TEST(Utility, BypassTree_Empty)
    {
        TreeType tree;
        NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
        PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

        EXPECT_EQ(normalLane.GetSize(), size_t(0));
        EXPECT_EQ(partialLane.GetSize(), size_t(0));
    }
   
    TEST(Utility, BypassTree_PushBackRoot)
    {
        TreeType tree;
        NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
        PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

        partialLane.PushBack(TestData1{ 1 });
        normalLane.PushBack(TestData1{ 2 });
        partialLane.PushBack(TestData1{ 3 });
        normalLane.PushBack(TestData1{ 4 });
        partialLane.PushBack(TestData1{ 5 });
        partialLane.PushBack(TestData1{ 6 });
        normalLane.PushBack(TestData1{ 7 });
        normalLane.PushBack(TestData1{ 8 });
        partialLane.PushBack(TestData1{ 9 });
            
        EXPECT_EQ(normalLane.GetSize(), size_t(9));
        EXPECT_EQ(partialLane.GetSize(), size_t(5));
  

        if (!CompareListContent<TestData1>(
            normalLane.begin(),
            normalLane.end(),
            { {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9} }))
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
    
    TEST(Utility, BypassTree_InsertRoot)
    {     
        TreeType tree;
        NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
        PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();
 
        partialLane.Insert(normalLane.begin(), TestData1{ 1 });
        normalLane.Insert(normalLane.begin(), TestData1{ 2 });
        partialLane.Insert(normalLane.begin(), TestData1{ 3 });
        normalLane.Insert(normalLane.end(), TestData1{ 4 });
        partialLane.Insert(normalLane.begin(), TestData1{ 5 });
        partialLane.Insert(normalLane.end(), TestData1{ 6 });
        normalLane.Insert(normalLane.begin(), TestData1{ 7 });
        normalLane.Insert(normalLane.begin(), TestData1{ 8 });
        partialLane.Insert(normalLane.begin(), TestData1{ 9 });
 
        EXPECT_EQ(normalLane.GetSize(), size_t(9));
        EXPECT_EQ(partialLane.GetSize(), size_t(5));

        if (!CompareListContent<TestData1>(
            normalLane.begin(),
            normalLane.end(),
            { {9}, {8}, {7}, {5}, {3}, {2}, {1}, {4}, {6} }))
        {
            ADD_FAILURE();
        }

        if (!CompareListContent<TestData1>(
            partialLane.begin(),
            partialLane.end(),
            { {9}, {5}, {3}, {1}, {6} }))
        {
            ADD_FAILURE();
        } 
    }

   
    TEST(Utility, BypassTree_EraseAllMain)
    {

        {// 1 item.
            { 
                TreeType tree;
                NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
                PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                auto it = normalLane.begin();
                auto it2 = tree.Erase(it);
                EXPECT_EQ(it2, normalLane.end());

                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
            {
                TreeType tree;
                NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
                PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

                normalLane.PushBack(TestData1{ 1 });
                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                auto it = normalLane.begin();
                auto it2 = tree.Erase(it);
                EXPECT_EQ(it2, normalLane.end());

                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
        }
        { // 2 items.
            { // Forward/*
                TreeType tree;
                NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
                PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                partialLane.PushBack(TestData1{ 2 });
                EXPECT_EQ(normalLane.GetSize(), size_t(2));
                EXPECT_EQ(partialLane.GetSize(), size_t(2));

                auto it = normalLane.begin();
                auto it2 = tree.Erase(it);
                EXPECT_NE(it2, normalLane.end());

                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                EXPECT_EQ((*normalLane.begin()).GetValue(), TestData1{ 2 });
                EXPECT_EQ(++normalLane.begin(), normalLane.end());
                EXPECT_EQ(++partialLane.begin(), partialLane.end());

                it = normalLane.begin();
                auto it3 = tree.Erase(it);
                EXPECT_EQ(it3, normalLane.end());

                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
            { // Backward.
                TreeType tree;
                NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
                PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                partialLane.PushBack(TestData1{ 2 });
                EXPECT_EQ(normalLane.GetSize(), size_t(2));
                EXPECT_EQ(partialLane.GetSize(), size_t(2));

                auto it = ++normalLane.begin();
                auto it2 = tree.Erase(it);
                EXPECT_EQ(it2, normalLane.end());

                EXPECT_EQ(normalLane.GetSize(), size_t(1));
                EXPECT_EQ(partialLane.GetSize(), size_t(1));

                EXPECT_EQ((*normalLane.begin()).GetValue(), TestData1{ 1 });
                EXPECT_EQ(++normalLane.begin(), normalLane.end());
                EXPECT_EQ(++partialLane.begin(), partialLane.end());

                it = normalLane.begin();
                auto it3 = tree.Erase(it);
                EXPECT_EQ(it3, normalLane.end());

                EXPECT_EQ(normalLane.GetSize(), size_t(0));
                EXPECT_EQ(partialLane.GetSize(), size_t(0));

                EXPECT_EQ(normalLane.begin(), normalLane.end());
                EXPECT_EQ(partialLane.begin(), partialLane.end());
            }
        }
    }
  
    TEST(Utility, BypassTree_TraverseTreePushBack)
    { 
        {
            TreeType tree;

            { // Add data 
                PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

                partialLane.PushBack(TestData1{ 1 });
                partialLane.PushBack(TestData1{ 2 });
                partialLane.PushBack(TestData1{ 3 });

                auto it1 = partialLane.begin();
                auto it2 = std::next(it1);
                auto it3 = std::next(it2);
                
                auto& item1 = *it1;
                auto& item3 = *it3;

                PartialLane partialLane1 = item1.GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane3 = item3.GetLane<TreeType::PartialLaneType>();

                partialLane1.PushBack(TestData1{ 11 });
                partialLane1.PushBack(TestData1{ 12 });

                partialLane3.PushBack(TestData1{ 31 });
                partialLane3.PushBack(TestData1{ 32 });
                partialLane3.PushBack(TestData1{ 33 });

                auto it11 = partialLane1.begin();
                auto& item11 = *it11;
                PartialLane partialLane11 = item11.GetLane<TreeType::PartialLaneType>();

                partialLane11.PushBack(TestData1{ 111 });
            }
            { // Check data
                NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
                PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();
                EXPECT_EQ(normalLane.GetSize(), size_t(3));
                EXPECT_EQ(partialLane.GetSize(), size_t(3));

                // First layer
                auto it1 = partialLane.begin();
                auto& item1 = *it1;
                EXPECT_EQ(item1.GetValue(), TestData1{ 1 });
                auto it2 = std::next(it1);
                auto& item2 = *it2;
                EXPECT_EQ(item2.GetValue(), TestData1{ 2 });
                auto it3 = std::next(it2);
                auto& item3 = *it3;
                EXPECT_EQ(item3.GetValue(), TestData1{ 3 });
                
                 // Second layer
                PartialLane partialLane1 = item1.GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane2 = item2.GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane3 = item3.GetLane<TreeType::PartialLaneType>();

                EXPECT_EQ(partialLane1.GetSize(), size_t(2));
                EXPECT_EQ(partialLane2.GetSize(), size_t(0));
                EXPECT_EQ(partialLane3.GetSize(), size_t(3));

                auto it11 = partialLane1.begin();
                auto& item11 = *it11;
                EXPECT_EQ(item11.GetValue(), TestData1{ 11 });
                auto it12 = std::next(it11);
                auto& item12 = *it12;
                EXPECT_EQ(item12.GetValue(), TestData1{ 12 });

                auto it31 = partialLane3.begin();
                auto& item31 = *it31;
                EXPECT_EQ(item31.GetValue(), TestData1{ 31 });
                auto it32 = std::next(it31);
                auto& item32 = *it32;
                EXPECT_EQ(item32.GetValue(), TestData1{ 32 });
                auto it33 = std::next(it32);
                auto& item33 = *it33;
                EXPECT_EQ(item33.GetValue(), TestData1{ 33 });

                PartialLane partialLane11 = item11.GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane12 = item12.GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane31 = item31.GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane32 = item32.GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane33 = item33.GetLane<TreeType::PartialLaneType>();

                EXPECT_EQ(partialLane11.GetSize(), size_t(1));
                EXPECT_EQ(partialLane12.GetSize(), size_t(0));
                EXPECT_EQ(partialLane31.GetSize(), size_t(0));
                EXPECT_EQ(partialLane32.GetSize(), size_t(0));
                EXPECT_EQ(partialLane33.GetSize(), size_t(0));
                
                // Third layer.
                auto it111 = partialLane11.begin();
                auto& item111 = *it111;
                EXPECT_EQ(item111.GetValue(), TestData1{ 111 });

                PartialLane partialLane111 = item111.GetLane<TreeType::PartialLaneType>();
                EXPECT_EQ(partialLane111.GetSize(), size_t(0));
            }
        }
    }
    
    TEST(Utility, BypassTree_ForEach)
    {
        {
            TreeType tree;

            { // Add data
                NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
                PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

                tree.GetItem().GetValue() = TestData1{ 1337 };

                normalLane.PushBack(TestData1{ 1 });
                partialLane.PushBack(TestData1{ 2 });
                partialLane.PushBack(TestData1{ 3 });

                auto it1 = normalLane.begin();
                auto it2 = std::next(it1);
                auto it3 = std::next(it2);

                auto& item1 = *it1;
                auto& item3 = *it3;

                PartialLane partialLane1 = item1.GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane3 = item3.GetLane<TreeType::PartialLaneType>();

                partialLane1.PushBack(TestData1{ 11 });
                partialLane1.PushBack(TestData1{ 12 });

                partialLane3.PushBack(TestData1{ 31 });
                partialLane3.PushBack(TestData1{ 32 });
                partialLane3.PushBack(TestData1{ 33 });

                auto it11 = partialLane1.begin();
                auto& item11 = *it11;
                PartialLane partialLane11 = item11.GetLane<TreeType::PartialLaneType>();

                partialLane11.PushBack(TestData1{ 111 });
            }          
            {
                std::vector<size_t> values;
                tree.ForEachPreorder<TreeType::NormalLaneType>([&](TestData1& data)
                {
                    values.push_back(data.value);
                });

                ASSERT_EQ(values.size(), size_t(10));
                EXPECT_EQ(values[0], size_t(1337));
                EXPECT_EQ(values[1], size_t(1));
                EXPECT_EQ(values[2], size_t(11));
                EXPECT_EQ(values[3], size_t(111));
                EXPECT_EQ(values[4], size_t(12));
                EXPECT_EQ(values[5], size_t(2));
                EXPECT_EQ(values[6], size_t(3));
                EXPECT_EQ(values[7], size_t(31));
                EXPECT_EQ(values[8], size_t(32));
                EXPECT_EQ(values[9], size_t(33));
            }
            {
                std::vector<size_t> values;
                tree.ForEachPreorder<TreeType::PartialLaneType>([&](TestData1& data)
                {
                    values.push_back(data.value);
                });

                ASSERT_EQ(values.size(), size_t(6));
                EXPECT_EQ(values[0], size_t(1337)); 
                EXPECT_EQ(values[1], size_t(2));
                EXPECT_EQ(values[2], size_t(3));
                EXPECT_EQ(values[3], size_t(31));
                EXPECT_EQ(values[4], size_t(32));
                EXPECT_EQ(values[5], size_t(33));
            }
        }
    }

    TEST(Utility, BypassTreeIterator_IsEmpty)
    {
        {
            TreeType::Iterator<TreeType::NormalLaneType> it1;
            EXPECT_TRUE(it1.IsEmpty());

            TreeType::Iterator<TreeType::PartialLaneType> it2;
            EXPECT_TRUE(it2.IsEmpty());

            TreeType::ConstIterator<TreeType::NormalLaneType> it3;
            EXPECT_TRUE(it3.IsEmpty());

            TreeType::ConstIterator<TreeType::PartialLaneType> it4;
            EXPECT_TRUE(it4.IsEmpty());
        }
        {
            TreeType tree;
            TreeType::NormalLane rootLane = tree.GetLane<TreeType::NormalLaneType>();
            EXPECT_FALSE(rootLane.begin().IsEmpty());
            EXPECT_FALSE(rootLane.end().IsEmpty());
            EXPECT_FALSE(rootLane.begin().IsEmpty());
            EXPECT_FALSE(rootLane.end().IsEmpty());
            
            const TreeType& constTree = tree;
            TreeType::NormalConstLane constRootLane = constTree.GetLane<TreeType::NormalLaneType>();
            EXPECT_FALSE(constRootLane.begin().IsEmpty());
            EXPECT_FALSE(constRootLane.end().IsEmpty());
            EXPECT_FALSE(constRootLane.begin().IsEmpty());
            EXPECT_FALSE(constRootLane.end().IsEmpty());
        }
    }

    TEST(Utility, BypassTreeIterator_Copy)
    {
        {
            TreeType tree;
            NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();

            NormalLane::Iterator it = normalLane.begin();

            NormalLane::Iterator it2 = it;
            EXPECT_EQ(it2, it2);

            NormalLane::Iterator it3;
            it3 = it;

            EXPECT_EQ(it3, it3);
        }
        {
            TreeType tree;
            PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

            PartialLane::Iterator it = partialLane.begin();

            PartialLane::Iterator it2 = it;
            EXPECT_EQ(it2, it2);

            PartialLane::Iterator it3;
            it3 = it;

            EXPECT_EQ(it3, it3);
        }
    }

    TEST(Utility, BypassTreeIterator_LaneTypeCast)
    {
        TreeType tree;
        NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
        PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

        NormalLane::Iterator normalIt = normalLane.begin();
        PartialLane::Iterator partialIt = partialLane.begin();

        NormalLane::Iterator normalIt2 = partialIt;
        EXPECT_EQ(normalIt2, normalIt2);

        NormalLane::Iterator normalIt3;
        normalIt3 = partialIt;
        EXPECT_EQ(normalIt3, normalIt3);
    }

    TEST(Utility, BypassTreeIterator_Traverse)
    {
        { // std::next
            TreeType tree;
            NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
            PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

            partialLane.PushBack(TestData1{ 1 });
            normalLane.PushBack(TestData1{ 2 });
            partialLane.PushBack(TestData1{ 3 });

            {
                auto it = normalLane.begin();

                auto itNext = it;
                EXPECT_EQ((*itNext).GetValue(), TestData1{ 1 });

                itNext = std::next(itNext);
                EXPECT_EQ((*itNext).GetValue(), TestData1{ 2 });

                itNext = std::next(itNext);
                EXPECT_EQ((*itNext).GetValue(), TestData1{ 3 });

                itNext = std::next(itNext);
                EXPECT_EQ(itNext, normalLane.end());
            }
            {
                auto it = partialLane.begin();

                auto itNext = it;
                EXPECT_EQ((*itNext).GetValue(), TestData1{ 1 });

                itNext = std::next(itNext);
                EXPECT_EQ((*itNext).GetValue(), TestData1{ 3 });

                itNext = std::next(itNext);
                EXPECT_EQ(itNext, partialLane.end());
            }
        }
        { // std::prev
            TreeType tree;
            NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
            PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

            partialLane.PushBack(TestData1{ 1 });
            normalLane.PushBack(TestData1{ 2 });
            partialLane.PushBack(TestData1{ 3 });

            {
                auto it = normalLane.end();

                auto itPrev = std::prev(it);
                EXPECT_EQ((*itPrev).GetValue(), TestData1{ 3 });

                itPrev = std::prev(itPrev);
                EXPECT_EQ((*itPrev).GetValue(), TestData1{ 2 });

                itPrev = std::prev(itPrev);
                EXPECT_EQ((*itPrev).GetValue(), TestData1{ 1 });
            }
            {
                auto it = partialLane.end();

                auto itPrev = std::prev(it);
                EXPECT_EQ((*itPrev).GetValue(), TestData1{ 3 });

                itPrev = std::prev(itPrev);
                EXPECT_EQ((*itPrev).GetValue(), TestData1{ 1 });
            }

        }
    }

}