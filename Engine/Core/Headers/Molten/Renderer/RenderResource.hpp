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

#ifndef MOLTEN_CORE_RENDERER_RENDERERRESOURCE_HPP
#define MOLTEN_CORE_RENDERER_RENDERERRESOURCE_HPP

#include "Molten/Core.hpp"
#include <memory>

namespace Molten
{
    class Renderer;

    class DescriptorSet;
    class FramedDescriptorSet;
    class Framebuffer;
    class IndexBuffer;
    class Pipeline;
    template<size_t VDimensions> class Sampler;
    class Texture;
    class UniformBuffer;
    class FramedUniformBuffer;
    class VertexBuffer; 

    struct MOLTEN_API RenderResourceDeleterHelper
    {

        explicit RenderResourceDeleterHelper(Renderer* renderer);

        void Destroy(DescriptorSet& descriptorSet);
        void Destroy(FramedDescriptorSet& framedDescriptorSet);
        void Destroy(Framebuffer& framebuffer);
        void Destroy(IndexBuffer& indexBuffer);
        void Destroy(Pipeline& pipeline);
        void Destroy(Sampler<1>& sampler1D);
        void Destroy(Sampler<2>& sampler2D);
        void Destroy(Sampler<3>& sampler3D);
        void Destroy(Texture& texture);
        void Destroy(UniformBuffer& uniformBuffer);
        void Destroy(FramedUniformBuffer& framedUniformBuffer);
        void Destroy(VertexBuffer& vertexBuffer);

        Renderer* renderer;

    };

    template<typename T>
    struct RenderResourceDeleter : private RenderResourceDeleterHelper
    {

        explicit RenderResourceDeleter(Renderer* renderer = nullptr);
        ~RenderResourceDeleter() = default;

        RenderResourceDeleter(const RenderResourceDeleter& renderResourceDeleter) = default;
        RenderResourceDeleter(RenderResourceDeleter&&) = default;
        RenderResourceDeleter& operator =(const RenderResourceDeleter& renderResourceDeleter) = default;
        RenderResourceDeleter& operator =(RenderResourceDeleter &&) = default;

        void operator()(T* resource);

    };


    template<typename T>
    using RenderResource = std::unique_ptr<T, RenderResourceDeleter<T>>;

    template<typename T>
    using SharedRenderResource = std::shared_ptr<T>;

}

#include "Molten/Renderer/RenderResource.inl"

#endif
