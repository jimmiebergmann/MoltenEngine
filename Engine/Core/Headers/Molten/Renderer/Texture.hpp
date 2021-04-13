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

#ifndef MOLTEN_CORE_RENDERER_TEXTURE_HPP
#define MOLTEN_CORE_RENDERER_TEXTURE_HPP

#include "Molten/Renderer/ImageFormat.hpp"
#include "Molten/Math/Vector.hpp"

namespace Molten
{   

    /** Texture resource object. */
    template<size_t VDimensions>
    class Texture
    {

        static_assert(VDimensions >= 1 && VDimensions <= 3, "Texture must be of dimension 1-3.");

    public:

        Texture() = default;
        virtual ~Texture() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        Texture(const Texture&) = delete;
        Texture(Texture&&) = delete;
        Texture& operator =(const Texture&) = delete;
        Texture& operator =(Texture&&) = delete;
        /**@}*/

    };

    using Texture1D = Texture<1>;
    using Texture2D = Texture<2>;
    using Texture3D = Texture<3>;


    /** Descriptor class of texture class. */
    template<size_t VDimensions>
    struct TextureDescriptor
    {

        static_assert(VDimensions >= 1 && VDimensions <= 3, "Texture descriptor must be of dimension 1-3.");

        TextureDescriptor();
        TextureDescriptor(
            const Vector<VDimensions, uint32_t>& dimensions,
            const void* data,
            const ImageFormat format);
        TextureDescriptor(
            const Vector<VDimensions, uint32_t>& dimensions,
            const void* data,
            const ImageFormat format,
            const ImageFormat internalFormat);
        ~TextureDescriptor() = default;

        TextureDescriptor(const TextureDescriptor&) = default;
        TextureDescriptor(TextureDescriptor&&) = default;
        TextureDescriptor& operator =(const TextureDescriptor&) = default;
        TextureDescriptor& operator =(TextureDescriptor&&) = default;

        Vector<VDimensions, uint32_t> dimensions; 
        const void* data;
        ImageFormat format;
        ImageFormat internalFormat;

    };

    using TextureDescriptor1D = TextureDescriptor<1>;
    using TextureDescriptor2D = TextureDescriptor<2>;
    using TextureDescriptor3D = TextureDescriptor<3>;

}

#include "Molten/Renderer/Texture.inl"

#endif
