///*
//* MIT License
//*
//* Copyright (c) 2020 Jimmie Bergmann
//*
//* Permission is hereby granted, free of charge, to any person obtaining a copy
//* of this software and associated documentation files(the "Software"), to deal
//* in the Software without restriction, including without limitation the rights
//* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//* copies of the Software, and to permit persons to whom the Software is
//* furnished to do so, subject to the following conditions :
//*
//* The above copyright notice and this permission notice shall be included in all
//* copies or substantial portions of the Software.
//*
//* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//* SOFTWARE.
//*
//*/
//
//#include "Test.hpp"
//#include "Molten/Gui/WidgetTree.hpp"
//#include <vector>
//
//template<typename TValueType, typename TLaneType>
//bool CompareListContent(
//    Molten::Gui::WidgetTreeLaneIterator<TLaneType, TValueType> begin,
//    Molten::Gui::WidgetTreeLaneIterator<TLaneType, TValueType> end,
//    const std::vector<TValueType>& compareData)
//{
//    // Post increment.
//    size_t i = 0;
//    for (auto it = begin; it != end; it++)
//    {
//        EXPECT_EQ(it->GetData(), compareData[i]);
//
//        if (it->GetData() != compareData[i])
//        {
//            return false;
//        }
//
//        i++;
//    }
//
//    EXPECT_EQ(i, compareData.size());
//    if (i != compareData.size())
//    {
//        return false;
//    }
//
//    // Pre increment.
//    i = 0;
//    auto it2 = begin;
//    while (it2 != end)
//    {
//        EXPECT_EQ(it2->GetData(), compareData[i]);
//
//        if (it2->GetData() != compareData[i])
//        {
//            return false;
//        }
//
//        ++it2;
//        i++;
//    }
//
//    EXPECT_EQ(i, compareData.size());
//    if (i != compareData.size())
//    {
//        return false;
//    }
//
//    return true;
//}
//
//namespace Molten
//{
//
//    /*TEST(Gui, WidgetTree)
//    {
//    }*/
//
//    TEST(Gui, WidgetTree_NonCopyMovableData)
//    {
//        Gui::WidgetTree<std::unique_ptr<int>> tree;
//
//        auto& lane = tree.GetLane<Gui::WidgetTreeFullLane>();
//        tree.Insert<Gui::WidgetTreePartialLane>(lane.end(), std::make_unique<int>(1));
//    }
//
//    TEST(Gui, WidgetTree_Traversal)
//    {
//        Gui::WidgetTree<int> tree;
//
//        //   P1    |   F2   |        P3
//        // F4  P5  | P6  P7 | F8 P9 P10 P11 F12
//
//        auto& rootFullLane = tree.GetLane<Gui::WidgetTreeFullLane>();
//        auto& rootPartialLane = tree.GetLane<Gui::WidgetTreePartialLane>();
//        auto it1 = tree.Insert<Gui::WidgetTreePartialLane>(rootPartialLane.end(), 1);
//        auto it2 = tree.Insert<Gui::WidgetTreeFullLane>(rootPartialLane.end(), 2);
//        auto it3 = tree.Insert<Gui::WidgetTreePartialLane>(rootPartialLane.end(), 3);
//
//        auto& fullLane1 = it1->GetLane<Gui::WidgetTreeFullLane>();
//        auto& partialLane1 = it1->GetLane<Gui::WidgetTreePartialLane>();
//        tree.Insert<Gui::WidgetTreeFullLane>(partialLane1.end(), 4);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane1.end(), 5);
//
//        auto& fullLane2 = it2->GetLane<Gui::WidgetTreeFullLane>();
//        auto& partialLane2 = it2->GetLane<Gui::WidgetTreePartialLane>();
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane2.end(), 6);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane2.end(), 7);
//
//        auto& fullLane3 = it3->GetLane<Gui::WidgetTreeFullLane>();
//        auto& partialLane3 = it3->GetLane<Gui::WidgetTreePartialLane>();
//        tree.Insert<Gui::WidgetTreeFullLane>(partialLane3.end(), 8);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane3.end(), 9);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane3.end(), 10);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane3.end(), 11);
//        tree.Insert<Gui::WidgetTreeFullLane>(partialLane3.end(), 12);
//
//        // Root lanes.
//        {
//            if (!CompareListContent<int>(
//                rootFullLane.begin(),
//                rootFullLane.end(),
//                { 1, 2, 3 }))
//            {
//                ADD_FAILURE();
//            }
//            if (!CompareListContent<int>(
//                rootPartialLane.begin(),
//                rootPartialLane.end(),
//                { 1, 3 }))
//            {
//                ADD_FAILURE();
//            }
//        }
//        // 1:st lane in root.
//        {
//            if (!CompareListContent<int>(
//                fullLane1.begin(),
//                fullLane1.end(),
//                { 4, 5 }))
//            {
//                ADD_FAILURE();
//            }
//            if (!CompareListContent<int>(
//                partialLane1.begin(),
//                partialLane1.end(),
//                { 5 }))
//            {
//                ADD_FAILURE();
//            }
//        }
//        // 2:nd lane in root.
//        {
//            if (!CompareListContent<int>(
//                fullLane2.begin(),
//                fullLane2.end(),
//                { 6, 7 }))
//            {
//                ADD_FAILURE();
//            }
//            if (!CompareListContent<int>(
//                partialLane2.begin(),
//                partialLane2.end(),
//                { 6, 7 }))
//            {
//                ADD_FAILURE();
//            }
//        }
//        // 3:rd lane in root.
//        {
//            if (!CompareListContent<int>(
//                fullLane3.begin(),
//                fullLane3.end(),
//                { 8, 9, 10, 11, 12 }))
//            {
//                ADD_FAILURE();
//            }
//            if (!CompareListContent<int>(
//                partialLane3.begin(),
//                partialLane3.end(),
//                { 9, 10, 11 }))
//            {
//                ADD_FAILURE();
//            }
//        }
//
//        // ForEachPreorder(callback)
//        {
//            size_t index = 0;
//            std::vector<int> fullData = { 1, 4, 5, 2, 6, 7, 3, 8, 9, 10, 11, 12 };
//
//            tree.ForEachPreorder<Gui::WidgetTreeFullLane>([&](auto& data)
//            {
//                EXPECT_EQ(data, fullData[index++]);
//            });
//
//            EXPECT_EQ(index, size_t{ 12 });
//        }
//        {
//            size_t index = 0;
//            std::vector<int> partialData = { 1, 5, 3, 9, 10, 11 };
//
//            tree.ForEachPreorder<Gui::WidgetTreePartialLane>([&](auto& data)
//            {
//                EXPECT_EQ(data, partialData[index++]);
//            });
//
//            EXPECT_EQ(index, size_t{ 6 });
//        }
//        
//        // ForEachPreorder(preCallback, postCallback)
//        {
//            size_t preIndex = 0;
//            size_t postIndex = 0;
//            std::vector<int> preFullData = { 1, 4, 5, 2, 6, 7, 3, 8, 9, 10, 11, 12 };
//            std::vector<int> postFullData = { 4, 5, 1, 6, 7, 2, 8, 9, 10, 11, 12, 3 };
//
//            tree.ForEachPreorder<Gui::WidgetTreeFullLane>(
//                [&](auto& data)
//            {
//                EXPECT_EQ(data, preFullData[preIndex++]);
//            },
//                [&](auto& data)
//            {
//                EXPECT_EQ(data, postFullData[postIndex++]);
//            });
//
//            EXPECT_EQ(preIndex, size_t{ 12 });
//            EXPECT_EQ(postIndex, size_t{ 12 });
//        }
//        {
//            size_t preIndex = 0;
//            size_t postIndex = 0;
//            std::vector<int> prePartialData = { 1, 5, 3, 9, 10, 11 };
//            std::vector<int> postPartialData = { 5, 1, 9, 10, 11, 3 };
//
//            tree.ForEachPreorder<Gui::WidgetTreePartialLane>(
//                [&](auto& data)
//            {
//                EXPECT_EQ(data, prePartialData[preIndex++]);
//            },
//                [&](auto& data)
//            {
//                EXPECT_EQ(data, postPartialData[postIndex++]);
//            });
//
//            EXPECT_EQ(preIndex, size_t{ 6 });
//            EXPECT_EQ(postIndex, size_t{ 6 });
//        }
//    }
//
//    TEST(Gui, WidgetTree_Erase)
//    {
//        Gui::WidgetTree<int> tree;
//
//        //   P1    |   F2   |        P3
//        // F4  P5  | P6  P7 | F8 P9 P10 P11 F12
//
//        auto& rootPartialLane = tree.GetLane<Gui::WidgetTreePartialLane>();
//        auto it1 = tree.Insert<Gui::WidgetTreePartialLane>(rootPartialLane.end(), 1);
//        auto it2 = tree.Insert<Gui::WidgetTreeFullLane>(rootPartialLane.end(), 2);
//        auto it3 = tree.Insert<Gui::WidgetTreePartialLane>(rootPartialLane.end(), 3);
//
//        //auto& fullLane1 = it1->GetLane<Gui::WidgetTreeFullLane>();
//        auto& partialLane1 = it1->GetLane<Gui::WidgetTreePartialLane>();
//        auto it11 = tree.Insert<Gui::WidgetTreeFullLane>(partialLane1.end(), 4);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane1.end(), 5);
//
//        //auto& fullLane2 = it2->GetLane<Gui::WidgetTreeFullLane>();
//        auto& partialLane2 = it2->GetLane<Gui::WidgetTreePartialLane>();
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane2.end(), 6);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane2.end(), 7);
//
//        //auto& fullLane3 = it3->GetLane<Gui::WidgetTreeFullLane>();
//        auto& partialLane3 = it3->GetLane<Gui::WidgetTreePartialLane>();
//        tree.Insert<Gui::WidgetTreeFullLane>(partialLane3.end(), 8);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane3.end(), 9);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane3.end(), 10);
//        tree.Insert<Gui::WidgetTreePartialLane>(partialLane3.end(), 11);
//        tree.Insert<Gui::WidgetTreeFullLane>(partialLane3.end(), 12);
//
//        tree.Erase(it11);
//    }
//
//}