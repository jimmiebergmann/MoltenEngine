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

#ifndef MOLTEN_CORE_RENDERER_UNIFORMBLOCK_HPP
#define MOLTEN_CORE_RENDERER_UNIFORMBLOCK_HPP

#include "Molten/Types.hpp"

namespace Molten
{

    class Pipeline;
    class UniformBuffer;

    /** Uniform block resource object. */
    class MOLTEN_API UniformBlock
    {

    protected:

        UniformBlock() = default;
        virtual ~UniformBlock() = default;

        UniformBlock(const UniformBlock&) = delete;
        UniformBlock(UniformBlock&&) = delete;
        UniformBlock& operator =(const UniformBlock&) = delete;
        UniformBlock& operator =(UniformBlock&&) = delete;

    };

    /** Descriptor class of uniform block class. */
    class MOLTEN_API UniformBlockDescriptor
    {

    public:

        UniformBlockDescriptor() = default;

        Pipeline* pipeline;
        UniformBuffer* buffer;
        uint32_t id;

    };

}

#endif
