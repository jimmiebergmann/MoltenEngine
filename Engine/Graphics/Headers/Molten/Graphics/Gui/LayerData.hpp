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

#ifndef MOLTEN_GRAPHICS_GUI_LAYERDATA_HPP
#define MOLTEN_GRAPHICS_GUI_LAYERDATA_HPP

#include "Molten/Utility/BypassList.hpp"
#include <memory>

namespace Molten::Gui
{
    template<typename TTheme>
    class Canvas;

    template<typename TTheme>
    class Layer;
    template<typename TTheme, template<typename> typename TLayer>
    class LayerMixin;

    template<typename TTheme>
    class LayerData
    {

    public:

        using List = BypassList<std::unique_ptr<LayerData<TTheme>>>;
        using ListNormalLaneType = typename List::NormalLaneType;
        using ListPartialLaneType = typename List::PartialLaneType;
        using ListPartialLane = typename List::template Lane<ListPartialLaneType>;
        using ListNormalLane = typename List::template Lane<ListNormalLaneType>;
        using ListNormalIterator = typename List::template Iterator<ListNormalLaneType>;
        using ListNormalConstIterator = typename List::template ConstIterator<ListNormalLaneType>;
        using ListPartialIterator = typename List::template Iterator<ListPartialLaneType>;
        using ListPartialConstIterator = typename List::template ConstIterator<ListPartialLaneType>;

        explicit LayerData(Canvas<TTheme>* canvas);
        virtual ~LayerData() = default;

        LayerData(const LayerData&) = delete;
        LayerData(LayerData&&) = delete;
        LayerData& operator= (const LayerData&) = delete;
        LayerData& operator= (LayerData&&) = delete;    

        [[nodiscard]] Canvas<TTheme>* GetCanvas();
        [[nodiscard]] const Canvas<TTheme>* GetCanvas() const;

        [[nodiscard]] List* GetList();
        [[nodiscard]] const List* GetList() const;

        [[nodiscard]] ListNormalIterator GetListNormalIterator();
        [[nodiscard]] ListNormalConstIterator GetListNormalIterator() const;

        [[nodiscard]] ListPartialIterator GetListPartialIterator();
        [[nodiscard]] ListPartialConstIterator GetListPartialIterator() const;

        [[nodiscard]] Layer<TTheme>* GetLayer();
        [[nodiscard]] const Layer<TTheme>* GetLayer() const;

    protected:

        template<typename TThemeOther>
        friend class Canvas;

        void Initialize(
            List* list,
            ListNormalIterator iterator,
            std::unique_ptr<Layer<TTheme>> && layer);

    private:

        Canvas<TTheme>* m_canvas;
        List* m_list;
        ListNormalIterator m_listIterator;
        std::unique_ptr<Layer<TTheme>> m_layer;

    };

}

#include "Molten/Graphics/Gui/LayerData.inl"

#endif