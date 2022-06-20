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
#include "Molten/Shader/Shader.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Matrix.hpp"


namespace Molten::Shader
{

    TEST(Shader, PaddedType)
    {
        EXPECT_EQ(sizeof(PaddedType<bool>), size_t{ 16 });
        EXPECT_EQ(sizeof(PaddedType<int32_t>), size_t{ 16 });
        EXPECT_EQ(sizeof(PaddedType<float>), size_t{ 16 });
        EXPECT_EQ(sizeof(PaddedType<Vector2f32>), size_t{ 16 });
        EXPECT_EQ(sizeof(PaddedType<Vector3f32>), size_t{ 16 });
        EXPECT_EQ(sizeof(PaddedType<Vector4f32>), size_t{ 16 });
        EXPECT_EQ(sizeof(PaddedType<Matrix4x4f32>), size_t{ 64 });

    }

}