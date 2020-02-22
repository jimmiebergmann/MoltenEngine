/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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

namespace Curse
{

    namespace Shader
    {

        // Addition
        template<> struct OperatorTrait<Operator::AddInt32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::AddFloat32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::AddVec2f32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::AddVec3f32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::AddVec4f32>
        {
            inline static const bool Supported = true;
        };
        // Division
        template<> struct OperatorTrait<Operator::DivInt32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::DivFloat32>
        {
            inline static const bool Supported = true;
        };
        // Multiplication
        template<> struct OperatorTrait<Operator::MultInt32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::MultFloat32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::MultVec2f32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::MultVec3f32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::MultVec4f32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::MultMat4f32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::MultMat4Vec4f32>
        {
            inline static const bool Supported = true;
        };
        // Subtraction
        template<> struct OperatorTrait<Operator::SubInt32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::SubFloat32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::SubVec2f32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::SubVec3f32>
        {
            inline static const bool Supported = true;
        };
        template<> struct OperatorTrait<Operator::SubVec4f32>
        {
            inline static const bool Supported = true;
        };

    }

}