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

    // Static implementations.
    template<typename T>
    static void DestroyResource(Renderer* renderer, T* resource)
    {
        if(renderer)
        {
            renderer->Destroy(*resource);
        }

        delete resource;
    }


    // Render resource implementations.
    RenderResourceDeleter::RenderResourceDeleter(Renderer* renderer) :
        m_renderer(renderer)
    {}

    void RenderResourceDeleter::operator()(FramedDescriptorSet* framedDescriptorSet)
    {
        DestroyResource(m_renderer, framedDescriptorSet);
    }

    void RenderResourceDeleter::operator()(DescriptorSet* descriptorSet)
    {
        DestroyResource(m_renderer, descriptorSet);
    }
    
    void RenderResourceDeleter::operator()(IndexBuffer* indexBuffer)
    {
        DestroyResource(m_renderer, indexBuffer);
    }
    
    void RenderResourceDeleter::operator()(Pipeline* pipeline)
    {
        DestroyResource(m_renderer, pipeline);
    }
    
    void RenderResourceDeleter::operator()(RenderPass* renderPass)
    {
        DestroyResource(m_renderer, renderPass);
    }
    
    void RenderResourceDeleter::operator()(Sampler<1>* sampler1D)
    {
        DestroyResource(m_renderer, sampler1D);
    }
    
    void RenderResourceDeleter::operator()(Sampler<2>* sampler2D)
    {
        DestroyResource(m_renderer, sampler2D);
    }
    
    void RenderResourceDeleter::operator()(Sampler<3>* sampler3D)
    {
        DestroyResource(m_renderer, sampler3D);
    }
    
    void RenderResourceDeleter::operator()(ShaderProgram* shaderProgram)
    {
        DestroyResource(m_renderer, shaderProgram);
    }
    
    void RenderResourceDeleter::operator()(Texture<1>* texture1D)
    {
        DestroyResource(m_renderer, texture1D);
    }
    
    void RenderResourceDeleter::operator()(Texture<2>* texture2D)
    {
        DestroyResource(m_renderer, texture2D);
    }
    
    void RenderResourceDeleter::operator()(Texture<3>* texture3D)
    {
        DestroyResource(m_renderer, texture3D);
    }
    
    void RenderResourceDeleter::operator()(UniformBuffer* uniformBuffer)
    {
        DestroyResource(m_renderer, uniformBuffer);
    }
    
    void RenderResourceDeleter::operator()(FramedUniformBuffer* framedUniformBuffer)
    {
        DestroyResource(m_renderer, framedUniformBuffer);
    }
    
    void RenderResourceDeleter::operator()(VertexBuffer* vertexBuffer)
    {
        DestroyResource(m_renderer, vertexBuffer);
    }  

}