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

#ifndef CURSE_CORE_RENDERER_RENDERER_HPP
#define CURSE_CORE_RENDERER_RENDERER_HPP

#include "Curse/Memory/Reference.hpp"
#include "Curse/System/Version.hpp"
#include "Curse/Renderer/Shader.hpp"
#include "Curse/Renderer/Texture.hpp"
#include <functional>

namespace Curse
{

    class WindowBase;


    /**
    * @brief Base class of renderer.
    */
    class CURSE_API Renderer
    {

    public:

        /**
        * @brief Types of renderers.
        */
        enum class BackendApi
        {
            OpenGL,
            Vulkan
        };

        /**
        * @brief Debug callback function.
        *        The function takes a const std::string & parameter for the debug message.
        */
        using DebugCallback = std::function<void(const std::string &)>;

        /**
        * @brief Static function for creating any renderer by Type.
        *        Make sure to open the renderer before using it.
        *
        * @return Pointer to renderer, nullptr if the type of renderer is unavailable.
        */
        static Renderer * Create(const BackendApi renderApi);

        /**
        * @brief Virtual destructor.
        */
        virtual ~Renderer();

        /**
        * @brief Opens renderer.
        *
        * @param window Render target window.
        */
        virtual void Open(const WindowBase& window, const Version& version = Version::None, DebugCallback debugCallback = nullptr) = 0;

        /**
        * @brief Closing renderer.
        */
        virtual void Close() = 0;

        /**
        * @brief Get backend API type.
        */
        virtual BackendApi GetBackendApi() const = 0;

        /**
        * @brief Get renderer API version.
        */
        virtual Version GetVersion() const = 0;

        /**
        * @brief Swap buffers.
        *        Necessary to do in order to present the frame, if double/tripple buffering is used.
        */
        virtual void SwapBuffers() = 0;

        /**
        * @brief Create shader object.
        */
        virtual Shader* CreateShader(const ShaderDescriptor & descriptor) = 0;

        /**
        * @brief Delete shader object.
        */
        virtual void DeleteShader(Shader* shader) = 0;

        /**
        * @brief Create texture object.
        */
        virtual Texture* CreateTexture() = 0;

        /**
        * @brief Delete texture object.
        */
        virtual void DeleteTexture(Texture* texture) = 0;

    };

}

#endif
