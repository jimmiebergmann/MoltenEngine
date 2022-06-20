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

#ifndef MOLTEN_CORE_RENDERER_VERTEXBUFFER_HPP
#define MOLTEN_CORE_RENDERER_VERTEXBUFFER_HPP

#include "Molten/Types.hpp"

namespace Molten
{

    /** Vertex buffer resource object. */
    class MOLTEN_API VertexBuffer
    {

    public:

        /** Enumerator of data types. */
        enum class DataType : uint8_t
        {
            Float32,
            Float64
        };

        VertexBuffer() = default;
        virtual ~VertexBuffer() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) = delete;
        VertexBuffer& operator =(const VertexBuffer&) = delete;
        VertexBuffer& operator =(VertexBuffer&&) = delete;
        /**@}*/

    };

    /**
    * @brief Descriptor class of vertex buffer class.
    */
    struct MOLTEN_API VertexBufferDescriptor
    {

        VertexBufferDescriptor();
        VertexBufferDescriptor(
            const uint32_t vertexCount,
            const uint32_t vertexSize,
            const void* data);
        ~VertexBufferDescriptor() = default;

        VertexBufferDescriptor(const VertexBufferDescriptor&) = default;
        VertexBufferDescriptor(VertexBufferDescriptor&&) = default;
        VertexBufferDescriptor& operator =(const VertexBufferDescriptor&) = default;
        VertexBufferDescriptor& operator =(VertexBufferDescriptor&&) = default;

        uint32_t vertexCount;
        uint32_t vertexSize;
        const void* data;

    };

}

#endif
