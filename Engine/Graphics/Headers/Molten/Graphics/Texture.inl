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

namespace Molten
{

    // Texture implementations.
    template<size_t VDimensions>
    Texture<VDimensions>::Texture(
        const ImageFormat format,
        const Vector<VDimensions, uint32_t>& dimensions
    ) :
        m_format(format),
        m_dimensions(dimensions)
    {}

    template<size_t VDimensions>
    ImageFormat Texture<VDimensions>::GetFormat() const
    {
        return m_format;
    }

    template<size_t VDimensions>
    Vector<VDimensions, uint32_t> Texture<VDimensions>::GetDimensions() const
    {
        return m_dimensions;
    }


    // Framed texture implementations.
    template<size_t VDimensions>
    FramedTexture<VDimensions>::FramedTexture(
        const ImageFormat format,
        const Vector<VDimensions, uint32_t>& dimensions
    ) :
        m_format(format),
        m_dimensions(dimensions)
    {}

    template<size_t VDimensions>
    ImageFormat FramedTexture<VDimensions>::GetFormat() const
    {
        return m_format;
    }

    template<size_t VDimensions>
    Vector<VDimensions, uint32_t> FramedTexture<VDimensions>::GetDimensions() const
    {
        return m_dimensions;
    }

}