/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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
#include "Molten/Shader/Visual/VisualShaderScript.hpp"

namespace Molten::Shader::Visual
{

    TEST(Shader, VisualShader_DescriptorSet)
    {

        FragmentScript script;
        FragmentDescriptorSets descSets(script);
        const FragmentDescriptorSets& constDescSets = descSets;

        for (size_t i = 0; i < 3; i++)
        {
            // Add
            EXPECT_EQ(descSets.begin(), descSets.end());
            EXPECT_EQ(constDescSets.begin(), constDescSets.end());
            EXPECT_EQ(descSets.GetSetCount(), size_t{ 0 });

            auto* set1 = descSets.AddSet(0);
            ASSERT_NE(set1, nullptr);
            EXPECT_EQ(set1->GetId(), uint32_t(0));
            EXPECT_EQ(descSets.GetSetCount(), size_t{ 1 });
            EXPECT_NE(constDescSets.begin(), constDescSets.end());

            auto* set2 = descSets.AddSet(0);
            ASSERT_EQ(set2, nullptr);
            EXPECT_EQ(descSets.GetSetCount(), size_t{ 1 });

            auto* set3 = descSets.AddSet(1);
            ASSERT_NE(set3, nullptr);
            EXPECT_EQ(set3->GetId(), uint32_t(1));
            EXPECT_EQ(descSets.GetSetCount(), size_t{ 2 });

            for (uint32_t j = 2; j < 32; j++)
            {
                descSets.AddSet(j);
            }
            EXPECT_EQ(descSets.GetSetCount(), size_t(32));

            // Remove
            descSets.RemoveSet(10);
            descSets.RemoveSet(12);
            descSets.RemoveSet(13);
            descSets.RemoveSet(15);
            descSets.RemoveSet(0);
            EXPECT_EQ(descSets.GetSetCount(), size_t(27));

            set1 = descSets.GetSet(0);
            ASSERT_NE(set1, nullptr);
            EXPECT_EQ(set1->GetId(), uint32_t(1));

            descSets.RemoveSet(descSets.begin());
            descSets.RemoveSet(constDescSets.begin());
            EXPECT_EQ(descSets.GetSetCount(), size_t(25));

            set1 = descSets.GetSet(0);
            ASSERT_NE(set1, nullptr);
            EXPECT_EQ(set1->GetId(), uint32_t(3));

            for (uint32_t j = 0; j < 25; j++)
            {
                descSets.RemoveSet(0);
            }

            EXPECT_EQ(descSets.begin(), descSets.end());
            EXPECT_EQ(constDescSets.begin(), constDescSets.end());
            EXPECT_EQ(descSets.GetSetCount(), size_t{ 0 });
        }

    }

    TEST(Shader, VisualShader_DescriptorBindings_Sampler)
    {
        FragmentScript script;
        FragmentDescriptorSets descSets(script);
        
        {
            auto& set = *descSets.AddSet(0);
            const auto& constSet = set;

            for (size_t i = 0; i < 3; i++)
            {
                // Add
                EXPECT_EQ(set.begin(), set.end());
                EXPECT_EQ(constSet.begin(), constSet.end());
                EXPECT_EQ(set.GetBindingCount(), size_t{ 0 });
                
                auto binding1 = set.AddBinding<Sampler2D>(0);
                ASSERT_NE(binding1, nullptr);
                EXPECT_NE(set.begin(), set.end());
                EXPECT_NE(constSet.begin(), constSet.end());
                EXPECT_EQ(set.GetBindingCount(), size_t{ 1 });

                auto binding2 = set.AddBinding<Sampler2D>(0);
                ASSERT_EQ(binding2, nullptr);
                EXPECT_NE(set.begin(), set.end());
                EXPECT_NE(constSet.begin(), constSet.end());
                EXPECT_EQ(set.GetBindingCount(), size_t{ 1 });

                auto binding3 = set.AddBinding<Sampler2D>(1);
                ASSERT_NE(binding3, nullptr);
                EXPECT_NE(set.begin(), set.end());
                EXPECT_NE(constSet.begin(), constSet.end());
                EXPECT_EQ(set.GetBindingCount(), size_t{ 2 });

                for (uint32_t j = 2; j < 32; j++)
                {
                    set.AddBinding<Sampler2D>(j);
                }
                EXPECT_EQ(set.GetBindingCount(), size_t{ 32 });

                // Remove
                set.RemoveBinding(10);
                set.RemoveBinding(12);
                set.RemoveBinding(13);
                set.RemoveBinding(15);
                set.RemoveBinding(0);
                EXPECT_EQ(set.GetBindingCount(), size_t{ 27 });

                binding1 = set.GetBinding<Sampler2D>(0);
                ASSERT_NE(binding1, nullptr);
                EXPECT_EQ(binding1->GetId(), uint32_t{ 1 });
                auto binding1base = set.GetBindingBase(0);
                ASSERT_NE(binding1base, nullptr);
                EXPECT_EQ(binding1base->GetId(), uint32_t{ 1 });

                set.RemoveBinding(set.begin());
                set.RemoveBinding(constSet.begin());
                EXPECT_EQ(set.GetBindingCount(), uint32_t{ 25 });

                binding1 = set.GetBinding<Sampler2D>(0);
                ASSERT_NE(binding1, nullptr);
                EXPECT_EQ(binding1->GetId(), uint32_t{ 3 });

                for (uint32_t j = 0; j < 25; j++)
                {
                    set.RemoveBinding(0);
                }

                EXPECT_EQ(set.begin(), set.end());
                EXPECT_EQ(constSet.begin(), constSet.end());
                EXPECT_EQ(set.GetBindingCount(), size_t{ 0 });
            }
        }
    }

    TEST(Shader, VisualShader_DescriptorBindings_UniformBuffer)
    {
        FragmentScript script;
        FragmentDescriptorSets descSets(script);

        {
            auto set = descSets.AddSet(0);
            auto ub = set->AddBinding<FragmentUniformBuffer>(0);
            ASSERT_NE(ub, nullptr);
        }
    }
}