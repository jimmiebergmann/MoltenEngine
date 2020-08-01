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

namespace Curse::Shader::Visual::Operators
{

    // Addition
    template<> struct Trait<Operators::AddInt32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::AddFloat32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::AddVec2f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::AddVec3f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::AddVec4f32>
    {
        inline static constexpr bool Supported = true;
    };
    // Division
    template<> struct Trait<Operators::DivInt32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::DivFloat32>
    {
        inline static constexpr bool Supported = true;
    };
    // Multiplication
    template<> struct Trait<Operators::MultInt32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::MultFloat32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::MultVec2f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::MultVec3f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::MultVec4f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::MultMat4f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::MultMat4Vec4f32>
    {
        inline static constexpr bool Supported = true;
    };
    // Subtraction
    template<> struct Trait<Operators::SubInt32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::SubFloat32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::SubVec2f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::SubVec3f32>
    {
        inline static constexpr bool Supported = true;
    };
    template<> struct Trait<Operators::SubVec4f32>
    {
        inline static constexpr bool Supported = true;
    };

}