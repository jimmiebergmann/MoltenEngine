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

#include "Curse/Types.hpp"
#include <list>

namespace Curse
{

    namespace Gui
    {

        /**
        * @brief GUI control class.
        *        Base class of all GUI controls.
        */
        class CURSE_API Control
        {

        public:

            /**
            * @brief Constructor.
            */
            Control();

            /**
            * @brief Virtual destructor.
            */
            virtual ~Control();

            /**
            * @brief Update the control.
            *        Prepare for rendering and handling input events.
            */
            virtual void Update() = 0;

            /**
            * @brief Render the control.
            */
            virtual void Render() const = 0;

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

        protected:

            void ReleaseInternal(Control& child);
            void ReleaseInternal();

            bool RemoveInternal(Control& parent, Control& child);
            virtual bool RemoveInternal(Control& child);

            void SetParentInternal(Control& child, Control& parent);
            void SetParentInternal(Control& parent);

        private:

            Control* m_parent;

        };


        /**
        * @brief Parent control, storing child controls in a dynamic 1D array.
        */
        class CURSE_API ParentControlArray1D : public Control
        {

        public:

            /**
            * @brief Destructor.
            */
            virtual ~ParentControlArray1D();

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

            virtual bool RemoveInternal(Control& child) override;

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
