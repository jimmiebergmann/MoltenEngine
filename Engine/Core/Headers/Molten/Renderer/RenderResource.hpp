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
    class IndexBuffer;
    class Pipeline;
    class RenderPass;
    template<size_t VDimensions> class Sampler;
    class ShaderProgram;
    template<size_t VDimensions> class Texture;
    class UniformBuffer;
    class FramedUniformBuffer;
    class VertexBuffer;

    class RenderResourceDeleter
    {

    public:

        explicit RenderResourceDeleter(Renderer* renderer = nullptr);
        ~RenderResourceDeleter() = default;

        RenderResourceDeleter(const RenderResourceDeleter& renderResourceDeleter) = default;
        RenderResourceDeleter(RenderResourceDeleter&&) = default;
        RenderResourceDeleter& operator =(const RenderResourceDeleter& renderResourceDeleter) = default;
        RenderResourceDeleter& operator =(RenderResourceDeleter &&) = default;

        void operator()(FramedDescriptorSet* framedDescriptorSet);
        void operator()(DescriptorSet* descriptorSet);
        void operator()(IndexBuffer* indexBuffer);
        void operator()(Pipeline* pipeline);
        void operator()(RenderPass* renderPass);
        void operator()(Sampler<1>* sampler1D);
        void operator()(Sampler<2>* sampler2D);
        void operator()(Sampler<3>* sampler3D);
        void operator()(ShaderProgram* shaderProgram);
        void operator()(Texture<1>* texture1D);
        void operator()(Texture<2>* texture2D);
        void operator()(Texture<3>* texture3D);
        void operator()(UniformBuffer* uniformBuffer);
        void operator()(FramedUniformBuffer* framedUniformBuffer);
        void operator()(VertexBuffer* vertexBuffer);

    private:

        Renderer* m_renderer;
    };


    template<typename T>
    using RenderResource = std::unique_ptr<T, RenderResourceDeleter>;

    template<typename T>
    using SharedRenderResource = std::shared_ptr<T>;

}

#endif
