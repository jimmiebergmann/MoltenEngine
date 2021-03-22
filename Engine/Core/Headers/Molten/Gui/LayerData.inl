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

    // Layer data implementations.
    template<typename TTheme>
    LayerData<TTheme>::LayerData(Canvas<TTheme>* canvas) :
        m_canvas(canvas),
        m_list(nullptr)
    {}

    template<typename TTheme>
    Canvas<TTheme>* LayerData<TTheme>::GetCanvas()
    {
        return m_canvas;
    }

    template<typename TTheme>
    const Canvas<TTheme>* LayerData<TTheme>::GetCanvas() const
    {
        return m_canvas;
    }

    template<typename TTheme>
    typename LayerData<TTheme>::List* LayerData<TTheme>::GetList()
    {
        return m_list;
    }
    template<typename TTheme>
    const typename LayerData<TTheme>::List* LayerData<TTheme>::GetList() const
    {
        return m_list;
    }

    template<typename TTheme>
    typename LayerData<TTheme>::ListNormalIterator LayerData<TTheme>::GetListNormalIterator()
    {
        return m_listIterator;
    }
    template<typename TTheme>
    typename LayerData<TTheme>::ListNormalConstIterator LayerData<TTheme>::GetListNormalIterator() const
    {
        return m_listIterator;
    }

    template<typename TTheme>
    typename LayerData<TTheme>::ListPartialIterator LayerData<TTheme>::GetListPartialIterator()
    {
        return m_listIterator;
    }
    template<typename TTheme>
    typename LayerData<TTheme>::ListPartialConstIterator LayerData<TTheme>::GetListPartialIterator() const
    {
        return m_listIterator;
    }

    template<typename TTheme>
    Layer<TTheme>* LayerData<TTheme>::GetLayer()
    {
        return m_layer.get();
    }
    template<typename TTheme>
    const Layer<TTheme>* LayerData<TTheme>::GetLayer() const
    {
        return m_layer.get();
    }

    template<typename TTheme>
    void LayerData<TTheme>::Initialize(
        List* list,
        ListNormalIterator iterator,
        std::unique_ptr<Layer<TTheme>>&& layer)
    {
        m_list = list;
        m_listIterator = iterator;
        m_layer = std::move(layer);
    }

}