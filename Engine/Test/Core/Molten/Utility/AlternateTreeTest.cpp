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

    TEST(Utility, AlternateTreeIterator_IsEmpty)
    {
        using TreeType = AlternateTree<TestData1>;
        using NodeType = AlternateTreeNode<TestData1>;

        {
            TreeType::Iterator<TreeType::MainPath> it1;
            EXPECT_TRUE(it1.IsEmpty());

            TreeType::Iterator<TreeType::SubPath> it2;
            EXPECT_TRUE(it2.IsEmpty());

            TreeType::ConstIterator<TreeType::MainPath> it3;
            EXPECT_TRUE(it3.IsEmpty());

            TreeType::ConstIterator<TreeType::SubPath> it4;
            EXPECT_TRUE(it4.IsEmpty());
        }
        {
            TreeType tree;
            NodeType& node = tree.GetRoot();
            TreeType::IteratorPath<TreeType::MainPath> path = node.GetMainPath();
            EXPECT_FALSE(path.begin().IsEmpty());
            EXPECT_FALSE(path.end().IsEmpty());
            EXPECT_FALSE(path.begin().IsEmpty());
            EXPECT_FALSE(path.end().IsEmpty());

            const TreeType& constList = tree;
            const NodeType& constNode = constList.GetRoot();
            TreeType::ConstIteratorPath<TreeType::MainPath> constPath = constNode.GetMainPath();
            EXPECT_FALSE(constPath.begin().IsEmpty());
            EXPECT_FALSE(constPath.end().IsEmpty());
            EXPECT_FALSE(constPath.begin().IsEmpty());
            EXPECT_FALSE(constPath.end().IsEmpty());
        }
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

                EXPECT_EQ((*mainPath.begin()).GetValue(), TestData1{ 2 });
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

                EXPECT_EQ((*mainPath.begin()).GetValue(), TestData1{ 1 });
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

            { // Add data
                NodeType& root = tree.GetRoot();
                auto path = root.GetPath<AlternateListMainPath>();

                root.PushBack(true, TestData1{ 1 });
                root.PushBack(true, TestData1{ 2 });
                root.PushBack(true, TestData1{ 3 });

                auto it1 = path.begin();
                auto it2 = std::next(it1);
                auto it3 = std::next(it2);

                auto& node1 = *it1;
                auto& node3 = *it3;

                node1.PushBack(true, TestData1{ 11 });
                node1.PushBack(true, TestData1{ 12 });

                node3.PushBack(true, TestData1{ 31 });
                node3.PushBack(true, TestData1{ 32 });
                node3.PushBack(true, TestData1{ 33 });

                auto path1 = node1.GetPath<AlternateListMainPath>();
                auto it11 = path1.begin();
                auto& node11 = *it11;
                node11.PushBack(true, TestData1{ 111 });

                EXPECT_TRUE(true);
            }
            { // Check data
                NodeType& root = tree.GetRoot();
                EXPECT_EQ(root.GetMainSize(), size_t(3));
                EXPECT_EQ(root.GetSubSize(), size_t(3));

                auto path = root.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path.GetSize(), size_t(3));

                 // First layer
                auto it1 = path.begin();
                auto& node1 = *it1;
                EXPECT_EQ(node1.GetValue(), TestData1{ 1 });
                auto it2 = std::next(it1);
                auto& node2 = *it2;
                EXPECT_EQ(node2.GetValue(), TestData1{ 2 });
                auto it3 = std::next(it2);
                auto& node3 = *it3;
                EXPECT_EQ(node3.GetValue(), TestData1{ 3 });
                
                 // Second layer
                auto path1 = node1.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path1.GetSize(), size_t(2));
                auto path2 = node2.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path2.GetSize(), size_t(0));
                auto path3 = node3.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path3.GetSize(), size_t(3));

                auto it11 = path1.begin();
                auto& node11 = *it11;
                EXPECT_EQ(node11.GetValue(), TestData1{ 11 });
                auto it12 = std::next(it11);
                auto& node12 = *it12;
                EXPECT_EQ(node12.GetValue(), TestData1{ 12 });

                auto it31 = path3.begin();
                auto& node31 = *it31;
                EXPECT_EQ(node31.GetValue(), TestData1{ 31 });
                auto it32 = std::next(it31);
                auto& node32 = *it32;
                EXPECT_EQ(node32.GetValue(), TestData1{ 32 });
                auto it33 = std::next(it32);
                auto& node33 = *it33;
                EXPECT_EQ(node33.GetValue(), TestData1{ 33 });

                auto path11 = node11.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path11.GetSize(), size_t(1));
                auto path12 = node12.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path12.GetSize(), size_t(0));

                auto path31 = node31.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path31.GetSize(), size_t(0));
                auto path32 = node32.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path32.GetSize(), size_t(0));
                auto path33 = node33.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path33.GetSize(), size_t(0));

                // Third layer.
                auto it111 = path11.begin();
                auto& node111 = *it111;
                EXPECT_EQ(node111.GetValue(), TestData1{ 111 });

                auto path111 = node111.GetPath<AlternateListMainPath>();
                EXPECT_EQ(path111.GetSize(), size_t(0));
            }
        }
    }
    
}