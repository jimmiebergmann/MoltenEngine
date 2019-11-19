/*
* MIT License
*
* Copyright (c) 2019 Jimmie Bergmann
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


#include "Curse/Renderer/Vulkan/RendererVulkan.hpp"

#if defined(CURSE_ENABLE_VULKAN)

#include "Curse/Window/WindowBase.hpp"
#include "Curse/System/Exception.hpp"

namespace Curse
{

    RendererVulkan::RendererVulkan()
    {
    }

    RendererVulkan::RendererVulkan(const WindowBase& window, const Version& version) :
        RendererVulkan()
    {
        Open(window, version);
    }

    RendererVulkan::~RendererVulkan()
    {
        Close();
    }

    void RendererVulkan::Open(const WindowBase& /*window*/, const Version& /*version*/)
    {

    }

    void RendererVulkan::Close()
    {
        
    }

    Renderer::BackendApi RendererVulkan::GetBackendApi() const
    {
        return Renderer::BackendApi::Vulkan;
    }

    Version RendererVulkan::GetVersion() const
    {
        return m_version;
    }

    void RendererVulkan::SwapBuffers()
    {
        //::SwapBuffers(m_deviceContext);
    }


}

#endif