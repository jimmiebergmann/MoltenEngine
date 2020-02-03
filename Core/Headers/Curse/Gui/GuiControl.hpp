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

#ifndef CURSE_CORE_GUI_GUICONTROL_HPP
#define CURSE_CORE_GUI_GUICONTROL_HPP

#include "Curse/Math/Vector.hpp"
#include <list>

namespace Curse
{

    namespace Gui
    {

        // Forward declarations.
        class Canvas;

        /**
        * @brief GUI control class.
        *        Base class of all GUI controls.
        */
        class CURSE_API Control
        {

        public:

            /**
            * @brief Virtual destructor.
            */
            virtual ~Control();

            /**
            * @brief Add child control to this control.
            *
            * @return true if child control is accepted and added, else false.
            */
            virtual bool Add(Control & child);

            /**
            * @brief Remove child control from this control and
            *        sets parent control free from responsibility of deallocating child.
            *
            * @return true if parent removed the child, false if child is unkown to parent.
            */
            virtual bool Remove(Control& child);

            /**
            * @brief Release from parent.
            *        Sets parent free from responsibility of deallocating this control.
            */
            void Release();

            /**
            * @brief Get parent control.
            *
            * @return Pointer of parent control, nullptr if control has no parent.
            */
            Control * GetParent();

            /**
            * @brief Get parent control.
            *
            * @return Pointer of parent control, nullptr if control has no parent.
            */
            const Control* GetParent() const;

            /**
            * @brief Get parent canvas.
            *
            * @return Pointer of parent canvas, nullptr if this control doesn't belong to any canvas.
            */
            Canvas* GetCanvas();

            /**
            * @brief Get parent canvas.
            *
            * @return Pointer of parent canvas, nullptr if this control doesn't belong to any canvas.
            */
            const Canvas* GetCanvas() const;

            /**
            * @brief Get relative position to parent control. Relative to origin if control is missing a parent.
            */
            Vector2f32 GetPosiion() const;

            /**
            * @brief Get control size.
            *        This is not the actual draw size, but preferred size, set by the user.
            *        The final render size is determined by the parent control.
            */
            Vector2f32 GetSize() const;

            /**
            * @brief Get actual draw posion of control, relative to origin.
            */
            Vector2f32 GetDrawPosiion() const;

            /**
            * @brief Get actual draw size of control.
            */
            Vector2f32 GetDrawSize() const;

            /**
            * @brief Set relative position.
            */
            void SetPosiion(const Vector2f32 & position);

            /**
            * @brief Set control size.
            */
            void SetSize(const Vector2f32& size);

        protected:

            /**
            * @brief Protected constructor.
            */
            Control();

            /**
            * @brief Update the control.
            *        Prepare for rendering and handling input events.
            */
            virtual void Update() = 0;

            /**
            * @brief Render the control.
            */
            virtual void Draw() const = 0;


            void ReleaseInternal(Control& child);
            void ReleaseInternal();

            bool RemoveInternal(Control& parent, Control& child);
            virtual bool RemoveInternal(Control& child);

            void SetParentInternal(Control& child, Control& parent);
            void SetParentInternal(Control& parent);

            void SetCanvasInternal(Control& control, Canvas * canvas);
            virtual void SetCanvasInternal(Canvas* canvas);

            void UpdateInternal(Control& control);
            void DrawInternal(Control& control) const;

            void SetDrawPositionInternal(Control& control, const Vector2f32& position);
            void SetDrawSizeInternal(Control& control, const Vector2f32& size);

        private:

            friend class Canvas;

            Control* m_parent;
            Canvas* m_canvas;       
            Vector2f32 m_position;
            Vector2f32 m_size;
            Vector2f32 m_drawPosition;
            Vector2f32 m_drawSize;

        };


        /**
        * @brief Parent control, storing child controls in a dynamic list.
        */
        class CURSE_API ParentControlList : public Control
        {

        public:

            /**
            * @brief Destructor.
            */
            virtual ~ParentControlList();

            /**
            * @brief Add child control to this control.
            *        The parent control is responsible of deallocating the control, until removed.
            *
            * @return true if child control is accepted and added, else false.
            */
            virtual bool Add(Control& child) override;

            /**
            * @brief Remove child control from this control and
            *        sets parent control free from responsibility of deallocating child.
            *
            * @return true if parent removed the child, false if child is unkown to parent.
            */
            virtual bool Remove(Control& child) override;

        protected:

            /**
            * @brief Protected constructor.
            */
            ParentControlList();

            virtual bool RemoveInternal(Control& child) override;

            virtual void SetCanvasInternal(Canvas* canvas) override;

            /**
            * @brief Get list of childs.
            */
            std::list<Control*>& GetChilds();

            /**
            * @brief Get list of childs.
            */
            const std::list<Control*>& GetChilds() const;

        private:

            std::list<Control*> m_childs;

        };

    }

}

#endif
