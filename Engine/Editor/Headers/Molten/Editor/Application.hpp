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

#ifndef MOLTEN_EDITOR_APPLICATION_HPP
#define MOLTEN_EDITOR_APPLICATION_HPP

#include "Molten/Logger.hpp"
#include "Molten/Editor/Editor.hpp"
#include <memory>

namespace Molten
{
    class Logger; 
}

namespace Molten::Editor
{

    class Editor;
    struct EditorDescriptor;

    /** Editor application class. */
    class Application
    {

    public:

        /** Constructor. */
        Application();

        /** Destructor. */
        ~Application() = default;

        /* Deleted operations. */
        /**@{*/
        Application(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator =(const Application&) = delete;
        Application& operator =(Application&&) = delete;
        /**@}*/

        /** Start editor application. */
        int Run(
            int argc,
            char** argv);

    private:

        bool Load(
            int argc,
            char** argv,
            EditorDescriptor& editorDescriptor);
     
        bool LoadSettings(
            int argc,
            char** argv,
            EditorDescriptor& editorDescriptor);

        void LoadLogger(const std::optional<std::string>& filename);

        int Start(const EditorDescriptor& editorDescriptor);

        std::shared_ptr<Logger> m_logger;
        std::unique_ptr<Editor> m_editor;

    };

}

#endif