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

#ifndef MOLTEN_GRAPHICS_TEXTURE_HPP
#define MOLTEN_GRAPHICS_TEXTURE_HPP

#include "Molten/Graphics/ImageFormat.hpp"
#include "Molten/Graphics/ImageSwizzle.hpp"
#include "Molten/Math/Vector.hpp"

namespace Molten
{   

    enum class TextureType
    {
        Color,          ///< Color texture.
        DepthStencil    ///< Depth/stencil texture.
    };

    enum class TextureUsage
    {
        ReadOnly,    ///< Read only texture, used as shader input.
        Attachment,  ///< Attachment of render pass.
    };


    /** Texture resource object. */
    template<size_t VDimensions>
    class Texture
    {

        static_assert(VDimensions >= 1 && VDimensions <= 3, "Texture must be of dimension 1-3.");

    public:

        static constexpr size_t Dimensions = VDimensions;

        Texture(
            const ImageFormat format,
            const Vector<VDimensions, uint32_t>& dimensions);

        virtual ~Texture() = default;

        /** Move constructor and assignment operator. */
        /**@{*/
        Texture(Texture&&) = default;
        Texture& operator = (Texture&&) = default;
        /**@}*/

        /** Deleted copy constructor and assignment operator. */
        /**@{*/
        Texture(const Texture&) = delete;
        Texture& operator = (const Texture&) = delete;
        /**@}*/

        /** Get image format of this texture. */
        [[nodiscard]] ImageFormat GetFormat() const;

        /** Get image dimensions of this texture. */
        [[nodiscard]] Vector<VDimensions, uint32_t> GetDimensions() const;

    protected:

        ImageFormat m_format;
        Vector<VDimensions, uint32_t> m_dimensions;

    };

    using Texture1D = Texture<1>;
    using Texture2D = Texture<2>;
    using Texture3D = Texture<3>;


    /** Framed texture resource object. */
    template<size_t VDimensions>
    class FramedTexture
    {

        static_assert(VDimensions >= 1 && VDimensions <= 3, "Texture must be of dimension 1-3.");

    public:

        static constexpr size_t Dimensions = VDimensions;

        FramedTexture(
            const ImageFormat format,
            const Vector<VDimensions, uint32_t>& dimensions);

        virtual ~FramedTexture() = default;

        /** Move constructor and assignment operator. */
        /**@{*/
        FramedTexture(FramedTexture&&) = default;
        FramedTexture& operator = (FramedTexture&&) = default;
        /**@}*/

        /** Deleted copy constructor and assignment operator. */
        /**@{*/
        FramedTexture(const FramedTexture&) = delete;
        FramedTexture& operator = (const FramedTexture&) = delete;
        /**@}*/

        /** Get image format of all texture frames. */
        [[nodiscard]] ImageFormat GetFormat() const;

        /** Get image dimensions of all texture frames. */
        [[nodiscard]] Vector<VDimensions, uint32_t> GetDimensions() const;

    protected:

        ImageFormat m_format;
        Vector<VDimensions, uint32_t> m_dimensions;

    };

    using FramedTexture1D = FramedTexture<1>;
    using FramedTexture2D = FramedTexture<2>;
    using FramedTexture3D = FramedTexture<3>;


    /** Descriptor class of texture class. */
    template<size_t VDimensions>
    struct TextureDescriptor
    {

        static_assert(VDimensions >= 1 && VDimensions <= 3, "Texture descriptor must be of dimension 1-3.");

        const void* data = nullptr;
        Vector<VDimensions, uint32_t> dimensions = {};
        TextureType type = TextureType::Color;
        TextureUsage initialUsage = TextureUsage::ReadOnly;
        ImageFormat format = ImageFormat::URed8Green8Blue8;
        ImageFormat internalFormat = ImageFormat::URed8Green8Blue8;
        ImageSwizzleMapping swizzleMapping = {};
    };

    using TextureDescriptor1D = TextureDescriptor<1>;
    using TextureDescriptor2D = TextureDescriptor<2>;
    using TextureDescriptor3D = TextureDescriptor<3>;


    /** Descriptor class of texture update. */
    template<size_t VDimensions>
    struct TextureUpdateDescriptor
    {
        static_assert(VDimensions >= 1 && VDimensions <= 3, "Texture descriptor must be of dimension 1-3.");

        const void* data = nullptr;          
        Vector<VDimensions, uint32_t> destinationDimensions = {};
        Vector<VDimensions, uint32_t> destinationOffset = {};
    };

    using TextureUpdateDescriptor1D = TextureUpdateDescriptor<1>;
    using TextureUpdateDescriptor2D = TextureUpdateDescriptor<2>;
    using TextureUpdateDescriptor3D = TextureUpdateDescriptor<3>;

}

#include "Molten/Graphics/Texture.inl"

#endif
