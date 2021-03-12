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
#include <set>
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
    class Docker : public WidgetMixin<TSkin, Docker>, public WidgetEventHandler
    {

    public:

        static constexpr bool overrideChildrenMouseEvents = true;
        static constexpr bool handleKeyboardEvents = false;
        static constexpr bool handleMouseEvents = true;

        float edgeWidth = WidgetSkinType::edgeWidth;
        float spacing = WidgetSkinType::spacing;
        float widgetDragActivationDistance = WidgetSkinType::widgetDragActivationDistance;
        Vector2f32 minElementSize = { 30.0f, 30.0f };

        Signal<Mouse::Cursor> onCursorChange;

        struct State
        {
            enum class Type
            {
                Normal,
                EdgeDrag,
                LeafDrag
            };
          
            struct LeafDragState
            {
                Bounds2f32 dockingBounds;
            };

            State();
            explicit State(const LeafDragState& leafDragState);

            Type type;

            union
            {
                LeafDragState leafDragState;
            };
        };

        explicit Docker(WidgetDataMixin<TSkin, Docker>& data);

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

        class Element;
        class Leaf;
        class Grid;
        struct Edge;
        struct PendingLeafInsert;

        using ElementPointer = std::unique_ptr<Element>;
        using ElementPointerList = std::list<ElementPointer>;
        using ElementPointerIterator = typename ElementPointerList::iterator;
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

        class Leaf
        {

        public:

            Leaf(
                WidgetData<TSkin>* widgetData, 
                const bool isDynamic);

            Leaf(const Leaf&) = delete;
            Leaf(Leaf&&) = delete;
            Leaf& operator =(const Leaf&) = delete;
            Leaf& operator =(Leaf&&) = delete;

            Element* AsElement();

            bool IsDynamic() const;

            WidgetData<TSkin>* widgetData;
            DraggableWidget* draggableWidget;
            bool isDynamic;

        private:

            friend class Element;
            Element* m_owner;
        };

        class Grid
        {

        public:

            explicit Grid(Direction direction);

            Grid(const Grid&) = delete;
            Grid(Grid&&) = delete;
            Grid& operator =(const Grid&) = delete;
            Grid& operator =(Grid&&) = delete;

            Element* AsElement();

            bool IsDynamic() const;
         
            Direction direction;
            ElementPointerList elements;
            std::set<Element*> dynamicElements;

        private:

            friend class Element;
            Element* m_owner;

        };

        class Element
        {

        public:

            using Type = std::variant<LeafPointer, GridPointer>;

            Element(
                LeafPointer&& leaf, 
                const Vector2f32& requestedSize,
                const Vector2f32& minSize);

            explicit Element(GridPointer&& grid);

            Element(const Element&) = delete;
            Element(Element&&) = delete;
            Element& operator =(const Element&) = delete;
            Element& operator =(Element&&) = delete;  

            ElementType GetType() const;    
            
            Leaf* GetLeaf();
            Grid* GetGrid();   

            Element* GetParent();

            Edge* GetPrevEdge();
            Edge* GetNextEdge();
            void SetPrevEdge(Edge* edge);
            void SetNextEdge(Edge* edge);

            template<typename TCallback>
            auto VisitData(TCallback && callback);

            bool IsDynamic() const;

            [[nodiscard]] EdgePointer InsertElement(
                ElementPointer&& element,
                const DockingPosition position);

            [[nodiscard]] std::pair<ElementPointer, Edge*> Extract();

            [[nodiscard]] std::pair<ElementPointer, Edge*> ExtractElement(Element* element);

            void UpdateConstraintsFromChildren();
            void UpdateConstraintsFromChildren(const Direction direction);
            template<Direction VDirection>
            void UpdateConstraintsFromChildren();

            void UpdateParentsConstraints();

            Vector2f32 minSize;
            Vector2f32 requestedSize;
            Vector2f32 renderSize;
            Bounds2f32 bounds;   // Move to leaf?

        private:

            void TransformLeafToGrid(Direction direction);

            void TransformGridToFlipperGrid();

            void TransformToChild();

            void TransformToChildLeaf();

            void TransformToChildGrid();

            void TransformToParentGrid();

            void SetChildrensParentToThis();

            EdgePointer InsertElementInGrid(
                ElementPointer&& element,
                const ElementPosition position);

            EdgePointer InsertElementInGrid(
                ElementPointer&& element,
                const typename ElementPointerList::iterator it);

            EdgePointer InsertElementInParentGrid(
                ElementPointer&& element,
                Element* neighborElement,
                const ElementPosition position);

            void AddDynamicElementFromParents(Element* element);
            void RemoveDynamicElementFromParents(Element* element);

            [[nodiscard]] Edge* ExtractEdge(Element* element);

            ElementType m_type;
            Type m_data;
            Element* m_parent;
            Edge* m_prevEdge;
            Edge* m_nextEdge;

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
            Element* m_prevElement;
            Element* m_nextElement;
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

        /** Child insert, move or deletion functions. */
        /**@{*/
        bool InsertNewLeafs();
        void InsertLeaf(PendingLeafInsert& pendingLeaf);

        ElementPointer ExtractElement(Element* element);

        static constexpr Direction GetInsertDirection(DockingPosition position);
        static constexpr ElementPosition GetInsertPosition(DockingPosition position);
        /**@}*/

        /** Input handling functions. */
        /**@{*/
        void SetCursor(const Mouse::Cursor cursor);

        void ActivateNormalUpdate();
        void ActivateEdgeDragUpdate(Edge* pressedEdge, const Vector2f32& mousePosition);
        void ActivateLeafDragUpdate(Leaf* pressedLeaf, const Vector2f32& mousePosition);

        bool HandleNormalMouseEvent(const WidgetEvent& widgetEvent);
        bool HandleEdgeDragMouseEvent(const WidgetEvent& widgetEvent);
        bool HandleLeafDragMouseEvent(const WidgetEvent& widgetEvent);

        bool HandleNormalMousePressEvent(const WidgetEvent::MouseEvent& mouseEvent);
        bool HandleNormalMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent);

        bool HandleEdgeDragMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent);
        bool HandleEdgeDragMouseReleaseEvent(const WidgetEvent::MouseEvent& mouseEvent);

        bool HandleLeafDragMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent);
        bool HandleLeafDragMouseReleaseEvent(const WidgetEvent::MouseEvent& mouseEvent);

        template<Direction VEdgeDirection>
        bool HandleDirectionalEdgeMovement(Edge& edge, float movement);

        Edge* FindIntersectingEdge(const Vector2f32& point);
        Leaf* FindIntersectingDraggableLeaf(const Vector2f32& point);
        Leaf* FindIntersectingLeaf(const Vector2f32& point);

        /**@}*/

        /** Bounds calculation functions. */
        /**@{*/
        void CalculateBounds();
        void CalculateElementBounds(Element& element, const Bounds2f32& grantedBounds);
        static void CalculateElementBounds(Element& element, Leaf& leaf, const Bounds2f32& grantedBounds);
        void CalculateElementBounds(Element& element, Grid& grid, const Bounds2f32& grantedBounds);
        template<Direction VGridDirection>
        void CalculateElementBounds(Element& element, Grid& grid, const Bounds2f32& grantedBound);

        template<typename TIterator>
        void HideElements(TIterator begin, TIterator end);

        template<Direction VGridDirection>
        static constexpr void DirectionalShrinkBounds(Bounds2f32& bounds, float shrinkValue);
        template<Direction VGridDirection>
        static constexpr void DirectionalShrinkBounds(Bounds2f32& bounds, Bounds2f32& shrinkedBounds, float shrinkValue);

        template<Direction VEdgeDirection>
        void SetElementPrevEdgeBounds(Element& element, const Bounds2f32& grantedBounds);
        /**@}*/

        /** Directional and other helper functions. */
        /**@{*/
        static constexpr Direction FlipDirection(const Direction direction);

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

        static bool GetDockingPositionInElement(const Vector2f32& mousePosition, Leaf& leaf, Bounds2f32& dockingBounds, DockingPosition& position);
        /**@}*/    

        struct EdgeDragData
        {
            EdgeDragData();
            void Reset();

            Edge* pressedEdge;
            Vector2f32 prevMousePosition;
        };

        struct LeafDragData
        {
            LeafDragData();
            void Reset();

            Leaf* pressedLeaf;        
            Vector2f32 initialMousePosition;
            Leaf* dockingLeaf;
            DockingPosition dockingPosition;
            bool dragIsActivated;
        };


        typename State::Type m_stateType;
        Mouse::Cursor m_currentCursor;
        ElementPointer m_rootElement;
        
        // Updates.
        bool(Docker<TSkin>::* m_mouseInputUpdateFunc)(const WidgetEvent&);
        bool m_forceUpdateBounds;
        Bounds2f32 m_oldGrantedBounds;      
        
        // Leaf insert.
        std::queue<PendingLeafInsertPointer> m_leafInsertQueue;
        std::map<Widget<TSkin>*, PendingLeafInsertPointer> m_leafInsertMap;
        
        // Edges.
        std::set<EdgePointer> m_edges;
        EdgeDragData m_edgeDragData;

        // Leafs.
        std::set<Leaf*> m_leafs;
        LeafDragData m_leafDragData;

    };

}

#include "Molten/Gui/Widgets/DockerWidget.inl"

#endif