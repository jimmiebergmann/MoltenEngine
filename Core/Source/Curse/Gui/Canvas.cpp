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

#include "Curse/Gui/Canvas.hpp"
#include "Curse/Gui/Templates/Padding.hpp"
#include "Curse/Logger.hpp"

#define CURSE_CANVAS_LOG(severity, message) if(m_logger){ m_logger->Write(severity, message); }

namespace Curse
{

    namespace Gui
    {

        Canvas::Canvas() :
            m_renderer(nullptr),
            m_logger(nullptr),
            m_rootWidget(nullptr)
        { }

        bool Canvas::Load(Renderer* renderer, Logger* logger)
        {
            m_renderer = renderer;
            m_logger = logger;

            m_context = std::make_unique<Private::Context>(Ecs::ContextDescriptor(64000));
            m_keyboardSystem = std::make_unique<KeyboardSystem>(*m_logger);
            m_mouseSystem = std::make_unique<MouseSystem>(*m_logger);

            m_context->RegisterSystem(*m_keyboardSystem.get());
            m_context->RegisterSystem(*m_mouseSystem.get());

            using RootType = Padding;

            auto rootWidget = m_context->CreateEntity<BaseWidget>();
            auto renderObject = std::make_unique<RenderObject>(m_renderer);
            RenderObjectTemplate<RootType>(*renderObject);
            auto widgetPointer = new Widget(
                rootWidget, 
                WidgetDescriptorTemplate<RootType>,
                std::move(renderObject));
            m_rootWidget = std::shared_ptr<Widget>(widgetPointer);
            rootWidget.GetComponent<BaseWidget>()->widget = m_rootWidget;

            return true;
        }

        void Canvas::Unload()
        {
            m_keyboardSystem.reset();
            m_mouseSystem.reset();
            m_context.reset();
            m_rootWidget.reset();
            //m_widgets.clear();
        }

        void Canvas::Update()
        {
            m_keyboardSystem->Process(Seconds(0.0f));
            m_mouseSystem->Process(Seconds(0.0f));
        }

        void Canvas::Draw()
        {
            /*for(auto& widget : m_widgets)
            {
               if (auto baseWidget = widget.GetComponent<BaseWidget>())
               {
                   if (baseWidget->OnDraw)
                   {
                       baseWidget->OnDraw(*baseWidget);
                   }
               }  
            }*/
        }

        bool Canvas::Move(WidgetPointer widget, WidgetPointer parent)
        {
            if (!widget || !parent || !parent->AllowsMoreChildren())
            {
                return false;
            }

            auto oldParent = widget->m_parent.lock();
            if (oldParent == parent)
            {
                return true;
            }

            auto it = std::find(oldParent->m_children.begin(), oldParent->m_children.end(), widget);
            if (it == oldParent->m_children.end())
            {
#if defined(CURSE_BUILD_DEBUG)
                CURSE_CANVAS_LOG(Logger::Severity::Error, "Widget move: Child is missing in parent Widget.");
#endif
                return false;
            }
            oldParent->m_children.erase(it);

            widget->m_parent = parent;
            parent->m_children.push_back(widget);

            return true;
        }

        WidgetPointer Canvas::GetRoot()
        {
            return m_rootWidget;
        }

        void Canvas::TraversalWidgetSizeUpdate(WidgetPointer startWidget)
        {
            struct Stack
            {
                std::vector<WidgetPointer>::iterator current;
                std::vector<WidgetPointer>::iterator last;
            };

            std::vector<Stack> stack =
            { 
                Stack
                {
                    startWidget->m_children.begin(),
                    startWidget->m_children.end()
                }
            };

            while (stack.size())
            {
                auto& top = stack.back();
                if (top.current == top.last)
                {
                    stack.pop_back();
                    continue;
                }

                auto current = *top.current;
                top.current++;

                if (!current->m_children.size())
                {
                    continue;
                }
            }

        }

    }

}