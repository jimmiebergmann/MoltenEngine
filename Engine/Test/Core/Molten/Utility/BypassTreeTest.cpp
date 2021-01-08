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

template<typename TBegin, typename TEnd>
size_t CountIteratorToEnd(TBegin begin, TEnd end)
{
    size_t count = 0;
    for (auto it = begin; it != end; it++)
    {
        ++count;
    }
    return count;
};

namespace Molten
{
    using TreeType = BypassTree<TestData1>;
    using NormalLane = TreeType::NormalLane;
    using NormalConstLane = TreeType::NormalConstLane;
    using NormalIterator = TreeType::Iterator<TreeType::NormalLaneType>;
    using NormalConstIterator = TreeType::ConstIterator<TreeType::NormalLaneType>;
    using PartialLane = TreeType::PartialLane;
    using PartialConstLane = TreeType::PartialConstLane;
    using PartialIterator = TreeType::Iterator<TreeType::PartialLaneType>;
    using PartialConstIterator = TreeType::ConstIterator<TreeType::PartialLaneType>;
    
    TEST(Utility, BypassTree_Empty)
    {
        TreeType tree;
        NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
        PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

        EXPECT_EQ(normalLane.GetSize(), size_t(0));
        EXPECT_EQ(partialLane.GetSize(), size_t(0));
    }
   
    TEST(Utility, BypassTree_InsertRootBack)
    {
        TreeType tree;
        NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
        PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

        tree.Insert(partialLane, normalLane.end(), TestData1{ 1 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 2 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 3 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 4 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 5 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 6 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 7 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 8 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 9 });
            
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
 
     
        tree.Insert(partialLane, normalLane.begin(), TestData1{ 1 });
        tree.Insert(normalLane, normalLane.begin(), TestData1{ 2 });
        tree.Insert(partialLane, normalLane.begin(), TestData1{ 3 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 4 });
        tree.Insert(partialLane, normalLane.begin(), TestData1{ 5 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 6 });
        tree.Insert(normalLane, normalLane.begin(), TestData1{ 7 });
        tree.Insert(normalLane, normalLane.begin(), TestData1{ 8 });
        tree.Insert(partialLane, normalLane.begin(), TestData1{ 9 });
 
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

    TEST(Utility, BypassTree_EraseAllNormal)
    {
        {// 1 item.
            { 
                TreeType tree;
                NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
                PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

                auto value1 = (*tree.Insert(partialLane, normalLane.end(), TestData1{ 1 })).GetValue().value;
                EXPECT_EQ(value1, size_t{ 1 });

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

                auto value1 = (*tree.Insert(normalLane, normalLane.end(), TestData1{ 1 })).GetValue().value;
                EXPECT_EQ(value1, size_t{ 1 });

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

                auto value1 = (*tree.Insert(partialLane, normalLane.end(), TestData1{ 1 })).GetValue().value;
                auto value2 = (*tree.Insert(partialLane, normalLane.end(), TestData1{ 2 })).GetValue().value;
                EXPECT_EQ(value1, size_t{ 1 });
                EXPECT_EQ(value2, size_t{ 2 });


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

                auto value1 = (*tree.Insert(partialLane, normalLane.end(), TestData1{ 1 })).GetValue().value;
                auto value2 = (*tree.Insert(partialLane, normalLane.end(), TestData1{ 2 })).GetValue().value;
                EXPECT_EQ(value1, size_t{ 1 });
                EXPECT_EQ(value2, size_t{ 2 });

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

                tree.Insert(partialLane, partialLane.end(), TestData1{ 1 });
                tree.Insert(partialLane, partialLane.end(), TestData1{ 2 });
                tree.Insert(partialLane, partialLane.end(), TestData1{ 3 });

                auto it1 = partialLane.begin();
                auto it2 = std::next(it1);
                auto it3 = std::next(it2);
                
                auto& item1 = *it1;
                auto& item3 = *it3;

                PartialLane partialLane1 = item1.GetChildren().GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane3 = item3.GetChildren().GetLane<TreeType::PartialLaneType>();

                tree.Insert(partialLane1, partialLane1.end(), TestData1{ 11 });
                tree.Insert(partialLane1, partialLane1.end(), TestData1{ 12 });

                tree.Insert(partialLane3, partialLane3.end(), TestData1{ 31 });
                tree.Insert(partialLane3, partialLane3.end(), TestData1{ 32 });
                tree.Insert(partialLane3, partialLane3.end(), TestData1{ 33 });

                auto it11 = partialLane1.begin();
                auto& item11 = *it11;
                PartialLane partialLane11 = item11.GetChildren().GetLane<TreeType::PartialLaneType>();

                tree.Insert(partialLane11, partialLane11.end(), TestData1{ 111 });
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
                PartialLane partialLane1 = item1.GetChildren().GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane2 = item2.GetChildren().GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane3 = item3.GetChildren().GetLane<TreeType::PartialLaneType>();

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

                PartialLane partialLane11 = item11.GetChildren().GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane12 = item12.GetChildren().GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane31 = item31.GetChildren().GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane32 = item32.GetChildren().GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane33 = item33.GetChildren().GetLane<TreeType::PartialLaneType>();

                EXPECT_EQ(partialLane11.GetSize(), size_t(1));
                EXPECT_EQ(partialLane12.GetSize(), size_t(0));
                EXPECT_EQ(partialLane31.GetSize(), size_t(0));
                EXPECT_EQ(partialLane32.GetSize(), size_t(0));
                EXPECT_EQ(partialLane33.GetSize(), size_t(0));
                
                // Third layer.
                auto it111 = partialLane11.begin();
                auto& item111 = *it111;
                EXPECT_EQ(item111.GetValue(), TestData1{ 111 });

                PartialLane partialLane111 = item111.GetChildren().GetLane<TreeType::PartialLaneType>();
                EXPECT_EQ(partialLane111.GetSize(), size_t(0));
            }
        }
    }
    
    TEST(Utility, BypassTree_ForEachPreorder)
    {
        {
            TreeType tree;

            { // Add data
                NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
                PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

                tree.Insert(normalLane, normalLane.end(), TestData1{ 1 });
                tree.Insert(partialLane, partialLane.end(), TestData1{ 2 });
                tree.Insert(partialLane, partialLane.end(), TestData1{ 3 });

                auto it1 = normalLane.begin();
                auto it2 = std::next(it1);
                auto it3 = std::next(it2);

                auto& item1 = *it1;
                auto& item3 = *it3;

                PartialLane partialLane1 = item1.GetChildren().GetLane<TreeType::PartialLaneType>();
                PartialLane partialLane3 = item3.GetChildren().GetLane<TreeType::PartialLaneType>();

                tree.Insert(partialLane1, partialLane1.end(), TestData1{ 11 });
                tree.Insert(partialLane1, partialLane1.end(), TestData1{ 12 });

                tree.Insert(partialLane3, partialLane3.end(), TestData1{ 31 });
                tree.Insert(partialLane3, partialLane3.end(), TestData1{ 32 });
                tree.Insert(partialLane3, partialLane3.end(), TestData1{ 33 });

                auto it11 = partialLane1.begin();
                auto& item11 = *it11;
                PartialLane partialLane11 = item11.GetChildren().GetLane<TreeType::PartialLaneType>();

                tree.Insert(partialLane11, partialLane11.end(), TestData1{ 111 });
            }
            { // Forward
                { // Pre
                    std::vector<size_t> values = {
                        1, 11, 111, 12, 2, 3, 31, 32, 33
                    };

                    auto it = values.begin();

                    tree.ForEachPreorder<TreeType::NormalLaneType>([&](TestData1& item)
                    {
                        ASSERT_NE(it, values.end());
                        EXPECT_EQ(item.value, *it);
                        ++it;
                    });

                    EXPECT_EQ(it, values.end());
                }
                { // Pre and post.
                    std::vector<size_t> preValues = {
                        1, 11, 111, 12, 2, 3, 31, 32, 33
                    };
                    std::vector<size_t> postValues = {
                        111, 11, 12, 1, 2, 31, 32, 33, 3
                    };

                    auto preIt = preValues.begin();
                    auto postIt = postValues.begin();

                    tree.ForEachPreorder<TreeType::NormalLaneType>(
                        [&](auto& item)
                    {
                        ASSERT_NE(preIt, preValues.end());
                        EXPECT_EQ(item.value, *preIt);
                        ++preIt;
                    },
                        [&](auto& item)
                    {
                        ASSERT_NE(postIt, postValues.end());
                        EXPECT_EQ(item.value, *postIt);
                        ++postIt;
                    });

                    EXPECT_EQ(preIt, preValues.end());
                    EXPECT_EQ(postIt, postValues.end());
                }
                { // Pre
                    std::vector<size_t> values = {
                        2, 3, 31, 32, 33
                    };

                    auto it = values.begin();

                    tree.ForEachPreorder<TreeType::PartialLaneType>([&](TestData1& item)
                    {
                        ASSERT_NE(it, values.end());
                        EXPECT_EQ(item.value, *it);
                        ++it;
                    });

                    EXPECT_EQ(it, values.end());
                }
                { // Pre and post.
                    std::vector<size_t> preValues = {
                        2, 3, 31, 32, 33
                    };
                    std::vector<size_t> postValues = {
                        2, 31, 32, 33, 3
                    };

                    auto preIt = preValues.begin();
                    auto postIt = postValues.begin();

                    tree.ForEachPreorder<TreeType::PartialLaneType>(
                        [&](auto& item)
                    {
                        ASSERT_NE(preIt, preValues.end());
                        EXPECT_EQ(item.value, *preIt);
                        ++preIt;
                    },
                        [&](auto& item)
                    {
                        ASSERT_NE(postIt, postValues.end());
                        EXPECT_EQ(item.value, *postIt);
                        ++postIt;
                    });

                    EXPECT_EQ(preIt, preValues.end());
                    EXPECT_EQ(postIt, postValues.end());
                }
            }
            { // Reverse
                { // Pre
                    std::vector<size_t> values = {
                        33, 32, 31, 3, 2, 12, 111, 11, 1
                    };

                    auto it = values.begin();

                    tree.ForEachReversePreorder<TreeType::NormalLaneType>([&](TestData1& item)
                    {
                        ASSERT_NE(it, values.end());
                        EXPECT_EQ(item.value, *it);
                        ++it;
                    });

                    EXPECT_EQ(it, values.end());
                }

            }
        }
    }

    TEST(Utility, BypassTreeLane_LaneCopy)
    {
        TreeType tree;
        NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
        PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

        tree.Insert(normalLane, normalLane.end(), TestData1{ 1 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 2 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 3 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 4 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 5 });

        // Non-const -> Non-const.
        NormalLane normalLane2;
        normalLane2 = normalLane;
        PartialLane partialLane2;
        partialLane2 = partialLane;

        EXPECT_EQ(normalLane2.GetSize(), size_t(5));
        EXPECT_EQ(partialLane2.GetSize(), size_t(2));

        // non-const -> const
        NormalConstLane normalConstLane2 = normalLane2;
        PartialConstLane partialConstLane2 = partialLane2;

        EXPECT_EQ(normalConstLane2.GetSize(), size_t(5));
        EXPECT_EQ(partialConstLane2.GetSize(), size_t(2));

        NormalConstLane normalConstLane4;
        normalConstLane4 = normalLane;
        PartialConstLane partialConstLane4;
        partialConstLane4 = partialLane;
        EXPECT_EQ(normalConstLane4.GetSize(), size_t(5));
        EXPECT_EQ(partialConstLane4.GetSize(), size_t(2));

        // const -> const
        NormalConstLane normalConstLane3 = normalConstLane2;
        PartialConstLane partialConstLane3 = partialConstLane2;

        EXPECT_EQ(normalConstLane3.GetSize(), size_t(5));
        EXPECT_EQ(partialConstLane3.GetSize(), size_t(2));

        NormalConstLane normalConstLane5;
        normalConstLane5 = normalConstLane4;
        PartialConstLane partialConstLane5;
        partialConstLane5 = partialConstLane4;

        EXPECT_EQ(normalConstLane5.GetSize(), size_t(5));
        EXPECT_EQ(partialConstLane5.GetSize(), size_t(2));
    }

    TEST(Utility, BypassTreeIterator_IsValid)
    {
        {
            TreeType::Iterator<TreeType::NormalLaneType> it1;
            EXPECT_TRUE(it1.IsValid());

            TreeType::Iterator<TreeType::PartialLaneType> it2;
            EXPECT_TRUE(it2.IsValid());

            TreeType::ConstIterator<TreeType::NormalLaneType> it3;
            EXPECT_TRUE(it3.IsValid());

            TreeType::ConstIterator<TreeType::PartialLaneType> it4;
            EXPECT_TRUE(it4.IsValid());
        }
        {
            TreeType tree;
            TreeType::NormalLane rootLane = tree.GetLane<TreeType::NormalLaneType>();
            EXPECT_FALSE(rootLane.begin().IsValid());
            EXPECT_FALSE(rootLane.end().IsValid());
            EXPECT_FALSE(rootLane.begin().IsValid());
            EXPECT_FALSE(rootLane.end().IsValid());
            
            const TreeType& constTree = tree;
            TreeType::NormalConstLane constRootLane = constTree.GetLane<TreeType::NormalLaneType>();
            EXPECT_FALSE(constRootLane.begin().IsValid());
            EXPECT_FALSE(constRootLane.end().IsValid());
            EXPECT_FALSE(constRootLane.begin().IsValid());
            EXPECT_FALSE(constRootLane.end().IsValid());
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

    TEST(Utility, BypassTreeIterator_LaneCopy)
    {
        TreeType tree;
        NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
        PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

        tree.Insert(normalLane, normalLane.end(), TestData1{ 1 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 2 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 3 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 4 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 5 });

        // Non-const -> Non-const.
        NormalIterator normalIt = normalLane.begin();
        PartialIterator partialIt = partialLane.begin();

        EXPECT_EQ(CountIteratorToEnd(normalIt, normalLane.end()), size_t(5));
        EXPECT_EQ(CountIteratorToEnd(partialIt, partialLane.end()), size_t(2));
        
        NormalIterator normalIt2;
        normalIt2 = normalIt;
        PartialIterator partialIt2;
        partialIt2 = partialIt;

        EXPECT_EQ(CountIteratorToEnd(normalIt2, normalLane.end()), size_t(5));
        EXPECT_EQ(CountIteratorToEnd(partialIt2, partialLane.end()), size_t(2));

        // non-const -> const
        NormalConstIterator normalConstIt2 = normalIt;
        PartialConstIterator partialConstIt2 = partialIt;

        EXPECT_EQ(CountIteratorToEnd(normalConstIt2, normalLane.end()), size_t(5));
        EXPECT_EQ(CountIteratorToEnd(partialConstIt2, partialLane.end()), size_t(2));

        NormalConstIterator normalConstIt3;
        normalConstIt3 = normalIt;
        PartialConstIterator partialConstIt3;
        partialConstIt3 = partialIt;

        EXPECT_EQ(CountIteratorToEnd(normalConstIt3, normalLane.end()), size_t(5));
        EXPECT_EQ(CountIteratorToEnd(partialConstIt3, partialLane.end()), size_t(2));


        // const -> const
        NormalConstIterator normalConstIt4 = normalConstIt3;
        PartialConstIterator partialConstIt4 = partialConstIt3;

        EXPECT_EQ(CountIteratorToEnd(normalConstIt4, normalLane.end()), size_t(5));
        EXPECT_EQ(CountIteratorToEnd(partialConstIt4, partialLane.end()), size_t(2));

        NormalConstIterator normalConstIt5;
        normalConstIt5 = normalConstIt3;
        PartialConstIterator partialConstIt5;
        partialConstIt5 = partialConstIt3;

        EXPECT_EQ(CountIteratorToEnd(normalConstIt5, normalLane.end()), size_t(5));
        EXPECT_EQ(CountIteratorToEnd(partialConstIt5, partialLane.end()), size_t(2));
    }

    TEST(Utility, BypassTreeIterator_Traverse)
    {
        TreeType tree;
        NormalLane normalLane = tree.GetLane<TreeType::NormalLaneType>();
        PartialLane partialLane = tree.GetLane<TreeType::PartialLaneType>();

        tree.Insert(partialLane, normalLane.end(), TestData1{ 1 });
        tree.Insert(normalLane, normalLane.end(), TestData1{ 2 });
        tree.Insert(partialLane, normalLane.end(), TestData1{ 3 });

        { // std::next
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