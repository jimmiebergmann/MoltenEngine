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

namespace Molten::Gui
{

    // Menu overlay implementations.
    template<typename TTheme>
    MenuOverlay<TTheme>::MenuOverlay(WidgetMixinDescriptor<TTheme, MenuOverlay>& desc) :
        WidgetMixin<TTheme, MenuOverlay>(desc)
    {}

    template<typename TTheme>
    size_t MenuOverlay<TTheme>::AddDivider()
    {
        m_dividers.emplace_back();
        return 0;
    }

    template<typename TTheme>
    bool MenuOverlay<TTheme>::RemoveDivider(const size_t index)
    {
	    if(index >= m_dividers.size())
	    {
            return false;
	    }

        m_dividers.erase(m_dividers.begin() + index);
        return true;
    }

    template<typename TTheme>
    void MenuOverlay<TTheme>::OnUpdate(WidgetUpdateContext<TTheme>& /*updateContext*/)
    {
        /*using Skin = typename WidgetMixin<TTheme, MenuOverlay>::WidgetSkinType;

        float itemPosition = 0.0f;
        for (auto& child : this->GetChildren())
        {
            this->SetPosition(child, { 0.0f, itemPosition });


            itemPosition += Skin::itemHeight;
        }*/
    }

}