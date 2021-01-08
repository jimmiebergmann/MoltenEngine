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

#ifndef MOLTEN_CORE_GUI_SKIN_HPP
#define MOLTEN_CORE_GUI_SKIN_HPP

#include "Molten/Math/Vector.hpp"
#include "Molten/Math/Bounds.hpp"
#include <memory>

namespace Molten::Gui
{
    enum class WidgetSkinStateType
    {
        Normal,
        Disabled,
        Hovered,
        Pressed
    };

    class WidgetSkinBase
    {

    public:

        WidgetSkinBase() :
            m_state(WidgetSkinStateType::Normal)
        {}

        virtual ~WidgetSkinBase() = default;

        virtual void Draw()
        {

        }

        virtual void Update()
        {
        }

        void SetState(const WidgetSkinStateType state)
        {
            m_state = state;
        }

        WidgetSkinStateType GetState() const
        {
            return m_state;
        }

        const Bounds2f32& GetGrantedBounds()
        {
            return m_grantedBounds;
        }

        void SetGrantedBounds(const Bounds2f32 grantedBounds)
        {
            m_grantedBounds = grantedBounds;
        }   
        
    protected:

        Bounds2f32& CalculateBounds(const Bounds2f32& margin)
        {
            m_grantedBounds = m_grantedBounds.RemoveMargins(margin);
            m_grantedBounds.ClampHighToLow();
            return m_grantedBounds;
        }

        Bounds2f32 CalculateContentBounds(const Bounds2f32& padding)
        {
            auto contentBounds = m_grantedBounds.RemoveMargins(padding);
            contentBounds.ClampHighToLow();
            return contentBounds;
        }

        WidgetSkinStateType m_state;
        Bounds2f32 m_grantedBounds;

    };

}

#endif