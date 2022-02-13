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
    template<typename TTheme>
    Viewport<TTheme>::Viewport(WidgetMixinDescriptor<TTheme, Viewport>& desc) :
        WidgetMixin<TTheme, Viewport>(desc),
        VisibilityWidget(desc.visibilityTracker),
        m_prevSize{ 0.0f, 0.0f }
    {}

    template<typename TTheme>
    void Viewport<TTheme>::SetTexture(SharedRenderResource<FramedTexture2D> framedTexture)
    {
        Mixin::SetSkinState(State{ std::move(framedTexture) });
    }

    template<typename TTheme>
    void Viewport<TTheme>::OnUpdate(WidgetUpdateContext<TTheme>& updateContext)
    {
        this->PreCalculateBounds();

        const auto contentSize = this->GetBounds().size;

        VisibilityWidget::PostUpdate(contentSize);

        if (contentSize != m_prevSize)
        {
            m_prevSize = contentSize;
            onResize(contentSize);
        }
    }

    template<typename TTheme>
    bool Viewport<TTheme>::OnMouseEvent(const WidgetMouseEvent&)
    {
        return false;
    }

}
