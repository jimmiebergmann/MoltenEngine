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
#include "Molten/System/Task.hpp"
#include <array>

namespace Molten
{
    TEST(System, Task)
    {
        const size_t taskCount = 100;
        std::vector<TaskSharedPointer> tasks;
        std::array<size_t, taskCount> results = { 0 };

        for(size_t i = 0; i < taskCount; i++)
        {
            tasks.push_back(std::make_shared<Task>([i, &results]() { results[i] = i + 1; }));
        }

        for (auto& task : tasks)
        {
            (*task)();
        }

        for (size_t i = 0; i < taskCount; i++)
        {
            EXPECT_EQ(results[i], i + 1);
        }
    }

    TEST(System, Task_ParallelTaskGroup)
    {
        ThreadPool threadPool;
        ParallelTaskGroup parallelGroup(threadPool);

        const size_t taskCount = 24;
        std::array<size_t, taskCount> results = { 0 };

        auto clearResults = [&results]()
        {
            for (auto& result : results)
            {
                result = 0;
            }
        };

        for (size_t i = 0; i < taskCount; i++)
        {
            parallelGroup.Emplace<Task>([&, i]()
            {
                size_t loop = i;
                for (size_t j = 0; j < 10001; j++)
                {
                    loop = j + i + 10;
                }

                results[i] = loop;
            });
        }

        for (size_t i = 0; i < taskCount; i++)
        {
            EXPECT_EQ(results[i], 0);
        }

        {
            clearResults();
            {
                Molten::Test::Benchmarker benchmarker("Parallel 1");
                parallelGroup.Execute();
            }
            for (size_t i = 0; i < taskCount; i++)
            {
                EXPECT_EQ(results[i], i + 10010);
            }
        }
        {
            clearResults();
            {
                Molten::Test::Benchmarker benchmarker("Parallel 2");
                parallelGroup.Execute();
            }
            for (size_t i = 0; i < taskCount; i++)
            {
                EXPECT_EQ(results[i], i + 10010);
            }
        }
        {
            clearResults();
            {
                Molten::Test::Benchmarker benchmarker("Parallel 3");
                parallelGroup.Execute();
            }
            for (size_t i = 0; i < taskCount; i++)
            {
                EXPECT_EQ(results[i], i + 10010);
            }
        }
        {
            clearResults();
            {
                Molten::Test::Benchmarker benchmarker("Serial 1");
                for (auto& task : parallelGroup)
                {
                    (*task)();
                }
            }
            for (size_t i = 0; i < taskCount; i++)
            {
                EXPECT_EQ(results[i], i + 10010);
            }
        }
        {
            clearResults();
            {
                Molten::Test::Benchmarker benchmarker("Serial 2");
                for (auto& task : parallelGroup)
                {
                    (*task)();
                }
            }
            for (size_t i = 0; i < taskCount; i++)
            {
                EXPECT_EQ(results[i], i + 10010);
            }
        }
        {
            clearResults();
            {
                Molten::Test::Benchmarker benchmarker("Parallel 4");
                parallelGroup.Execute();
            }
            for (size_t i = 0; i < taskCount; i++)
            {
                EXPECT_EQ(results[i], i + 10010);
            }
        }
        {
            clearResults();
            {
                Molten::Test::Benchmarker benchmarker("Parallel 5");
                parallelGroup.Execute();
            }
            for (size_t i = 0; i < taskCount; i++)
            {
                EXPECT_EQ(results[i], i + 10010);
            }
        }
        {
            clearResults();
            {
                Molten::Test::Benchmarker benchmarker("Parallel 6");
                parallelGroup.Execute();
            }
            for (size_t i = 0; i < taskCount; i++)
            {
                EXPECT_EQ(results[i], i + 10010);
            }
        }
    }

    TEST(System, Task_SerialTaskGroup)
    {
        ThreadPool threadPool;
        SerialTaskGroup serialGroup(threadPool);

        std::array<size_t, 10> results = { 0 };

        serialGroup.EmplaceBack<Task>([&]()
        {
            for (size_t i = 0; i < results.size(); i++)
            {
                EXPECT_EQ(results[i], 0);
            }
            results[0] = 1;
        });

        auto p1 = serialGroup.EmplaceBack<ParallelTaskGroup>();
        p1->Emplace<Task>([&]()
        {
            for(size_t i = 4; i < results.size(); i++)
            {
                EXPECT_EQ(results[i], 0);
            }
            results[1] = 2;
        });
        p1->Emplace<Task>([&]()
        {
            for (size_t i = 4; i < results.size(); i++)
            {
                EXPECT_EQ(results[i], 0);
            }
            results[2] = 3;
        });
        p1->Emplace<Task>([&]()
        {
            for (size_t i = 4; i < results.size(); i++)
            {
                EXPECT_EQ(results[i], 0);
            }
            results[3] = 4;
        });

        serialGroup.EmplaceBack<Task>([&]()
        {
            EXPECT_EQ(results[0], 1);
            EXPECT_EQ(results[1], 2);
            EXPECT_EQ(results[2], 3);
            EXPECT_EQ(results[3], 4);

            for (size_t i = 4; i < results.size(); i++)
            {
                EXPECT_EQ(results[i], 0);
            }
            results[4] = 5;
        });

        auto p2 = serialGroup.EmplaceBack<ParallelTaskGroup>();
        p2->Emplace<Task>([&]()
        {
            EXPECT_EQ(results[0], 1);
            EXPECT_EQ(results[1], 2);
            EXPECT_EQ(results[2], 3);
            EXPECT_EQ(results[3], 4);
            EXPECT_EQ(results[4], 5);

            for (size_t i = 8; i < results.size(); i++)
            {
                EXPECT_EQ(results[i], 0);
            }
            results[5] = 6;
        });
        p2->Emplace<Task>([&]()
        {
            EXPECT_EQ(results[0], 1);
            EXPECT_EQ(results[1], 2);
            EXPECT_EQ(results[2], 3);
            EXPECT_EQ(results[3], 4);
            EXPECT_EQ(results[4], 5);

            for (size_t i = 8; i < results.size(); i++)
            {
                EXPECT_EQ(results[i], 0);
            }
            results[6] = 7;
        });
        p2->Emplace<Task>([&]()
        {
            EXPECT_EQ(results[0], 1);
            EXPECT_EQ(results[1], 2);
            EXPECT_EQ(results[2], 3);
            EXPECT_EQ(results[3], 4);
            EXPECT_EQ(results[4], 5);

            for (size_t i = 8; i < results.size(); i++)
            {
                EXPECT_EQ(results[i], 0);
            }
            results[7] = 8;
        });

        serialGroup.EmplaceBack<Task>([&]()
        {
            EXPECT_EQ(results[0], 1);
            EXPECT_EQ(results[1], 2);
            EXPECT_EQ(results[2], 3);
            EXPECT_EQ(results[3], 4);
            EXPECT_EQ(results[4], 5);
            EXPECT_EQ(results[5], 6);
            EXPECT_EQ(results[6], 7);
            EXPECT_EQ(results[7], 8);

            for (size_t i = 8; i < results.size(); i++)
            {
                EXPECT_EQ(results[i], 0);
            }
            results[8] = 9;
        });
        serialGroup.EmplaceBack<Task>([&]()
        {
            EXPECT_EQ(results[0], 1);
            EXPECT_EQ(results[1], 2);
            EXPECT_EQ(results[2], 3);
            EXPECT_EQ(results[3], 4);
            EXPECT_EQ(results[4], 5);
            EXPECT_EQ(results[5], 6);
            EXPECT_EQ(results[6], 7);
            EXPECT_EQ(results[7], 8);
            EXPECT_EQ(results[8], 9);
  
            results[9] = 10;
        });

        for(size_t i = 0; i < 10; i++)
        {
            results = { 0 };

            EXPECT_NO_THROW(serialGroup.Execute());
            EXPECT_EQ(results[0], 1);
            EXPECT_EQ(results[1], 2);
            EXPECT_EQ(results[2], 3);
            EXPECT_EQ(results[3], 4);
            EXPECT_EQ(results[4], 5);
            EXPECT_EQ(results[5], 6);
            EXPECT_EQ(results[6], 7);
            EXPECT_EQ(results[7], 8);
            EXPECT_EQ(results[8], 9);
            EXPECT_EQ(results[9], 10);
        }
    }


    TEST(System, Task_SerialTaskGroup_Emplace)
    {
        ThreadPool threadPool;
        {

            SerialTaskGroup serialGroup(threadPool);

            std::array<size_t, 8> results = { 0 };
            size_t index = 0;

            serialGroup.EmplaceBack<Task>([&]()
            {
                results[0] = ++index;
            });
            serialGroup.EmplaceFront<Task>([&]()
            {
                results[1] = ++index;
            });
            serialGroup.EmplaceBack<Task>([&]()
            {
                results[2] = ++index;
            });
            serialGroup.EmplaceFront<Task>([&]()
            {
                results[3] = ++index;
            });

            serialGroup.Emplace<Task>(serialGroup.begin(), [&]()
            {
                results[4] = ++index;
            });
            serialGroup.Emplace<Task>(serialGroup.end(), [&]()
            {
                results[5] = ++index;
            });
            serialGroup.Emplace<Task>(serialGroup.begin(), [&]()
            {
                results[6] = ++index;
            });
            serialGroup.Emplace<Task>(serialGroup.end(), [&]()
            {
                results[7] = ++index;
            });

            EXPECT_NO_THROW(serialGroup.Execute());

            EXPECT_EQ(index, 8);
            EXPECT_EQ(results[0], 5);
            EXPECT_EQ(results[1], 4);
            EXPECT_EQ(results[2], 6);
            EXPECT_EQ(results[3], 3);
            EXPECT_EQ(results[4], 2);
            EXPECT_EQ(results[5], 7);
            EXPECT_EQ(results[6], 1);
            EXPECT_EQ(results[7], 8);
        }
        {
            SerialTaskGroup serialGroup(threadPool);
            std::array<size_t, 4> results = { 0 };

            auto p1 = serialGroup.EmplaceBack<ParallelTaskGroup>();
            auto p2 = serialGroup.EmplaceFront<ParallelTaskGroup>();
            auto p3 = serialGroup.Emplace<ParallelTaskGroup>(serialGroup.end());
            auto p4 = serialGroup.Emplace<ParallelTaskGroup>(serialGroup.begin());

            p1->Emplace<Task>([&]()
            {
                results[0] = 3;
                EXPECT_EQ(results[1], 2);
                EXPECT_EQ(results[2], 0);
                EXPECT_EQ(results[3], 1);
            });
            p2->Emplace<Task>([&]()
            {
                EXPECT_EQ(results[0], 0);
                results[1] = 2;
                EXPECT_EQ(results[2], 0);
                EXPECT_EQ(results[3], 1);
                
            });
            p3->Emplace<Task>([&]()
            {
                EXPECT_EQ(results[0], 3);
                EXPECT_EQ(results[1], 2);
                results[2] = 4;
                EXPECT_EQ(results[3], 1);
            });
            p4->Emplace<Task>([&]()
            {
                EXPECT_EQ(results[0], 0);
                EXPECT_EQ(results[1], 0);
                EXPECT_EQ(results[2], 0);
                results[3] = 1;
            });

            EXPECT_NO_THROW(serialGroup.Execute());
            EXPECT_EQ(results[0], 3);
            EXPECT_EQ(results[1], 2);
            EXPECT_EQ(results[2], 4);
            EXPECT_EQ(results[3], 1);
        }
    }

}