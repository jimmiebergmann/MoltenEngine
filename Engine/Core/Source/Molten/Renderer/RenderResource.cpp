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

#include "Molten/Renderer/RenderResource.hpp"
#include "Molten/Renderer/Renderer.hpp"

namespace Molten
{
    // Render resource deleter helper implementations.
    RenderResourceDeleterHelper::RenderResourceDeleterHelper(Renderer* renderer) :
        renderer(renderer)
    {}

    void RenderResourceDeleterHelper::Destroy(DescriptorSet& descriptorSet)
    {
        renderer->Destroy(descriptorSet);
    }

    void RenderResourceDeleterHelper::Destroy(FramedDescriptorSet& framedDescriptorSet)
    {
        renderer->Destroy(framedDescriptorSet);
    }

    void RenderResourceDeleterHelper::Destroy(Framebuffer& framebuffer)
    {
        renderer->Destroy(framebuffer);
    }

    void RenderResourceDeleterHelper::Destroy(IndexBuffer& indexBuffer)
    {
        renderer->Destroy(indexBuffer);
    }

    void RenderResourceDeleterHelper::Destroy(Pipeline& pipeline)
    {
        renderer->Destroy(pipeline);
    }

    void RenderResourceDeleterHelper::Destroy(Sampler<1>& sampler1D)
    {
        renderer->Destroy(sampler1D);
    }

    void RenderResourceDeleterHelper::Destroy(Sampler<2>& sampler2D)
    {
        renderer->Destroy(sampler2D);
    }

    void RenderResourceDeleterHelper::Destroy(Sampler<3>& sampler3D)
    {
        renderer->Destroy(sampler3D);
    }

    void RenderResourceDeleterHelper::Destroy(Texture<1>& texture1D)
    {
        renderer->Destroy(texture1D);
    }

    void RenderResourceDeleterHelper::Destroy(Texture<2>& texture2D)
    {
        renderer->Destroy(texture2D);
    }

    void RenderResourceDeleterHelper::Destroy(Texture<3>& texture3D)
    {
        renderer->Destroy(texture3D);
    }

    void RenderResourceDeleterHelper::Destroy(UniformBuffer& uniformBuffer)
    {
        renderer->Destroy(uniformBuffer);
    }

    void RenderResourceDeleterHelper::Destroy(FramedUniformBuffer& framedUniformBuffer)
    {
        renderer->Destroy(framedUniformBuffer);
    }

    void RenderResourceDeleterHelper::Destroy(VertexBuffer& vertexBuffer)
    {
        renderer->Destroy(vertexBuffer);
    }

}