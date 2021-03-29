/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

    // Widget size implementations.
    constexpr WidgetSize::WidgetSize() :
        type(WidgetSizeType::Fixed)
    {}

    constexpr WidgetSize::WidgetSize(const ValueType& value) :
        type(WidgetSizeType::Fixed),
        value(value)
    {}

    constexpr WidgetSize::WidgetSize(const float x, const float y) :
        type(WidgetSizeType::Fixed),
        value(x, y)
    {}

    constexpr WidgetSize::WidgetSize(const WidgetSizeType type, const ValueType & value) :
        type(type),
        value(value)
    {}

    inline WidgetSize::ValueType WidgetSize::CalculateValue(const ValueType & fitSize, const ValueType & occupySize) const
    {
        switch(type)
        {
            case WidgetSizeType::Fit: return fitSize;
            case WidgetSizeType::Occupy: return occupySize;
            default: break;
        }
        return value;
    }

}