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

#ifndef MOLTEN_GRAPHICS_INDEXBUFFER_HPP
#define MOLTEN_GRAPHICS_INDEXBUFFER_HPP

#include "Molten/Graphics/Build.hpp"

namespace Molten
{

    /** Index buffer resource object. */
    class MOLTEN_GRAPHICS_API IndexBuffer
    {

    public:

        /** Enumerator of data types. */
        enum class DataType : uint8_t
        {
            Uint16, ///< 16 bit unsigned integer data type.
            Uint32  ///< 32 bit unsigned integer data type.
        };

        IndexBuffer() = default;
        virtual ~IndexBuffer() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer(IndexBuffer&&) = delete;
        IndexBuffer& operator =(const IndexBuffer&) = delete;
        IndexBuffer& operator =(IndexBuffer&&) = delete;
        /**@}*/

    };

    /** Descriptor class of index buffer class. */
    struct MOLTEN_GRAPHICS_API IndexBufferDescriptor
    {
        uint32_t indexCount = 0;
        const void* data = nullptr;   
        IndexBuffer::DataType dataType = IndexBuffer::DataType::Uint32;
    };

}

#endif
