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
#include "Molten/System/ThreadPool.hpp"
#include <array>
#include <string>

namespace Molten
{
    TEST(System, ThreadPool_Constructor)
    {
        {
            const ThreadPool pool(0);
            EXPECT_GT(pool.GetWorkerCount(), size_t{ 0 });
            EXPECT_EQ(pool.GetWorkerCount(), std::thread::hardware_concurrency());
        }
        {
            const ThreadPool pool(1);
            EXPECT_EQ(pool.GetWorkerCount(), size_t{ 1 });
        }
        {
            const ThreadPool pool(2);
            EXPECT_EQ(pool.GetWorkerCount(), size_t{ 2 });
        }

    }

    TEST(System, ThreadPool_Execute_SingleThread)
    {
        ThreadPool pool(1);
        std::array<size_t, 200> values = {};

        for(size_t i = 0; i < values.size(); i++)
        {
            pool.Execute([i, &values]()
            {
                values[i] = i + 1;
            }).get();
        }

        for (size_t i = 0; i < values.size(); i++)
        {
            EXPECT_EQ(values[i], size_t{ i + 1 });
        }
    }


    TEST(System, ThreadPool_Execute_MultipleThreads)
    {
        ThreadPool pool(std::thread::hardware_concurrency());

        { // Set value.
            std::array<size_t, 200> values = {};

            size_t index = 0;
            for (size_t i = 0; i < 20; i++)
            {
                std::vector<std::future<void>> results;

                for (size_t j = 0; j < 10; j++)
                {
                    auto result = pool.Execute([index, &values]()
                    {
                        values[index] = index + 1;
                    });

                    results.push_back(std::move(result));

                    ++index;
                }

                for (auto& result : results)
                {
                    result.get();
                }
            }

            for (size_t i = 0; i < values.size(); i++)
            {
                EXPECT_EQ(values[i], size_t{ i + 1 });
            }
        }    
        { // Set value and future.
       
            std::array<std::pair<size_t, std::future<size_t>>, 50> values = {};

            for (size_t i = 0; i < values.size(); i++)
            {
                std::vector<std::future<size_t>> results;

                auto result = pool.Execute([i, &values]()
                {
                    return values[i].first = i + 1;
                });

                values[i].second = std::move(result);
            }

            for (size_t i = 0; i < values.size(); i++)
            {
                EXPECT_EQ(values[i].second.get(), size_t{ i + 1 });
                EXPECT_EQ(values[i].first, size_t{ i + 1 });
            }
        }
        { // Non-copyable but movable future type.

            const std::string testString = "This is a long string: ";

            struct Data
            {
                Data() = default;
                ~Data() = default;
                Data(const Data&) = delete;
                Data& operator = (const Data&) = delete;

                Data(Data&& data) noexcept :
                    string(std::move(data.string))
                {}

                Data& operator = (Data&& data) noexcept
                {
                    string = std::move(data.string);
                    return *this;
                }

                std::string string;
            };

            std::array<std::future<Data>, 10> futures = {};

            for (size_t i = 0; i < futures.size(); i++)
            {
                auto result = pool.Execute([&, i]()
                {
                    Data data;
                    data.string = testString + std::to_string(i);
                    return data;
                });

                futures[i] = std::move(result);
            }

            for (size_t i = 0; i < futures.size(); i++)
            {
                auto value = futures[i].get();
                EXPECT_EQ(value.string, std::string(testString + std::to_string(i)));
            }
        }
    }
    
    TEST(System, ThreadPool_TryExecute)
    {
        ThreadPool pool(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::array<size_t, 3> values = { 0, 0, 0 };

        for (size_t i = 0; i < 2; i++)
        {
            values = { 0, 0, 0 };

            auto tryResult1 = pool.TryExecute([&]()
            {
                values[0] = 1;
            });
            
            ASSERT_TRUE(tryResult1.has_value());
            tryResult1.value().wait();
            
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto tryResult2 = pool.TryExecute([&]()
            {
                values[1] = 2;
            });
            
            std::this_thread::sleep_for(std::chrono::seconds(1));

            ASSERT_TRUE(tryResult2.has_value());
            tryResult2.value().wait();

            auto tryResult3 = pool.TryExecute([&]()
            {
                values[2] = 3;
            });
            ASSERT_TRUE(tryResult3.has_value());     
            tryResult3.value().wait();

            EXPECT_EQ(values[0], size_t{ 1 });
            EXPECT_EQ(values[1], size_t{ 2 });
            EXPECT_EQ(values[2], size_t{ 3 });

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

}