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

#include "Curse/Gui/GuiControl.hpp"

namespace Curse
{

    namespace Gui
    {
        // Control implementations.       
        Control::~Control()
        { }

        bool Control::Add(Control&)
        {
            return false;
        }

        bool Control::Remove(Control&)
        {
            return false;
        }

        void Control::Release()
        {
            if (!m_parent)
            {
                return;
            }

            Control& parent = *m_parent;

            RemoveInternal(parent, *this);
            m_parent = nullptr;
        }

        Control* Control::GetParent()
        {
            return m_parent;
        }
        const Control* Control::GetParent() const
        {
            return m_parent;
        }

        Canvas* Control::GetCanvas()
        {
            return m_canvas;
        }
        const Canvas* Control::GetCanvas() const
        {
            return m_canvas;
        }

        Vector2f32 Control::GetPosiion() const
        {
            return m_position;
        }

        Vector2f32 Control::GetSize() const
        {
            return m_size;
        }

        Vector2f32 Control::GetDrawPosiion() const
        {
            return m_drawPosition;
        }

        Vector2f32 Control::GetDrawSize() const
        {
            return m_drawSize;
        }

        void Control::SetPosiion(const Vector2f32& position)
        {
            m_position = position;
        }
        void Control::SetSize(const Vector2f32& size)
        {
            m_size = size;
        }

        Control::Control() :
            m_parent(nullptr),
            m_canvas(nullptr)
        { }


        void Control::ReleaseInternal(Control& child)
        {
            child.ReleaseInternal();
        }
        void Control::ReleaseInternal()
        {
            m_parent = nullptr;
            m_canvas = nullptr;
        }

        bool Control::RemoveInternal(Control& parent, Control& child)
        {
            return parent.RemoveInternal(child);
        }
        bool Control::RemoveInternal(Control&)
        {
            return false;
        }

        void Control::SetParentInternal(Control& child, Control& parent)
        {
            child.SetParentInternal(parent);
        }
        void Control::SetParentInternal(Control& parent)
        {
            m_parent = &parent;
            if (m_parent)
            {
                SetCanvasInternal(m_parent->GetCanvas());
            }
        }

        void Control::SetCanvasInternal(Control& control, Canvas* canvas)
        {
            control.SetCanvasInternal(canvas);
        }
        void Control::SetCanvasInternal(Canvas* canvas)
        {
            auto oldCanvas = m_canvas;
            m_canvas = canvas;
            if (m_canvas != oldCanvas)
            {
                // Trigger canvas change.
                // Reload graphics...
                // ...
                // HERE.
            }
        }

        void Control::UpdateInternal(Control& control)
        {
            control.Update();
        }

        void Control::DrawInternal(Control& control) const
        {
            control.Draw();
        }

        void Control::SetDrawPositionInternal(Control& control, const Vector2f32& position)
        {
            control.m_drawPosition = position;
        }

        void Control::SetDrawSizeInternal(Control& control, const Vector2f32& size)
        {
            control.m_drawSize = size;
        }


        // Parent control array 1D implementations.
        ParentControlList::~ParentControlList()
        {
            for (auto* child : m_childs)
            {
                delete child;
            }
        }

        bool ParentControlList::Add(Control& control)
        {
            Control* controlPtr = &control;

            for (auto* child : m_childs)
            {
                if (child == controlPtr)
                {
                    return true;
                }
            }

            Control::SetParentInternal(control, *this);
            Control::SetCanvasInternal(control, GetCanvas());
            m_childs.push_back(controlPtr);
            return true;
        }

        bool ParentControlList::Remove(Control& control)
        {
            if (RemoveInternal(control))
            {
                Control::ReleaseInternal(control);
                return true;
            }
            return false;
        }

        ParentControlList::ParentControlList()
        {
        }

        bool ParentControlList::RemoveInternal(Control& control)
        {
            Control* controlPtr = &control;

            for (auto it = m_childs.begin(); it != m_childs.end(); it++)
            {
                if (*it == controlPtr)
                {
                    m_childs.erase(it);
                    return true;
                }
            }

            return false;
        }

        void ParentControlList::SetCanvasInternal(Canvas* canvas)
        {
            Control::SetCanvasInternal(canvas);

            for (auto* child : m_childs)
            {
                Control::SetCanvasInternal(*child, canvas);
            }
        }

        std::list<Control*>& ParentControlList::GetChilds()
        {
            return m_childs;
        }
        const std::list<Control*>& ParentControlList::GetChilds() const
        {
            return m_childs;
        }

    }

}