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

#ifndef MOLTEN_CORE_GUI_WIDGETS_DOCKERWIDGET_HPP
#define MOLTEN_CORE_GUI_WIDGETS_DOCKERWIDGET_HPP

#include "Molten/Gui/DockingWidget.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include <memory>
#include <vector>
#include <set>
#include <limits>
#include <variant>
#include <algorithm>
#include <type_traits>

// TEMP
#include "Molten/Gui/Widgets/PaneWidget.hpp"

namespace Molten::Gui
{

    template<typename TSkin>
    class Docker : public Widget<TSkin>, public WidgetEventHandler
    {

    public:

        static constexpr bool handleKeyboardEvents = false;
        static constexpr bool handleMouseEvents = true;

        explicit Docker(WidgetData<TSkin>& data);

        void Update() override;

        bool HandleEvent(const WidgetEvent& widgetEvent) override;

    private:
  
        struct Element;
        struct WidgetElement;
        struct GridElement;
        struct Edge;

        enum class Direction
        {
            Horizontal,
            Vertical
        };

        enum class ElementPosition
        {
            First,
            Last
        };

        enum class ElementType
        {
            Widget,
            Grid
        };

        struct WidgetElement
        {
            explicit WidgetElement(WidgetData<TSkin>& widgetData);

            WidgetData<TSkin>* widgetData;
            bool dynamicSize;
            Vector2f32 size;

        private:

            static bool IsDynamicSize(WidgetData<TSkin>& widgetData);
        };

        struct GridElement
        {
            using Elements = std::vector<std::unique_ptr<Element>>;

            GridElement(Direction direction);

            template<typename TElementValue>
            void PushBackElement(TElementValue&& elementValue);

            template<typename TElementValue>
            std::pair<Element*, Element*> InsertElement(TElementValue&& elementValue, const ElementPosition position);

            Direction direction;
            Elements elements;
        };

        struct Element
        {
            Element(WidgetElement* widgetElement);
            Element(std::unique_ptr<GridElement>&& gridElement);

            using Type = std::variant<WidgetElement*, std::unique_ptr<GridElement>>;

            const ElementType type;
            Type data;
            Edge* prevEdge;
            Edge* nextEdge;
        };

        struct Edge
        {
            Edge(
                Direction direction, 
                Element* prevElement,
                Element* nextElement);

            Bounds2f32 bounds;
            Direction direction;           
            Element* prevElement;
            Element* nextElement;
        };


        void OnAddChild(WidgetData<TSkin>& childData) override;

        bool HandleMouseEvent(const WidgetEventSubType subType, const WidgetEvent::MouseEvent& mouseEvent);

        void UpdateBounds();

        static void UpdateGridElementBounds(GridElement& gridElement, const Bounds2f32& grantedBounds);
        template<Direction VDirection>
        static void UpdateDirectionalGridElementBounds(GridElement& gridElement, const Bounds2f32& grantedBounds);
        //template<Direction VDirection>
        //static float GetDirectionalBoundsSize(const Bounds2f32& bounds);
        static void UpdateWidgetElementBounds(WidgetElement& widgetElement, const Bounds2f32& grantedBounds);

        static constexpr Direction FlipDirection(const Direction direction);
        static constexpr Direction GetInsertDirection(DockingPosition position);
        static constexpr ElementPosition GetInsertPosition(DockingPosition position);

        template<Direction VDirection>
        static constexpr float GetDirectionalSizeElement(const Vector2f32& size);

        template<Direction VDirection>
        static Bounds2f32 CutDirectionalHighBounds(Bounds2f32& grantedBounds, const float size);

        Edge* FindIntersectingEdge(const Vector2f32& point);  

        Bounds2f32 m_oldGrantedBounds;
        std::unique_ptr<GridElement> m_rootGrid;
        std::set<std::unique_ptr<Edge>> m_edges;
        std::set<std::unique_ptr<WidgetElement>> m_widgetElements;
        Edge* m_pressedEdge;
        Vector2f32 m_prevMousePosition;

        friend struct TSkin::template WidgetSkin<Docker<TSkin>>;

    };

}

#include "Molten/Gui/Widgets/DockerWidget.inl"

#endif