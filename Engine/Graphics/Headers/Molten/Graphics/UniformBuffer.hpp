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

#ifndef MOLTEN_GRAPHICS_UNIFORMBUFFER_HPP
#define MOLTEN_GRAPHICS_UNIFORMBUFFER_HPP

#include "Molten/Graphics/Build.hpp"

namespace Molten
{

    /** Uniform buffer resource object. */
    class MOLTEN_GRAPHICS_API UniformBuffer
    {

    public:

        UniformBuffer() = default;
        virtual ~UniformBuffer() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer(UniformBuffer&&) = delete;
        UniformBuffer& operator =(const UniformBuffer&) = delete;
        UniformBuffer& operator =(UniformBuffer&&) = delete;
        /**@}*/

    };

    /** Descriptor class of uniform buffer class. */
    struct MOLTEN_GRAPHICS_API UniformBufferDescriptor
    {

        UniformBufferDescriptor();
        explicit UniformBufferDescriptor(const uint32_t size);
        ~UniformBufferDescriptor() = default;

        UniformBufferDescriptor(const UniformBufferDescriptor&) = default;
        UniformBufferDescriptor(UniformBufferDescriptor&&) = default;
        UniformBufferDescriptor& operator =(const UniformBufferDescriptor&) = default;
        UniformBufferDescriptor& operator =(UniformBufferDescriptor&&) = default;

        uint32_t size; ///< Buffer size in bytes.

    };


    /** Uniform buffer resource object. */
    class MOLTEN_GRAPHICS_API FramedUniformBuffer
    {

    public:

        FramedUniformBuffer() = default;
        virtual ~FramedUniformBuffer() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        FramedUniformBuffer(const FramedUniformBuffer&) = delete;
        FramedUniformBuffer(FramedUniformBuffer&&) = delete;
        FramedUniformBuffer& operator =(const FramedUniformBuffer&) = delete;
        FramedUniformBuffer& operator =(FramedUniformBuffer&&) = delete;
        /**@}*/        

    };

    
    /** Descriptor class of framed uniform buffer class. */
    struct MOLTEN_GRAPHICS_API FramedUniformBufferDescriptor
    {

        FramedUniformBufferDescriptor();
        explicit FramedUniformBufferDescriptor(const uint32_t size);
        ~FramedUniformBufferDescriptor() = default;

        FramedUniformBufferDescriptor(const FramedUniformBufferDescriptor&) = default;
        FramedUniformBufferDescriptor(FramedUniformBufferDescriptor&&) = default;
        FramedUniformBufferDescriptor& operator =(const FramedUniformBufferDescriptor&) = default;
        FramedUniformBufferDescriptor& operator =(FramedUniformBufferDescriptor&&) = default;

        uint32_t size;///< Buffer size in bytes.

    };

}

#endif
