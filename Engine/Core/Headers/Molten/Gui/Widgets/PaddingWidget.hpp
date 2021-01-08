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

#ifndef MOLTEN_CORE_GUI_WIDGETS_PADDINGWIDGET_HPP
#define MOLTEN_CORE_GUI_WIDGETS_PADDINGWIDGET_HPP

#include "Molten/Gui/Widget.hpp"

namespace Molten::Gui
{

    /*template<typename TSkin>
    class Padding : public Widget<TSkin>, public PaddingData
    {

    public:

        static constexpr bool handleKeyboardEvents = false;
        static constexpr bool handleMouseEvents = false;

        explicit Padding(
            TSkin& skin,
            const float left = 0.0f,
            const float top = 0.0f,
            const float right = 0.0f,
            const float bottom = 0.0f);

        void Update(const Time& deltaTime) override;

        //void Draw(CanvasRenderer& renderer) override;

        //Vector2f32 CalculateSize(const Vector2f32& grantedSize) override;

       // void CalculateChildrenGrantedSize(typename WidgetTreeData<TSkin>::Tree::template ConstLane<typename WidgetTreeData<TSkin>::Tree::PartialLaneType> children) override;

    private:

        bool OnAddChild(WidgetPointer<TSkin>) override;

    };*/

}

#include "Molten/Gui/Widgets/PaddingWidget.inl"

#endif