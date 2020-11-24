/*
* MIT License
*
* Copyright (c) 2020 Jimmie Bergmann
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
#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Scene/Camera.hpp"
#include "Molten/System/Clock.hpp"
#include "Molten/Window/Window.hpp"
#include "Molten/Gui/Canvas.hpp"
#include <memory>

namespace Molten::Editor
{

    /** Editor application class. */
    class Application
    {

    public:

        /** Constructor. */
        Application();

        /** Destructor. */
        ~Application();

        /** Start editor application. */
        int Start(int argv, char** argc);

    private:

        void Load();
        
        void LoadGui();
        void LoadGridPipeline();
        void LoadGridShaders();
        void LoadObjectPipeline();
        void LoadObjectShaders();

        void Unload();
        void UnloadGridPipeline();
        void UnloadObjectPipeline();

        void Tick();

        bool Update();

        void Draw();            

        Logger m_logger;
        std::unique_ptr<Window> m_window;
        std::unique_ptr <Renderer> m_renderer;

        FramedUniformBuffer* m_viewMatrixBuffer;

        Pipeline* m_gridPipeline;
        Shader::Visual::VertexScript m_gridVertexScript;
        Shader::Visual::FragmentScript m_gridFragmentScript;
        VertexBuffer* m_gridVertexBuffer;
        IndexBuffer* m_gridIndexBuffer;
        FramedDescriptorSet* m_gridMatrixDescriptorSet;
        uint32_t m_gridColor1PushLocation;
        uint32_t m_gridColor2PushLocation;

        Pipeline* m_objectPipeline;
        Shader::Visual::VertexScript m_objectVertexScript;
        Shader::Visual::FragmentScript m_objectFragmentScript;
        VertexBuffer* m_objectVertexBuffer;
        IndexBuffer* m_objectIndexBuffer;
        FramedDescriptorSet* m_objectMatrixDescriptorSet;
        DescriptorSet* m_objectTextureDescriptorSet;
        uint32_t m_objectPosPushLocation;
        uint32_t m_objectColorPushLocation;
        Texture* m_objectTexture;
  
        Clock m_programTimer;
        float m_programTime;
        Clock m_secondTimer;
        Clock m_deltaTimer;
        float m_deltaTime;

        Scene::Camera m_camera;

        //Gui::Canvas m_guiCanvas;

        Gui::CanvasRendererPointer m_canvasRenderer;
        Gui::CanvasPointer m_canvas;


    };

}

#endif