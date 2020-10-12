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

#ifndef MOLTEN_CORE_GUI_ROOTLAYER_HPP
#define MOLTEN_CORE_GUI_ROOTLAYER_HPP

#include "Molten/Gui/GuiTypes.hpp"
#include "Molten/Gui/Layer.hpp"
#include "Molten/Gui/WidgetData.hpp"
#include "Molten/Gui/Widget.hpp"
#include "Molten/Math/Vector.hpp"
#include "Molten/System/Time.hpp"

namespace Molten::Gui
{

    class MOLTEN_API RootLayer : public Layer
    {

    public:

        RootLayer(Canvas& canvas);

        ~RootLayer() = default;

        virtual void Update(const Time& deltaTime) override;

        virtual void Draw(CanvasRenderer & renderer) override;

        virtual void Resize(const Vector2f32& size) override;

        virtual void SetScale(const Vector2f32& scale) override;

        virtual bool OnAddChild(WidgetPointer parent, WidgetPointer child) override;

    private:        

        WidgetTreeData::Tree m_widgetTree;
        Vector2f32 m_size;
        Vector2f32 m_scale;

    };

}


#endif