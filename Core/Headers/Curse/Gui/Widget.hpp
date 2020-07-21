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

#ifndef CURSE_CORE_GUI_WIDGET_HPP
#define CURSE_CORE_GUI_WIDGET_HPP

#include "Curse/Gui/WidgetDescriptor.hpp"
#include "Curse/Gui/RenderObject.hpp"
#include "Curse/Ecs/EcsEntity.hpp"
#include "Curse/Gui/Context.hpp"
#include <memory>

namespace Curse::Gui
{
    class Canvas;
    class Widget;

    using WidgetPointer = std::shared_ptr<Widget>;
    using WidgetPointerWeak = std::weak_ptr<Widget>;
    using WidgetEntity = Ecs::Entity<Ecs::Context<Private::Context>>;

    struct CURSE_API WidgetCache
    {
        Vector2f32 assignedSize;
    };

    /*
        * @breif
        * @tparam T Widget data type.
    */
    class CURSE_API Widget : public WidgetDescriptor
    {

    public:

        ~Widget();

        template<typename ... Components>
        void AddComponents();

        template<typename ... Components>
        void RemoveComponents();

        template<typename Component>
        Component* GetComponent();

        bool AllowsMoreChildren();

        const WidgetCache& GetCache() const;

    protected:

        Widget(
            WidgetEntity entity,
            const WidgetDescriptor& descriptor,
            std::unique_ptr<RenderObject> renderObject);

    private:

        friend class Canvas;

        Widget(const Widget&) = delete;
        Widget(Widget&&) = delete;
        Widget& operator = (const Widget&) = delete;
        Widget& operator = (Widget&&) = delete;

        WidgetEntity m_entity;
        std::unique_ptr<RenderObject> m_renderObject;
        WidgetPointerWeak m_parent;
        std::vector<WidgetPointer> m_children;
        WidgetCache m_cache;

    };

}

#include "Curse/Gui/Widget.inl"

#endif