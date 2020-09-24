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
#include "Molten/Utility/AlternateTree.hpp"
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
    typename Molten::AlternateTree<TValueType>::template Iterator<TPathType> begin,
    typename Molten::AlternateTree<TValueType>::template Iterator<TPathType> end,
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
    while (it2 != end)
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
    TEST(Utility, AlternateTree_Empty)
    {
        using TreeType = AlternateTree<TestData1>;
        using NodeType = AlternateTreeNode<TestData1>;

        TreeType tree;
        NodeType& root = tree.GetRoot();

        EXPECT_FALSE(root.HasParent());
        EXPECT_EQ(root.GetSize<TreeType::MainPath>(), size_t(0));
        EXPECT_EQ(root.GetSize<TreeType::SubPath>(), size_t(0));
        EXPECT_EQ(root.GetSize<NodeType::MainPath>(), size_t(0));
        EXPECT_EQ(root.GetSize<NodeType::SubPath>(), size_t(0));
        EXPECT_EQ(root.GetMainSize(), size_t(0));
        EXPECT_EQ(root.GetMainSize(), size_t(0));
        EXPECT_EQ(root.GetSubSize(), size_t(0));
    }

    TEST(Utility, AlternateTree_PushBackRoot)
    {
        using TreeType = AlternateTree<TestData1>;
        using NodeType = AlternateTreeNode<TestData1>;

        {
            TreeType tree;
            NodeType& root = tree.GetRoot();

            root.PushBack(true, TestData1{ 1 });
            root.PushBack(false, TestData1{ 2 });
            root.PushBack(true, TestData1{ 3 });
            root.PushBack(false, TestData1{ 4 });
            root.PushBack(true, TestData1{ 5 });
            root.PushBack(true, TestData1{ 6 });
            root.PushBack(false, TestData1{ 7 });
            root.PushBack(false, TestData1{ 8 });
            root.PushBack(true, TestData1{ 9 });

            EXPECT_EQ(root.GetSize<AlternateListMainPath>(), size_t(9));
            EXPECT_EQ(root.GetMainSize(), size_t(9));
            EXPECT_EQ(root.GetSize<AlternateListSubPath>(), size_t(5));
            EXPECT_EQ(root.GetSubSize(), size_t(5));

            if (!CompareListContent<TestData1, AlternateListMainPath>(
                root.GetPath<AlternateListMainPath>().begin(),
                root.GetPath<AlternateListMainPath>().end(),
                { {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9} }))
            {
                ADD_FAILURE();
            }
            
            if (!CompareListContent<TestData1, AlternateListSubPath>(
                root.GetPath<AlternateListSubPath>().begin(),
                root.GetPath<AlternateListSubPath>().end(),
                { {1}, {3}, {5}, {6}, {9} }))
            {
                ADD_FAILURE();
            }

        }
    }

    TEST(Utility, AlternateTree_EraseAllMain)
    {
        using TreeType = AlternateTree<TestData1>;
        using NodeType = AlternateTreeNode<TestData1>;

        {// 1 item.
            {
                TreeType tree;
                NodeType& root = tree.GetRoot();
                auto mainPath = root.GetPath<AlternateListMainPath>();
                auto subPath = root.GetPath<AlternateListSubPath>();

                root.PushBack(true, TestData1{ 1 });
                EXPECT_EQ(root.GetMainSize(), size_t(1));
                EXPECT_EQ(root.GetSubSize(), size_t(1));

                auto it = mainPath.begin();
                tree.Erase(it);

                EXPECT_EQ(root.GetMainSize(), size_t(0));
                EXPECT_EQ(root.GetSubSize(), size_t(0));

                EXPECT_EQ(mainPath.begin(), mainPath.end());
                EXPECT_EQ(subPath.begin(), subPath.end());
            }
            {
                TreeType tree;
                NodeType& root = tree.GetRoot();
                auto mainPath = root.GetPath<AlternateListMainPath>();
                auto subPath = root.GetPath<AlternateListSubPath>();

                root.PushBack(true, TestData1{ 1 });
                EXPECT_EQ(root.GetMainSize(), size_t(1));
                EXPECT_EQ(root.GetSubSize(), size_t(1));

                auto it = mainPath.begin();
                root.Erase(it);

                EXPECT_EQ(root.GetMainSize(), size_t(0));
                EXPECT_EQ(root.GetSubSize(), size_t(0));

                EXPECT_EQ(mainPath.begin(), mainPath.end());
                EXPECT_EQ(subPath.begin(), subPath.end());
            }
        }
        { // 2 items.
            { // Forward
                TreeType tree;
                NodeType& root = tree.GetRoot();
                auto mainPath = root.GetPath<AlternateListMainPath>();
                auto subPath = root.GetPath<AlternateListSubPath>();

                root.PushBack(true, TestData1{ 1 });
                root.PushBack(true, TestData1{ 2 });
                EXPECT_EQ(root.GetMainSize(), size_t(2));
                EXPECT_EQ(root.GetSubSize(), size_t(2));

                auto it = mainPath.begin();
                root.Erase(it);

                EXPECT_EQ(root.GetMainSize(), size_t(1));
                EXPECT_EQ(root.GetSubSize(), size_t(1));

                EXPECT_EQ((*mainPath.begin()).value, size_t(2));
                EXPECT_EQ(++mainPath.begin(), mainPath.end());
                EXPECT_EQ(++subPath.begin(), subPath.end());

                it = mainPath.begin();
                root.Erase(it);

                EXPECT_EQ(root.GetMainSize(), size_t(0));
                EXPECT_EQ(root.GetSubSize(), size_t(0));

                EXPECT_EQ(mainPath.begin(), mainPath.end());
                EXPECT_EQ(subPath.begin(), subPath.end());
            }
            { // Backward.
                TreeType tree;
                NodeType& root = tree.GetRoot();
                auto mainPath = root.GetPath<AlternateListMainPath>();
                auto subPath = root.GetPath<AlternateListSubPath>();

                root.PushBack(true, TestData1{ 1 });
                root.PushBack(true, TestData1{ 2 });
                EXPECT_EQ(root.GetMainSize(), size_t(2));
                EXPECT_EQ(root.GetSubSize(), size_t(2));

                auto it = ++mainPath.begin();
                root.Erase(it);

                EXPECT_EQ(root.GetMainSize(), size_t(1));
                EXPECT_EQ(root.GetSubSize(), size_t(1));

                EXPECT_EQ((*mainPath.begin()).value, size_t(1));
                EXPECT_EQ(++mainPath.begin(), mainPath.end());
                EXPECT_EQ(++subPath.begin(), subPath.end());

                it = mainPath.begin();
                root.Erase(it);

                EXPECT_EQ(root.GetMainSize(), size_t(0));
                EXPECT_EQ(root.GetSubSize(), size_t(0));

                EXPECT_EQ(mainPath.begin(), mainPath.end());
                EXPECT_EQ(subPath.begin(), subPath.end());
            }
        }
    }

    TEST(Utility, AlternateTree_TraverseTreePushBack)
    {
        using TreeType = AlternateTree<TestData1>;
        using NodeType = AlternateTreeNode<TestData1>;

        {
            TreeType tree;
            NodeType& root = tree.GetRoot();
            auto mainPath = root.GetPath<AlternateListMainPath>();
            auto subPath = root.GetPath<AlternateListSubPath>();

            root.PushBack(true, TestData1{ 1 });
            root.PushBack(true, TestData1{ 2 });
            root.PushBack(true, TestData1{ 3 });

        }
    }

}