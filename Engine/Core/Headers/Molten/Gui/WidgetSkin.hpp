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

#ifndef MOLTEN_CORE_GUI_WIDGETSKIN_HPP
#define MOLTEN_CORE_GUI_WIDGETSKIN_HPP

namespace Molten::Gui
{

    template<typename TTheme>
    class WidgetData;

    template<typename TTheme, template<typename> typename TWidget>
    struct WidgetSkin;


    template<typename TTheme, template<typename> typename TWidget>
    struct WidgetSkinDescriptor
    {
        TTheme& theme;
        TWidget<TTheme>& widget;
        WidgetData<TTheme>& widgetData;
    };


    /** Widget skin base class. Widget skins should inherit from WidgetSkinMixin instead of this class template.*/
    class WidgetSkinBase
    {

    public:

        WidgetSkinBase() = default;
        virtual ~WidgetSkinBase() = default;

        WidgetSkinBase(const WidgetSkinBase&) = delete;
        WidgetSkinBase(WidgetSkinBase&&) = delete;
        WidgetSkinBase& operator =(const WidgetSkinBase&) = delete;
        WidgetSkinBase& operator =(WidgetSkinBase&&) = delete;

        virtual void Draw();

    };


    /** Base class for all widget skin types. */
    template<typename TTheme, template<typename> typename TWidget>
    class WidgetSkinMixin : public WidgetSkinBase
    {

    public:

        using State = typename TWidget<TTheme>::State;

        WidgetSkinMixin(const WidgetSkinDescriptor<TTheme, TWidget>& descriptor);
        ~WidgetSkinMixin() override = default;

        WidgetSkinMixin(const WidgetSkinMixin&) = delete;
        WidgetSkinMixin(WidgetSkinMixin&&) = delete;
        WidgetSkinMixin& operator =(const WidgetSkinMixin&) = delete;
        WidgetSkinMixin& operator =(WidgetSkinMixin&&) = delete;

        void SetState(const State& state);

        const State& GetState() const;

        virtual void OnStateChange(const State& state);

    protected:

        TTheme& theme; // Private plz
        TWidget<TTheme>& widget; // Private plz
        WidgetData<TTheme>& widgetData; // Private plz

    private:

        State m_state;

    };


}

#include "Molten/Gui/WidgetSkin.inl"

#endif