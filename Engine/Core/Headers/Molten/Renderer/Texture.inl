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

namespace Molten
{

    // Texture descriptor implementations.
    template<size_t VDimensions>
    TextureDescriptor<VDimensions>::TextureDescriptor() :
        data(nullptr),
        dimensions(0),
        format(ImageFormat::URed8Green8Blue8),
        internalFormat(ImageFormat::URed8Green8Blue8),
        swizzleMapping{}
    {}

    template<size_t VDimensions>
    TextureDescriptor<VDimensions>::TextureDescriptor(
        const void* data,
        const Vector<VDimensions, uint32_t>& dimensions,
        const ImageFormat format,
        const ImageSwizzleMapping& swizzleMapping
    ) :
        data(data),
        dimensions(dimensions),
        format(format),
        internalFormat(format),
        swizzleMapping(swizzleMapping)
    {}

    template<size_t VDimensions>
    TextureDescriptor<VDimensions>::TextureDescriptor(
        const void* data,
        const Vector<VDimensions, uint32_t>& dimensions,
        const ImageFormat format,
        const ImageFormat internalFormat,
        const ImageSwizzleMapping& swizzleMapping
    ) :
        data(data),
        dimensions(dimensions),
        format(format),
        internalFormat(internalFormat),
        swizzleMapping(swizzleMapping)
    {}


    // Texture update descriptor.
    template<size_t VDimensions>
    TextureUpdateDescriptor<VDimensions>::TextureUpdateDescriptor(
        const void* data,
        Vector<VDimensions, uint32_t> destinationDimensions,
        Vector<VDimensions, uint32_t> destinationOffset
    ) :
        data(data),
        destinationDimensions(destinationDimensions),
        destinationOffset(destinationOffset)
    {}

}