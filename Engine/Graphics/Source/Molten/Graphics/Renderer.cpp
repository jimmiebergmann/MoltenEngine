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

#include "Molten/Graphics/Renderer.hpp"
#include "Molten/Graphics/OpenGL/OpenGLRenderer.hpp"
#include "Molten/Graphics/Vulkan/VulkanRenderer.hpp"

namespace Molten
{

    std::unique_ptr<Renderer> Renderer::Create(
        const BackendApi backendApi,
        [[maybe_unused]] const RendererDescriptor& descriptor)
    {
        switch (backendApi)
        {
#if MOLTEN_ENABLE_OPENGL
        case BackendApi::OpenGL: {
            auto renderer = std::make_unique<OpenGLRenderer>();
            if (!renderer->Open(descriptor))
            {
                return nullptr;
            }
            return renderer;
        }         
#endif
#if MOLTEN_ENABLE_VULKAN
        case BackendApi::Vulkan: {
            auto renderer = std::make_unique<VulkanRenderer>();
            if (!renderer->Open(descriptor))
            {
                return nullptr;
            }
            return renderer;
        }         
#endif
        default: break;
        }

        return nullptr;
    }

}
