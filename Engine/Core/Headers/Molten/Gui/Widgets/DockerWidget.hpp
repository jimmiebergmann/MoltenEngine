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

#include "Molten/Gui/Widget.hpp"
#include "Molten/Gui/WidgetEvent.hpp"
#include "Molten/Gui/DraggableWidget.hpp"
#include "Molten/System/Signal.hpp"
#include <memory>
#include <vector>
#include <map>
#include <queue>
#include <list>
#include <limits>
#include <variant>
#include <algorithm>
#include <type_traits>

namespace Molten::Gui
{

    enum class DockingPosition
    {
        Left,
        Right,
        Top,
        Bottom
    };

    template<typename TSkin>
    class Docker : public WidgetMixin<TSkin, Docker<TSkin>>, public WidgetEventHandler
    {

    public:

        struct State
        {
            enum class Type
            {
                Normal,
                MoveEdge
            };

            Type type;
        };

        static constexpr bool overrideChildrenMouseEvents = true;
        static constexpr bool handleKeyboardEvents = false;
        static constexpr bool handleMouseEvents = true;

        float edgeWidth = 10.0f;
        float spacing = 10.0f;

        Signal<Mouse::Cursor> onCursorChange;

        explicit Docker(WidgetData<TSkin>& data);

        template<template<typename> typename TWidgetType, typename ... TArgs>
        WidgetTypePointer<TWidgetType<TSkin>> CreateChild(
            const DockingPosition position,
            const bool dynamic,
            TArgs ... args);

        void Update() override;
        bool HandleEvent(const WidgetEvent& widgetEvent) override;
        void OnAddChild(WidgetData<TSkin>& childData) override;

    private:

        using Widget<TSkin>::CreateChild;

        struct Element;
        struct Leaf;
        struct Grid;
        struct Edge;
        struct PendingLeafInsert;
        struct CalculatedGridResult;

        using ElementPointer = std::unique_ptr<Element>;
        using ElementPointerList = std::list<ElementPointer>;
        using LeafPointer = std::unique_ptr<Leaf>;
        using GridPointer = std::unique_ptr<Grid>;
        using EdgePointer = std::unique_ptr<Edge>;
        using PendingLeafInsertPointer = std::shared_ptr<PendingLeafInsert>;

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
            Leaf,
            Grid
        };

        struct Leaf
        {
            Leaf(
                WidgetData<TSkin>* widgetData,
                const bool isDynamic);

            bool IsDynamic() const;

            WidgetData<TSkin>* widgetData;
            DraggableWidget* draggableWidget;
            bool isDynamic;
        };

        struct Grid
        {
            explicit Grid(Direction direction);

            std::pair<Element*, Element*> InsertElement(ElementPointer&& element, const ElementPosition position);

            bool IsDynamic() const;

            Direction direction;
            ElementPointerList elements;
            CalculatedGridResult calculatedResult;
        };

        struct Element
        {
            using Type = std::variant<LeafPointer, GridPointer>;

            explicit Element(LeafPointer&& leaf);
            explicit Element(GridPointer&& grid); 

            bool IsDynamic() const;

            Vector2f32 size;
            Vector2f32 calculatedSize;
            Bounds2f32 bounds;

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

            Mouse::Cursor GetSizeCursor() const;

            Bounds2f32 bounds;
            Direction direction;
            Element* prevElement;
            Element* nextElement;
        };

        struct CalculatedGridResult
        {
            explicit CalculatedGridResult(Grid& grid);

            void Clear();

            using ElementIterator = typename ElementPointerList::reverse_iterator;

            Grid& grid;
            Bounds2f32 grantedBounds;
            Vector2f32 grantedSize;
            float totalStaticWidth;
            float totalDynamicWidth;
            std::vector<Element*> staticElements;
            std::vector<Element*> dynamicElements;
            ElementIterator activeElementsBegin;
            ElementIterator activeElementsEnd;
        };

        struct PendingLeafInsert
        {
            PendingLeafInsert(
                const DockingPosition position,
                const bool isDynamic,
                WidgetData<TSkin>* widgetData);

            DockingPosition position;
            bool isDynamic;
            WidgetData<TSkin>* widgetData;
        };

        /** Child insert or deletion functions. */
        /**@{*/
        bool InsertNewLeafs();
        void InsertLeaf(PendingLeafInsert& pendingLeaf);

        void InsertElementInGrid(
            GridPointer& grid,
            ElementPointer&& element,
            const ElementPosition insertPosition);

        ElementPointer TransformElementToGrid(
            ElementPointer&& oldElement,
            ElementPointer&& newElement,
            const Direction gridDirectionn,
            const ElementPosition insertPosition);

        static constexpr Direction GetInsertDirection(DockingPosition position);
        static constexpr ElementPosition GetInsertPosition(DockingPosition position);
        /**@}*/

        /** Input handling functions. */
        /**@{*/
        void SetCursor(const Mouse::Cursor cursor);

        bool HandleMouseEvent(const WidgetEventSubType subType, const WidgetEvent::MouseEvent& mouseEvent);
        bool HandleMousePressEvent(const WidgetEvent::MouseEvent& mouseEvent);
        bool HandleMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent);
        bool HandleMouseReleaseEvent(const WidgetEvent::MouseEvent& mouseEvent);

        bool HandleEdgeMovement(const WidgetEvent::MouseEvent& mouseEvent);
        bool HandleLeafMovement(const WidgetEvent::MouseEvent& mouseEvent);
        bool HandleMouseHover(const WidgetEvent::MouseEvent& mouseEvent);

        template<Direction VEdgeDirection>
        bool HandleDirectionalEdgeMovement(Edge& edge, float movement);
     
        template<Direction VEdgeDirection>
        static constexpr float LockEdgeMovement(const Edge& edge, const float movement);

        Edge* FindIntersectingEdge(const Vector2f32& point);
        Element* FindIntersectingDraggableLeaf(const Vector2f32& point);
        /**@}*/

        /** Bounds calculation functions. */
        /**@{*/
        void CalculateBounds();

        void CalculateElementBounds(Element& element, const Bounds2f32& grantedBounds);
        void CalculateElementBounds(Leaf& leaf, const Bounds2f32& grantedBounds);
        void CalculateElementBounds(Grid& grid, const Bounds2f32& grantedBounds);

        template<Direction VGridDirection>
        void CalculateGridBounds(Grid& grid, const Bounds2f32& grantedBound);

        template<Direction VGridDirection>
        void CalculateGridElementsBounds(CalculatedGridResult& calculatedGridResult);

        template<Direction VGridDirection>
        void BalanceGridDynamicElements(const CalculatedGridResult& calculatedGridResult);

        template<Direction VGridDirection>
        void BalanceGridStaticElements(const CalculatedGridResult& calculatedGridResult);

        template<Direction VGridDirection>
        void SetGridElementBounds(const CalculatedGridResult& calculatedGridResult);

        template<Direction VEdgeDirection>
        void SetElementPrevEdgeBounds(Element& element, const Bounds2f32& grantedBounds);

        template<typename TIterator>
        void HideElements(TIterator begin, TIterator end);
        /**@}*/

        /** Directional and other helper functions. */
        /**@{*/
        static constexpr Direction FlipDirection(const Direction direction);

        static constexpr Vector2f32 GetDirectionalMinSize(const Vector2f32& size);

        template<Direction VDirection>
        static constexpr float GetDirectionalMinWidth(const Vector2f32& size);

        template<Direction VDirection>
        static constexpr float GetDirectionalWidth(const Vector2f32& size);

        template<Direction VDirection>
        static constexpr float GetDirectionalHeight(const Vector2f32& size);


        template<Direction VDirection>
        static constexpr void SetDirectionalWidth(Vector2f32& size, const float width);

        template<Direction VDirection>
        static constexpr void SetDirectionalHeight(Vector2f32& size, const float height);


        template<Direction VDirection>
        static constexpr void AddDirectionalWidth(Vector2f32& size, const float width);

        template<Direction VDirection>
        static constexpr void AddDirectionalHeight(Vector2f32& size, const float height);


        static constexpr void CutWidth(float& widthLeft, const float width);
        static constexpr void CutAndClampWidth(float& widthLeft, float& width);      

        template<Direction VDirection>
        static constexpr Bounds2f32 CutDirectionalBounds(Bounds2f32& grantedBounds, const float size);
        /**@}*/
        

        static constexpr float m_minElementWidth = 10.0f;
        
        typename State::Type m_stateType;
        Mouse::Cursor m_currentCursor;
        ElementPointer m_rootElement;
        std::set<EdgePointer> m_edges;
        std::set<Element*> m_leafElements;
        std::queue<PendingLeafInsertPointer> m_leafInsertQueue;
        std::map<Widget<TSkin>*, PendingLeafInsertPointer> m_leafInsertMap;
        Edge* m_pressedEdge;
        Element* m_pressedLeafElement;
        Vector2f32 m_prevMousePosition;
        Bounds2f32 m_oldGrantedBounds;
        bool m_forceUpdateBounds;
        
        friend struct TSkin::template WidgetSkin<Docker<TSkin>>;

    };

}

#include "Molten/Gui/Widgets/DockerWidget.inl"

#endif