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

#include "Molten/Graphics/Gui/VisibilityWidget.hpp"

namespace Molten::Gui
{

    // Widget visibility tracker implementations.
    inline WidgetVisibilityTracker::WidgetVisibilityTracker() :
        m_containers{},
		m_currentContainer{ &m_containers[0] }
    {}

    inline void WidgetVisibilityTracker::RegisterVisibleWidget(VisibilityWidget* widget)
    {
        m_currentContainer->push_back(widget);
    }

    inline void WidgetVisibilityTracker::Update()
    {
        auto* otherContainer = m_currentContainer == &m_containers[0] ? &m_containers[1] : &m_containers[0];

        for(auto* visibileWidget : *m_currentContainer)
        {
            auto it = std::find(otherContainer->begin(), otherContainer->end(), visibileWidget);

            if(it == otherContainer->end())
            {
                visibileWidget->onShow();
            }
            else
            {
                otherContainer->erase(it);
            }

            visibileWidget->onIsVisible();
        }

        for(auto* notVisibleWidget : *otherContainer)
        {
            notVisibleWidget->onHide();
        }

        m_currentContainer = otherContainer;
        m_currentContainer->clear();
    }

}