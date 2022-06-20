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

#ifndef MOLTEN_CORE_RENDERER_COMBINEDTEXTURESAMPLER_HPP
#define MOLTEN_CORE_RENDERER_COMBINEDTEXTURESAMPLER_HPP

#include "Molten/Renderer/RenderResource.hpp"

namespace Molten
{

    template<size_t VDimensions> class Sampler;
    template<size_t VDimensions> class Texture;
    template<size_t VDimensions> class FramedTexture;

    template<size_t VDimensions>
    struct CombinedTextureSampler
    {

        CombinedTextureSampler() = default;
        CombinedTextureSampler(
            SharedRenderResource<Texture<VDimensions>> texture,
            SharedRenderResource<Sampler<VDimensions>> sampler);

        ~CombinedTextureSampler() = default;

        CombinedTextureSampler(const CombinedTextureSampler&) = default;
        CombinedTextureSampler(CombinedTextureSampler&&) = default;
        CombinedTextureSampler& operator =(const CombinedTextureSampler&) = default;
        CombinedTextureSampler& operator =(CombinedTextureSampler&&) = default;

        SharedRenderResource<Texture<VDimensions>> texture;
        SharedRenderResource<Sampler<VDimensions>> sampler;

    };

    using CombinedTextureSampler1D = CombinedTextureSampler<1>;
    using CombinedTextureSampler2D = CombinedTextureSampler<2>;
    using CombinedTextureSampler3D = CombinedTextureSampler<3>;


    template<size_t VDimensions>
    struct CombinedFramedTextureSampler
    {

        CombinedFramedTextureSampler() = default;
        CombinedFramedTextureSampler(
            SharedRenderResource<FramedTexture<VDimensions>> framedTexture,
            SharedRenderResource<Sampler<VDimensions>> sampler);

        ~CombinedFramedTextureSampler() = default;

        CombinedFramedTextureSampler(const CombinedFramedTextureSampler&) = default;
        CombinedFramedTextureSampler(CombinedFramedTextureSampler&&) = default;
        CombinedFramedTextureSampler& operator =(const CombinedFramedTextureSampler&) = default;
        CombinedFramedTextureSampler& operator =(CombinedFramedTextureSampler&&) = default;

        SharedRenderResource<FramedTexture<VDimensions>> framedTexture;
        SharedRenderResource<Sampler<VDimensions>> sampler;

    };

    using CombinedFramedTextureSampler1D = CombinedFramedTextureSampler<1>;
    using CombinedFramedTextureSampler2D = CombinedFramedTextureSampler<2>;
    using CombinedFramedTextureSampler3D = CombinedFramedTextureSampler<3>;

}

#include "Molten/Renderer/CombinedTextureSampler.inl"

#endif