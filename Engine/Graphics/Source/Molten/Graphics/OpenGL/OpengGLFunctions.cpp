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


#include "Molten/Graphics/OpenGL/OpengGLFunctions.hpp"

#if defined(MOLTEN_ENABLE_OPENGL)

#if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS
    #define GetProcAddress(ext) ::wglGetProcAddress(ext)
#elif MOLTEN_PLATFORM == MOLTEN_PLATFORM_LINUX
    #define Bool bool
    #define GetProcAddress(ext) glXGetProcAddress((const GLubyte *)ext)
#endif

#include "Molten/System/Exception.hpp"

namespace Molten
{

    namespace OpenGL
    {

        PFNGLGETSTRINGIPROC GetStringi = NULL;

        PFNGLBINDVERTEXARRAYPROC BindVertexArray = NULL;
        PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays = NULL;
        PFNGLGENVERTEXARRAYSPROC GenVertexArrays = NULL;
        PFNGLISVERTEXARRAYPROC IsVertexArray = NULL;

        bool BindOpenGLExtensions()
        {
            bool error = false;

        #if MOLTEN_PLATFORM == MOLTEN_PLATFORM_WINDOWS

            error |= (GetStringi = (PFNGLGETSTRINGIPROC)GetProcAddress("glGetStringi")) == NULL;
            if (error)
            {
                if (::GetLastError() == ERROR_INVALID_HANDLE)
                {
                    throw Exception("Cannot bind OpenGL extensions. No context is current.");
                }
            }

            error |= (BindVertexArray = (PFNGLBINDVERTEXARRAYPROC)GetProcAddress("glBindVertexArray")) == NULL;
            error |= (DeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)GetProcAddress("glDeleteVertexArrays")) == NULL;
            error |= (GenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)GetProcAddress("glGenVertexArrays")) == NULL;
            error |= (IsVertexArray = (PFNGLISVERTEXARRAYPROC)GetProcAddress("glIsVertexArray")) == NULL;

        #endif

            return !error;
        }

    }

}

#endif
