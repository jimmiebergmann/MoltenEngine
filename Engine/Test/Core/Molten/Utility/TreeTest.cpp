/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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
#include "Molten/Utility/Tree.hpp"
#include <array>

namespace Molten
{
    TEST(Utility, Tree)
    {
        Tree<int> tree;

        // |   P1     |   F2    |        P3                       |
        // | F4 | P5  | P6 | P7 | F8 | P9 |    P10    | P11 | F12 |
        // |    | P13 |         |         | P14 | P15 |           |

        // Insert
        auto it2 = tree.Insert(tree.end(), 2);
        auto it1 = tree.Insert(tree.begin(), 1);
        auto it3 = tree.Insert(tree.end(), 3);

        tree.Insert(it1->end(), 4);
        auto it12 = tree.Insert(it1->end(), 5);
        tree.Insert(it12->end(), 13);

        tree.Insert(it2->begin(), 7);
        tree.Insert(it2->begin(), 6);

        tree.Insert(it3->end(), 9);
        const auto it34 = tree.Insert(it3->end(), 11);
        auto it33 = tree.Insert(it34, 10);
        tree.Insert(it33->end(), 14);
        tree.Insert(it33->end(), 15);

        tree.Insert(it3->end(), 12);
        tree.Insert(it3->begin(), 8);

        // Iterate and check values.
        {
            const std::array<int, 3> rootValues = { 1, 2, 3 };

            const std::array<std::vector<int>, 3> leafValues = {
                std::vector<int>{ 4, 5 },
                std::vector<int>{ 6, 7 },
                std::vector<int>{ 8, 9, 10, 11, 12 }
            };

            size_t rootIndex = 0;
            for (auto& node1 : tree)
            {
                ASSERT_EQ(node1.GetData(), rootValues[rootIndex]);

                size_t leafIndex = 0;
                for (auto& node2 : node1)
                {
                    ASSERT_EQ(node2.GetData(), leafValues[rootIndex][leafIndex]);
                    ++leafIndex;
                }

                ASSERT_EQ(leafIndex, leafValues[rootIndex].size());
                ++rootIndex;
            }

            EXPECT_EQ(rootIndex, size_t{ 3 });
        }


        // |   P1     |   F2    |        P3                       |
        // | F4 | P5  | P6 | P7 | F8 | P9 |    P10    | P11 | F12 |
        // |    | P13 |         |         | P14 | P15 |           |

        // ForEach
        {
            std::vector<int> preValues = { 1, 4, 5, 13, 2, 6, 7, 3, 8, 9, 10, 14, 15, 11, 12 };
            std::vector<int> postValues = { 4, 13, 5, 1, 6, 7, 2, 8, 9, 14, 15, 10, 11, 12, 3 };
            std::vector<std::pair<int, int>> preChildValues = { { 1, 4 }, { 1, 5 }, { 5, 13 }, { 2, 6 }, { 2, 7 }, { 3, 8 }, { 3, 9 }, { 3, 10 }, { 10, 14 }, { 10, 15 }, { 3, 11 }, { 3, 12 } };
            std::vector<std::pair<int, int>> postChildValues = { { 1, 4 }, { 5, 13 }, { 1, 5 }, { 2, 6 }, { 2, 7 }, { 3, 8 }, { 3, 9 }, { 10, 14 }, { 10, 15 }, { 3, 10 }, { 3, 11 }, { 3, 12 } };

            size_t preIndex = 0;
            size_t postIndex = 0;
            size_t preChildIndex = 0;
            size_t postChildIndex = 0;

            tree.GetChildren().ForEachPreorder(
                [&](auto& node)
            {
                ASSERT_LT(preIndex, preValues.size());
                EXPECT_EQ(node.GetData(), preValues[preIndex]);
                ++preIndex;
            },
                [&](auto& node)
            {
                ASSERT_LT(postIndex, postValues.size());
                EXPECT_EQ(node.GetData(), postValues[postIndex]);
                ++postIndex;
            },
                [&](auto& parent, auto& child)
            {
                ASSERT_LT(preChildIndex, preChildValues.size());
                EXPECT_EQ(parent.GetData(), preChildValues[preChildIndex].first);
                EXPECT_EQ(child.GetData(), preChildValues[preChildIndex].second);
                ++preChildIndex;
            },
                [&](auto& parent, auto& child)
            {
                ASSERT_LT(postChildIndex, postChildValues.size());
                EXPECT_EQ(parent.GetData(), postChildValues[postChildIndex].first);
                EXPECT_EQ(child.GetData(), postChildValues[postChildIndex].second);
                ++postChildIndex;
            });

            EXPECT_EQ(preIndex, preValues.size());
            EXPECT_EQ(postIndex, postValues.size());
            EXPECT_EQ(preChildIndex, preChildValues.size());
            EXPECT_EQ(postChildIndex, postChildValues.size());
        }

        // Erase
        {
            // root 2
            {
                const auto eIt2 = tree.Erase(it2);
                EXPECT_EQ(eIt2, it3);

                const std::array<int, 3> rootValues = { 1, 3 };

                size_t rootIndex = 0;
                for (auto& node1 : tree)
                {
                    ASSERT_EQ(node1.GetData(), rootValues[rootIndex]);
                    ++rootIndex;
                }
                EXPECT_EQ(rootIndex, size_t{ 2 });
            }

            // root 3
            {
                const auto eEnd = tree.Erase(it3);
                EXPECT_EQ(eEnd, tree.end());

                size_t rootIndex = 0;
                for (auto& node1 : tree)
                {
                    ASSERT_EQ(node1.GetData(), 1);
                    ++rootIndex;
                }
                EXPECT_EQ(rootIndex, size_t{ 1 });
            }

            // root 1
            {
                const auto eEnd = tree.Erase(it1);
                EXPECT_EQ(eEnd, tree.end());

                size_t rootIndex = 0;
                for (auto it = tree.begin(); it != tree.end(); ++it)
                {
                    ++rootIndex;
                }
                EXPECT_EQ(rootIndex, size_t{ 0 });
            }
        }
    }

    TEST(Utility, Tree_EraseFull)
    {
        {
            Tree<int> tree;

            //   P1    |   F2   |        P3
            // F4  P5  | P6  P7 | F8 P9 P10 P11 F12

            // Insert
            auto it2 = tree.Insert(tree.end(), 2);
            auto it1 = tree.Insert(tree.begin(), 1);
            auto it3 = tree.Insert(tree.end(), 3);

            tree.Insert(it1->end(), 4);
            tree.Insert(it1->end(), 5);

            tree.Insert(it2->begin(), 7);
            tree.Insert(it2->begin(), 6);

            tree.Insert(it3->end(), 9);
            const auto it34 = tree.Insert(it3->end(), 11);
            tree.Insert(it34, 10);
            tree.Insert(it3->end(), 12);
            tree.Insert(it3->begin(), 8);

        }
    }

}