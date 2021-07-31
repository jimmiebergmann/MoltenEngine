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
#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include <string>

namespace Molten::Vulkan
{
    TEST(Renderer, VulkanResult_VkResult)
    {

        Result<> result1 = VkResult::VK_SUCCESS;
        EXPECT_TRUE(result1.IsSuccessful());
        ASSERT_EQ(result1.GetTypeIndex(), size_t(0));
        EXPECT_EQ(result1.Get<0>(), VkResult::VK_SUCCESS);

        Result<> result2 = result1;
        EXPECT_TRUE(result2.IsSuccessful());
        ASSERT_EQ(result2.GetTypeIndex(), size_t(0));
        EXPECT_EQ(result2.Get<0>(), VkResult::VK_SUCCESS);

        result2 = VkResult::VK_ERROR_DEVICE_LOST;
        EXPECT_FALSE(result2.IsSuccessful());
        ASSERT_EQ(result2.GetTypeIndex(), size_t(0));
        EXPECT_EQ(result2.Get<0>(), VkResult::VK_ERROR_DEVICE_LOST);

        Result<> result3 = result2;
        EXPECT_FALSE(result3.IsSuccessful());
        ASSERT_EQ(result3.GetTypeIndex(), size_t(0));
        EXPECT_EQ(result3.Get<0>(), VkResult::VK_ERROR_DEVICE_LOST);
    }

    TEST(Renderer, VulkanResult_std_string)
    {
        {
            Result<std::string> result1 = VkResult::VK_SUCCESS;
            ASSERT_EQ(result1.GetTypeIndex(), size_t(0));
            EXPECT_EQ(result1.Get<0>(), VkResult::VK_SUCCESS);
            EXPECT_TRUE(result1.ExpectSuccessOr("test"));
            EXPECT_TRUE(result1.ExpectAny(VkResult::VK_SUCCESS, "test"));

            Result<std::string> result2 = result1;
            ASSERT_EQ(result2.GetTypeIndex(), size_t(0));
            EXPECT_EQ(result2.Get<0>(), VkResult::VK_SUCCESS);
            EXPECT_TRUE(result2.ExpectSuccessOr("test"));
            EXPECT_TRUE(result2.ExpectAny(VkResult::VK_SUCCESS, "test"));

            result2 = VkResult::VK_ERROR_DEVICE_LOST;
            ASSERT_EQ(result2.GetTypeIndex(), size_t(0));
            EXPECT_EQ(result2.Get<0>(), VkResult::VK_ERROR_DEVICE_LOST);
            EXPECT_FALSE(result2.ExpectSuccessOr("test"));
            EXPECT_FALSE(result2.ExpectAny(VkResult::VK_SUCCESS, "test"));

            Result<std::string> result3 = result2;
            ASSERT_EQ(result3.GetTypeIndex(), size_t(0));
            EXPECT_EQ(result3.Get<0>(), VkResult::VK_ERROR_DEVICE_LOST);
            EXPECT_FALSE(result3.ExpectSuccessOr("test"));
            EXPECT_FALSE(result3.ExpectAny(VkResult::VK_SUCCESS, "test"));


            result1 = "wow";
            ASSERT_EQ(result1.GetTypeIndex(), size_t(1));
            EXPECT_EQ(result1.Get<1>(), "wow");
            EXPECT_FALSE(result1.ExpectSuccessOr("test"));
            EXPECT_FALSE(result1.ExpectAny(VkResult::VK_SUCCESS, "test"));
            result1 = "test";
            ASSERT_EQ(result1.GetTypeIndex(), size_t(1));
            EXPECT_EQ(result1.Get<1>(), "test");
            EXPECT_TRUE(result1.ExpectSuccessOr("test"));
            EXPECT_TRUE(result1.ExpectAny(VkResult::VK_SUCCESS, "test"));

            result2 = result1;
            ASSERT_EQ(result2.GetTypeIndex(), size_t(1));
            EXPECT_EQ(result2.Get<1>(), "test");
            EXPECT_TRUE(result2.ExpectSuccessOr("test"));
            EXPECT_TRUE(result2.ExpectAny(VkResult::VK_SUCCESS, "test"));
            result2 = "wow";
            ASSERT_EQ(result2.GetTypeIndex(), size_t(1));
            EXPECT_EQ(result2.Get<1>(), "wow");
            EXPECT_FALSE(result2.ExpectSuccessOr("test"));
            EXPECT_FALSE(result2.ExpectAny(VkResult::VK_SUCCESS, "test"));


            result2 = result3;
            ASSERT_EQ(result2.GetTypeIndex(), size_t(0));
            EXPECT_EQ(result2.Get<0>(), VkResult::VK_ERROR_DEVICE_LOST);
        }
    }

}

#endif