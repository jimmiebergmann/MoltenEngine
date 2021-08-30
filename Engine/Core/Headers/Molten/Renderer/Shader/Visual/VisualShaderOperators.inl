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

namespace Molten::Shader::Visual::Operators
{

    // Addition
    template<> struct Trait<AddInt32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<AddFloat32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<AddVec2f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<AddVec3f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<AddVec4f32>
    {
        inline static constexpr bool Supported = true;
    };
    // Division
    template<> struct Trait<DivInt32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<DivFloat32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<DivVec2f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<DivVec3f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<DivVec4f32>
    {
        inline static constexpr bool Supported = true;
    };
    // Multiplication
    template<> struct Trait<MultInt32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<MultFloat32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<MultVec2f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<MultVec3f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<MultVec4f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<MultMat4f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<MultMat4f32Vec4f32>
    {
        inline static constexpr bool Supported = true;
    };
    // Subtraction
    template<> struct Trait<SubInt32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<SubFloat32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<SubVec2f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<SubVec3f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<SubVec4f32>
    {
        inline static constexpr bool Supported = true;
    };

}