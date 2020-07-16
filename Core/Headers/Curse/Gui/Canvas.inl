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

namespace Curse
{

    namespace Gui
    {

        template<typename System>
        inline void Canvas::RegisterSystem(System& system)
        {
            m_context->RegisterSystem(system);
        }

        template<typename Template, typename ... Behaviors, typename ... ConstructorArgs>
        inline TemplatedWidgetPointer<Template> Canvas::Add(WidgetPointer parent, ConstructorArgs ... args)
        {
            if (!parent || !parent->AllowsMoreChildren())
            {
                return nullptr;
            }

            auto widgetEntity = m_context->CreateEntity<BaseWidget, Behaviors...>();
            auto renderObject = std::make_unique<RenderObject>(m_renderer);
            RenderObjectTemplate<Template>(*renderObject);
            auto widgetPointer = new TemplatedWidget<Template>(
                widgetEntity,
                WidgetDescriptorTemplate<Template>,
                std::move(renderObject),
                args...);
            auto widget = TemplatedWidgetPointer<Template>(widgetPointer);
            widgetEntity.GetComponent<BaseWidget>()->widget = widget;

            parent->m_children.push_back(widget);
            widget->m_parent = parent;
            return widget;
        }

    }

}