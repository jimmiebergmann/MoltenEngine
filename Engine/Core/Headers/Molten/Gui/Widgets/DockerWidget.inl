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

    // Docket state implementations.
    template<typename TSkin>
    Docker<TSkin>::State::State() :
        type(Type::Normal),
        leafDragState{ Bounds2f32{} }
    {}

    template<typename TSkin>
    Docker<TSkin>::State::State(const LeafDragState& leafDragState) :
        type(Type::LeafDrag),
        leafDragState(leafDragState)
    {}


    // Docker public implementations.
    template<typename TSkin>
    Docker<TSkin>::Docker(WidgetDataMixin<TSkin, Docker>& data) :
        WidgetMixin<TSkin, Docker>(data),  
        m_stateType{},
        m_currentCursor(Mouse::Cursor::Normal),
        m_rootElement(nullptr),
        m_mouseInputUpdateFunc(&Docker<TSkin>::HandleNormalMouseEvent),
        m_forceUpdateBounds(false),
        m_oldGrantedBounds{},
        m_leafInsertQueue{},
        m_leafInsertMap{},
        m_edges{},
        m_edgeDragData{},
        m_leafs{},
        m_leafDragData{}
    { }

    template<typename TSkin>
    template<template<typename> typename TWidgetType, typename ... TArgs>
    WidgetTypePointer<TWidgetType<TSkin>> Docker<TSkin>::CreateChild(
        const DockingPosition position,
        const bool dynamic,
        TArgs ... args)
    {
        auto widget = Widget<TSkin>::template CreateChild<TWidgetType>(std::forward<TArgs>(args)...);

        auto it = m_leafInsertMap.find(widget.get());
        if (it == m_leafInsertMap.end())
        {
            return widget;
        }

        auto& leafInsert = it->second;
        leafInsert->position = position;
        leafInsert->isDynamic = dynamic;

        return widget;
    }

    template<typename TSkin>
    void Docker<TSkin>::Update()
    {
        if (InsertNewLeafs())
        {
            m_forceUpdateBounds = true;
        }

        ApplyMarginsToGrantedBounds();

        if (m_forceUpdateBounds || m_oldGrantedBounds != GetGrantedBounds())
        {
            m_forceUpdateBounds = false;
            CalculateBounds(); 
        }

        m_oldGrantedBounds = GetGrantedBounds();

        if (GetSkinState<State>().type == State::Type::LeafDrag && m_leafDragData.dragIsActivated) // Ugly stuff going on here.
        {
            GetDataMixin().canvas->PushTopDrawCommand([this]()
            {
                GetDataMixin().widgetSkinMixin->DrawLeafDocking();
            });
        }
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEvent(const WidgetEvent& widgetEvent)
    {
        switch (widgetEvent.type)
        {
            case WidgetEventType::Mouse: return (*this.*m_mouseInputUpdateFunc)(widgetEvent); break;
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    void Docker<TSkin>::OnAddChild(WidgetData<TSkin>& childData)
    {
        auto* widget = childData.GetWidget();

        bool isDynamic = widget->size.x <= 0.0f || widget->size.y <= 0.0f;
        auto pendingLeaf = std::make_shared<PendingLeafInsert>(DockingPosition::Right, isDynamic, &childData);

        m_leafInsertMap.insert({ widget, pendingLeaf });
        m_leafInsertQueue.push(pendingLeaf);
    }


    // Docker leaf implementations.
    template<typename TSkin>
    Docker<TSkin>::Leaf::Leaf(
        WidgetData<TSkin>* widgetData,
        const bool isDynamic
    ) :       
        widgetData(widgetData),
        draggableWidget(dynamic_cast<DraggableWidget*>(widgetData->GetWidget())),
        isDynamic(isDynamic),
        m_owner(nullptr)
    {}

    template<typename TSkin>
    typename Docker<TSkin>::Element* Docker<TSkin>::Leaf::AsElement()
    {
        return m_owner;
    }

    template<typename TSkin>
    bool Docker<TSkin>::Leaf::IsDynamic() const
    {
        return isDynamic;
    }


    // Docker grid implementations.
    template<typename TSkin>
    Docker<TSkin>::Grid::Grid(Direction direction) :
        direction(direction),
        m_owner(nullptr)
    {}

    template<typename TSkin>
    typename Docker<TSkin>::Element* Docker<TSkin>::Grid::AsElement()
    {
        return m_owner;
    }

    template<typename TSkin>
    bool Docker<TSkin>::Grid::IsDynamic() const
    {
        return !dynamicElements.empty();
    }


    // Docker element implementations.
    template<typename TSkin>
    Docker<TSkin>::Element::Element(
        LeafPointer&& leaf,
        const Vector2f32& requestedSize,
        const Vector2f32& minSize
    ) :
        m_type(ElementType::Leaf),
        m_data(std::move(leaf)),
        m_parent(nullptr),
        m_prevEdge(nullptr),
        m_nextEdge(nullptr),
        minSize(minSize),
        requestedSize(std::max(minSize.x, requestedSize.x), std::max(minSize.y, requestedSize.y))
    {
        std::get<LeafPointer>(m_data)->m_owner = this;
    }

    template<typename TSkin>
    Docker<TSkin>::Element::Element(GridPointer&& grid) :
        m_type(ElementType::Grid),
        m_data(std::move(grid)),
        m_parent(nullptr),
        m_prevEdge(nullptr),
        m_nextEdge(nullptr)
    {
        auto& gridData = std::get<GridPointer>(m_data);
        gridData->m_owner = this;

        for (auto& element : gridData->elements)
        {
            element->m_parent = this;
        }
    }

    template<typename TSkin>
    typename Docker<TSkin>::ElementType Docker<TSkin>::Element::GetType() const
    {
        return m_type;
    }

    template<typename TSkin>
    typename Docker<TSkin>::Leaf* Docker<TSkin>::Element::GetLeaf()
    {
        return std::get<LeafPointer>(m_data).get();
    }

    template<typename TSkin>
    typename Docker<TSkin>::Grid* Docker<TSkin>::Element::GetGrid()
    {
        return std::get<GridPointer>(m_data).get();
    }

    template<typename TSkin>
    typename Docker<TSkin>::Element* Docker<TSkin>::Element::GetParent()
    {
        return m_parent;
    }

    template<typename TSkin>
    typename Docker<TSkin>::Edge* Docker<TSkin>::Element::GetPrevEdge()
    {
        return m_prevEdge;
    }

    template<typename TSkin>
    typename Docker<TSkin>::Edge* Docker<TSkin>::Element::GetNextEdge()
    {
        return m_nextEdge;
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::SetPrevEdge(Edge* edge)
    {
        m_prevEdge = edge;
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::SetNextEdge(Edge* edge)
    {
        m_nextEdge = edge;
    }

    template<typename TSkin>
    template<typename TCallback>
    auto Docker<TSkin>::Element::VisitData(TCallback&& callback)
    {
        return std::visit(callback, m_data);
    }

    template<typename TSkin>
    bool Docker<TSkin>::Element::IsDynamic() const
    {
        return std::visit([&](auto& data)
        {
            return data->IsDynamic();
        }, m_data);
    }

    template<typename TSkin>
    typename Docker<TSkin>::EdgePointer Docker<TSkin>::Element::InsertElement(
        ElementPointer&& element,
        const DockingPosition position)
    {   
        const auto gridDirection = GetInsertDirection(position);

        if (m_type == ElementType::Grid)
        {
            auto* grid = GetGrid();
            if (gridDirection == grid->direction) // Use this grid, append to begining/end.
            {
                return InsertElementInGrid(std::move(element), GetInsertPosition(position));
            }

            TransformGridToFlipperGrid();
            return InsertElementInGrid(std::move(element), GetInsertPosition(position));
        }

        // Leaf
        if (m_parent && m_parent->m_type == ElementType::Grid && m_parent->GetGrid()->direction == gridDirection) // Append to parents grid.
        {
            return InsertElementInParentGrid(std::move(element), this, GetInsertPosition(position));
        }

        // Transform this leaf to a grid and append.
        TransformLeafToGrid(GetInsertDirection(position));
        return InsertElementInGrid(std::move(element), GetInsertPosition(position));
    }

    template<typename TSkin>
    std::pair<typename Docker<TSkin>::ElementPointer, typename Docker<TSkin>::Edge*> Docker<TSkin>::Element::Extract()
    {
        if (!m_parent)
        {
            return { nullptr, nullptr };
        }

        return m_parent->ExtractElement(this);
    }

    template<typename TSkin>
    std::pair<typename Docker<TSkin>::ElementPointer, typename Docker<TSkin>::Edge*> Docker<TSkin>::Element::ExtractElement(Element* element)
    {
        if (!element || m_type != ElementType::Grid)
        {
            return { nullptr, nullptr };
        }

        auto& grid = std::get<GridPointer>(m_data);

        // Find extract and erase from grid.
        auto elementIt = std::find_if(grid->elements.begin(), grid->elements.end(), [&](auto& child)
        {
            return child.get() == element;
        });

        if (elementIt == grid->elements.end())
        {
            return { nullptr, nullptr };
        }
      
        if (element->IsDynamic())
        {
            grid->dynamicElements.erase(element);
            RemoveDynamicElementFromParents(this);
        }
        auto extractedElement = std::move(*elementIt);
        grid->elements.erase(elementIt); 

        auto* extractedEdge = ExtractEdge(extractedElement.get());

        extractedElement->m_parent = nullptr;
        extractedElement->m_prevEdge = nullptr;
        extractedElement->m_nextEdge = nullptr;

        if (grid->elements.size() >= 2)
        {
            UpdateConstraintsFromChildren(grid->direction);
        }
        else
        {
            TransformToChild();
        }

        return { std::move(extractedElement), extractedEdge };
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::UpdateConstraintsFromChildren()
    {
        if (m_type != ElementType::Grid)
        {
            return;
        }

        auto& grid = std::get<GridPointer>(m_data);

        minSize = { 0.0f, 0.0f };
        requestedSize = { 0.0f, 0.0f };

        if (grid->direction == Direction::Horizontal)
        {
            for (auto& element : grid->elements)
            {
                minSize.x += element->minSize.x;
                requestedSize.x += element->requestedSize.x;

                if (element->minSize.y > minSize.y)
                {
                    minSize.y = element->minSize.y;
                }
                if (element->requestedSize.y > requestedSize.y)
                {
                    requestedSize.y = element->requestedSize.y;
                }
            }
        }
        else
        {
            for (auto& element : grid->elements)
            {
                minSize.y += element->minSize.y;
                requestedSize.y += element->requestedSize.y;

                if (element->minSize.x > minSize.x)
                {
                    minSize.x = element->minSize.x;
                }
                if (element->requestedSize.x > requestedSize.x)
                {
                    requestedSize.x = element->requestedSize.x;
                }
            }
        }
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::UpdateConstraintsFromChildren(const Direction direction)
    {
        if (m_type != ElementType::Grid)
        {
            return;
        }

        if (direction == Direction::Horizontal)
        {
            UpdateConstraintsFromChildren<Direction::Horizontal>();
        }
        else
        {
            UpdateConstraintsFromChildren<Direction::Vertical>();
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    void Docker<TSkin>::Element::UpdateConstraintsFromChildren()
    {
        auto oldHeight = GetDirectionalHeight<VDirection>(requestedSize);
        UpdateConstraintsFromChildren();
        SetDirectionalHeight<VDirection>(requestedSize, oldHeight);
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::UpdateParentsConstraints()
    {
        auto* parent = m_parent;
        while (parent)
        {
            parent->UpdateConstraintsFromChildren();
            parent = parent->m_parent;
        }
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::TransformLeafToGrid(Direction direction)
    {
        auto& leaf = std::get<LeafPointer>(m_data);
        auto leafData = std::move(leaf);
        auto oldChildElement = std::make_unique<Element>(std::move(leafData), requestedSize, minSize);

        auto newGrid = std::make_unique<Grid>(direction);
        newGrid->m_owner = this;

        m_type = ElementType::Grid;
        m_data = std::move(newGrid);
        minSize = { 0.0f, 0.0f };
        requestedSize = { 0.0f, 0.0f };

        InsertElementInGrid(std::move(oldChildElement), ElementPosition::First);
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::TransformGridToFlipperGrid()
    {
        auto& grid = std::get<GridPointer>(m_data);
        auto gridDirection = grid->direction;

        auto gridData = std::move(grid);
        auto oldChildElement = std::make_unique<Element>(std::move(gridData));
        oldChildElement->minSize = minSize;
        oldChildElement->requestedSize = requestedSize;

        auto newGrid = std::make_unique<Grid>(FlipDirection(gridDirection));
        newGrid->m_owner = this;

        m_data = std::move(newGrid);
        minSize = { 0.0f, 0.0f };
        requestedSize = { 0.0f, 0.0f };

        InsertElementInGrid(std::move(oldChildElement), ElementPosition::First);
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::TransformToChild()
    {
        auto& grid = std::get<GridPointer>(m_data);

        if (grid->elements.size() != 1)
        {
            return;
        }

        if (grid->elements.front()->m_type == ElementType::Leaf)
        {
            TransformToChildLeaf();
        }
        else
        {
            TransformToChildGrid();
        }

    }

    template<typename TSkin>
    void Docker<TSkin>::Element::TransformToChildLeaf()
    {
        auto& grid = std::get<GridPointer>(m_data);

        auto* firstElement = grid->elements.front().get();

        minSize = firstElement->minSize;
        requestedSize = grid->direction == Direction::Horizontal ? Vector2f32{ firstElement->requestedSize.x, requestedSize.y } : Vector2f32{ requestedSize.x, firstElement->requestedSize.y };
        requestedSize = { std::max(minSize.x, requestedSize.x), std::max(minSize.y, requestedSize.y) };

        auto extractedData = std::move(std::get<LeafPointer>(firstElement->m_data));
        extractedData->m_owner = this;
        m_data = std::move(extractedData);
        m_type = ElementType::Leaf;
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::TransformToChildGrid()
    {
        auto& grid = std::get<GridPointer>(m_data);

        auto* firstElement = grid->elements.front().get();

        minSize = firstElement->minSize;
        requestedSize = grid->direction == Direction::Horizontal ? Vector2f32{ firstElement->requestedSize.x, requestedSize.y } : Vector2f32{ requestedSize.x, firstElement->requestedSize.y };
        requestedSize = { std::max(minSize.x, requestedSize.x), std::max(minSize.y, requestedSize.y) };

        auto extractedData = std::move(std::get<GridPointer>(firstElement->m_data));
        extractedData->m_owner = this;
        m_data = std::move(extractedData);

        SetChildrensParentToThis();

        if (m_parent && m_parent->m_type == ElementType::Grid)
        {
            auto parentGrid = m_parent->GetGrid();
            if (GetGrid()->direction == parentGrid->direction)
            {
                TransformToParentGrid();
            }
        }
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::TransformToParentGrid()
    {
        auto& parentGrid = std::get<GridPointer>(m_parent->m_data);

        auto it = std::find_if(parentGrid->elements.begin(), parentGrid->elements.end(), [&](auto& child)
        {
            return child.get() == this;
        });

        if(it == parentGrid->elements.end())
        {
            return;
        }

        auto* newPrevElement = it->get() != parentGrid->elements.front().get() ? std::prev(it)->get() : nullptr;
        auto* newNextElement = it->get() != parentGrid->elements.back().get() ? std::next(it)->get() : nullptr;

        auto extractedGrid = std::move(*it);
        it = parentGrid->elements.erase(it);

        auto& grid = std::get<GridPointer>(extractedGrid->m_data);

        auto* oldFirstElement = grid->elements.front().get();
        auto* oldLastElement = grid->elements.back().get();

        // Insert this elements to parent grid.
        for(auto eIt = grid->elements.rbegin(); eIt != grid->elements.rend(); eIt++)
        {
            (*eIt)->m_parent = m_parent;
            it = parentGrid->elements.insert(it, std::move(*eIt));
        }

        // Erase this elements from parent's dynamic elements.
        auto deIt = parentGrid->dynamicElements.find(this);
        if(deIt != parentGrid->dynamicElements.end())
        {
            parentGrid->dynamicElements.erase(deIt);
            m_parent->RemoveDynamicElementFromParents(m_parent);
        }

        // Add thid elements dynamic elements to this
        for(auto* dynamicElement : grid->dynamicElements)
        {
            parentGrid->dynamicElements.insert(dynamicElement);
            m_parent->AddDynamicElementFromParents(m_parent);
        }

        if (newPrevElement && newPrevElement->GetNextEdge())
        {
            newPrevElement->GetNextEdge()->m_nextElement = oldFirstElement;
            oldFirstElement->SetPrevEdge(newPrevElement->GetNextEdge());
        }
        if (newNextElement && newNextElement->GetPrevEdge())
        {
            newNextElement->GetPrevEdge()->m_prevElement = oldLastElement;
            oldLastElement->SetNextEdge(newNextElement->GetPrevEdge());
        }

        m_prevEdge = nullptr;
        m_nextEdge = nullptr;
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::SetChildrensParentToThis()
    {
        auto& grid = std::get<GridPointer>(m_data);
        for(auto& element : grid->elements)
        {
            element->m_parent = this;
        }
    }

    template<typename TSkin>
    typename Docker<TSkin>::EdgePointer Docker<TSkin>::Element::InsertElementInGrid(
        ElementPointer&& element,
        const ElementPosition position)
    {
        auto& grid = std::get<GridPointer>(m_data);
        auto it = position == ElementPosition::First ? grid->elements.begin() : grid->elements.end();
        return InsertElementInGrid(std::move(element), it);
    }

    template<typename TSkin>
    typename Docker<TSkin>::EdgePointer Docker<TSkin>::Element::InsertElementInGrid(
        ElementPointer&& element,
        const typename ElementPointerList::iterator it)
    {
        auto& grid = std::get<GridPointer>(m_data);

        EdgePointer newEdge = nullptr;
        if (it == grid->elements.begin()) // Insert a front.
        {
            auto* nextElement = grid->elements.empty() ? nullptr : it->get();
            if (nextElement)
            {
                newEdge = std::make_unique<Edge>(FlipDirection(grid->direction), element.get(), nextElement);

                element->SetNextEdge(newEdge.get());
                nextElement->SetPrevEdge(newEdge.get());
            }
           
        }
        else if (it == grid->elements.end()) // Insert at back.
        {
            auto* prevElement = grid->elements.empty() ? nullptr : std::prev(it)->get();
            if (prevElement)
            {
                newEdge = std::make_unique<Edge>(FlipDirection(grid->direction), prevElement, element.get());

                prevElement->SetNextEdge(newEdge.get());
                element->SetPrevEdge(newEdge.get());               
            }
        }       
        else // Insert in middle, somewhere.
        {
            auto* prevElement = std::prev(it)->get();
            auto* nextEdge = it->get()->m_prevEdge;

            newEdge = std::make_unique<Edge>(FlipDirection(grid->direction), prevElement, element.get());

            prevElement->SetNextEdge(newEdge.get());
            element->SetPrevEdge(newEdge.get());
            element->SetNextEdge(nextEdge);
            nextEdge->m_prevElement = element.get();
        }

        auto* newChildElementPtr = element.get();
        grid->elements.insert(it, std::move(element));
        if (newChildElementPtr->IsDynamic())
        {
            grid->dynamicElements.insert(newChildElementPtr);
            AddDynamicElementFromParents(this);
        }

        newChildElementPtr->m_parent = this;

        UpdateConstraintsFromChildren();
        UpdateParentsConstraints();

        return newEdge;
    }

    template<typename TSkin>
    typename Docker<TSkin>::EdgePointer Docker<TSkin>::Element::InsertElementInParentGrid(
        ElementPointer&& element,
        Element* neighborElement,
        const ElementPosition position)
    {
        auto& parentGrid = std::get<GridPointer>(m_parent->m_data);

        auto it = std::find_if(parentGrid->elements.begin(), parentGrid->elements.end(), [&](auto& child)
        {
            return child.get() == neighborElement;
        });

        if(position == ElementPosition::Last)
        {
            ++it;
        }

        return m_parent->InsertElementInGrid(std::move(element), it);
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::AddDynamicElementFromParents(Element* element)
    {
        auto* parent = m_parent;

        while (parent && element->IsDynamic())
        {
            auto* parentGrid = parent->GetGrid();

            auto it = parentGrid->dynamicElements.find(element);
            if (it != parentGrid->dynamicElements.end())
            {
                return;
            }

            parentGrid->dynamicElements.insert(element);

            element = parent;
            parent = parent->m_parent;
        }
    }

    template<typename TSkin>
    void Docker<TSkin>::Element::RemoveDynamicElementFromParents(Element* element)
    {
        auto* parent = m_parent;

        while (parent && !element->IsDynamic())
        {
            auto* parentGrid = parent->GetGrid();

            auto it = parentGrid->dynamicElements.find(element);
            if (it == parentGrid->dynamicElements.end())
            {
                return;
            }

            parentGrid->dynamicElements.erase(it);

            element = parent;
            parent = parent->m_parent; 
        }
    }

    template<typename TSkin>
    typename Docker<TSkin>::Edge* Docker<TSkin>::Element::ExtractEdge(Element* element)
    {
        Edge* extactedEdge = nullptr;
        if ((extactedEdge = element->m_prevEdge) != nullptr)
        {
            auto* nextEdge = element->m_nextEdge;
            auto* prevElement = extactedEdge->m_prevElement;

            if (prevElement)
            {
                prevElement->m_nextEdge = nextEdge;
            }
            if (nextEdge)
            {
                nextEdge->m_prevElement = prevElement;
            }
        }
        else if ((extactedEdge = element->m_nextEdge) != nullptr)
        {
            auto* prevEdge = element->m_prevEdge;
            auto* nextElement = extactedEdge->m_nextElement;

            if (nextElement)
            {
                nextElement->m_prevEdge = prevEdge;
            }
            if (prevEdge)
            {
                prevEdge->m_nextElement = nextElement;
            }
        }

        if(extactedEdge)
        {
            extactedEdge->m_prevElement = nullptr;
            extactedEdge->m_nextElement = nullptr;
        }

        return extactedEdge;
    }


    // Docker edge implementations.
    template<typename TSkin>
    Docker<TSkin>::Edge::Edge(
        Direction direction,
        Element* prevElement,
        Element* nextElement
    ) :
        bounds(
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min()
        ),
        direction(direction),
        m_prevElement(prevElement),
        m_nextElement(nextElement)
    {}

    template<typename TSkin>
    Mouse::Cursor Docker<TSkin>::Edge::GetSizeCursor() const
    {
        return direction == Direction::Horizontal ? Mouse::Cursor::SizeUpDown : Mouse::Cursor::SizeLeftRight;
    }


    // Docker widget queued item implementations.
    template<typename TSkin>
    Docker<TSkin>::PendingLeafInsert::PendingLeafInsert(
        const DockingPosition position,
        const bool isDynamic,
        WidgetData<TSkin>* widgetData
    ) :
        position(position),
        isDynamic(isDynamic),
        widgetData(widgetData)
    {}


    // Private docket implementations.
    template<typename TSkin>
    bool Docker<TSkin>::InsertNewLeafs()
    {
        m_leafInsertMap.clear();
        
        if (m_leafInsertQueue.empty())
        {
            return false;
        }

        while (!m_leafInsertQueue.empty())
        {
            auto& pendingLeaf = m_leafInsertQueue.front();
            InsertLeaf(*pendingLeaf);
            m_leafInsertQueue.pop();
        }

        return true;
    }

    template<typename TSkin>
    void Docker<TSkin>::InsertLeaf(PendingLeafInsert& pendingLeaf)
    {
        auto newLeaf = std::make_unique<Leaf>(pendingLeaf.widgetData, pendingLeaf.isDynamic);
        m_leafs.insert(newLeaf.get());

        const auto widgetSize = pendingLeaf.widgetData->GetWidget()->size;
        auto newElement = std::make_unique<Element>(std::move(newLeaf), widgetSize, minElementSize);

        if (m_rootElement)
        {
            auto newEdge = m_rootElement->InsertElement(std::move(newElement), pendingLeaf.position);
            if (newEdge)
            {
                m_edges.insert(std::move(newEdge));
            }
        }
        else
        {
            m_rootElement = std::move(newElement);
        } 
    }

    template<typename TSkin>
    typename Docker<TSkin>::ElementPointer Docker<TSkin>::ExtractElement(Element* element)
    {
        auto [extractedElement, extractedEdge] = element->Extract();

        if (extractedEdge)
        {
            auto edgeIt = std::find_if(m_edges.begin(), m_edges.end(), [&](auto& edge)
            {
                return edge.get() == extractedEdge;
            });
            if (edgeIt != m_edges.end())
            {
                m_edges.erase(edgeIt);
            }
        }

        return std::move(extractedElement);
    }

    template<typename TSkin>
    constexpr typename Docker<TSkin>::Direction Docker<TSkin>::GetInsertDirection(DockingPosition position)
    {
        return position == DockingPosition::Left || position == DockingPosition::Right ? Direction::Horizontal : Direction::Vertical;
    }

    template<typename TSkin>
    constexpr typename Docker<TSkin>::ElementPosition Docker<TSkin>::GetInsertPosition(DockingPosition position)
    {
        return position == DockingPosition::Left || position == DockingPosition::Top ? ElementPosition::First : ElementPosition::Last;
    }

    template<typename TSkin>
    void Docker<TSkin>::SetCursor(const Mouse::Cursor cursor)
    {
        if (m_currentCursor != cursor)
        {
            onCursorChange(cursor);
            m_currentCursor = cursor;
        }
    }

    template<typename TSkin>
    void Docker<TSkin>::ActivateNormalUpdate()
    {
        m_edgeDragData.Reset();
        m_leafDragData.Reset();

        m_mouseInputUpdateFunc = &Docker<TSkin>::HandleNormalMouseEvent;
    }

    template<typename TSkin>
    void Docker<TSkin>::ActivateEdgeDragUpdate(Edge* pressedEdge, const Vector2f32& mousePosition)
    {
        m_edgeDragData.Reset();
        m_leafDragData.Reset();

        m_edgeDragData.pressedEdge = pressedEdge;
        m_edgeDragData.prevMousePosition = mousePosition;

        m_mouseInputUpdateFunc = &Docker<TSkin>::HandleEdgeDragMouseEvent;
        GetData().canvas->OverrideMouseEventsUntilMouseRelease(*this);
        SetCursor(pressedEdge->GetSizeCursor());
    }

    template<typename TSkin>
    void Docker<TSkin>::ActivateLeafDragUpdate(Leaf* pressedLeaf, const Vector2f32& mousePosition)
    {
        m_edgeDragData.Reset();
        m_leafDragData.Reset();

        m_leafDragData.pressedLeaf = pressedLeaf;
        m_leafDragData.initialMousePosition = mousePosition;

        m_mouseInputUpdateFunc = &Docker<TSkin>::HandleLeafDragMouseEvent;
        GetData().canvas->OverrideMouseEventsUntilMouseRelease(*this);
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleNormalMouseEvent(const WidgetEvent& widgetEvent)
    {
        switch (widgetEvent.subType)
        {
            case WidgetEventSubType::MouseButtonPressed: return HandleNormalMousePressEvent(widgetEvent.mouseEvent);
            case WidgetEventSubType::MouseMove: return HandleNormalMouseMoveEvent(widgetEvent.mouseEvent);
            case WidgetEventSubType::MouseLeave: SetCursor(Mouse::Cursor::Normal); break;
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEdgeDragMouseEvent(const WidgetEvent& widgetEvent)
    {
        switch(widgetEvent.subType)
        {
            case WidgetEventSubType::MouseMove: return HandleEdgeDragMouseMoveEvent(widgetEvent.mouseEvent);
            case WidgetEventSubType::MouseButtonReleasedIn:
            case WidgetEventSubType::MouseButtonReleasedOut: return HandleEdgeDragMouseReleaseEvent(widgetEvent.mouseEvent);
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleLeafDragMouseEvent(const WidgetEvent& widgetEvent)
    {
        switch(widgetEvent.subType)
        {
            case WidgetEventSubType::MouseMove: return HandleLeafDragMouseMoveEvent(widgetEvent.mouseEvent);
            case WidgetEventSubType::MouseButtonReleasedIn:
            case WidgetEventSubType::MouseButtonReleasedOut: return HandleLeafDragMouseReleaseEvent(widgetEvent.mouseEvent);
            default: break;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleNormalMousePressEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        auto* pressedEdge = FindIntersectingEdge(mouseEvent.position);
        if (pressedEdge)
        {         
            ActivateEdgeDragUpdate(pressedEdge, mouseEvent.position);
            return true;
        }

        auto* pressedLeaf = FindIntersectingDraggableLeaf(mouseEvent.position);
        if (pressedLeaf)
        {
            ActivateLeafDragUpdate(pressedLeaf, mouseEvent.position);
            return true;
        }

        return false;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleNormalMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        auto* hoveredEdge = FindIntersectingEdge(mouseEvent.position);
        if (hoveredEdge)
        {
            SetCursor(hoveredEdge->GetSizeCursor());
        }
        else
        {
            SetCursor(Mouse::Cursor::Normal);
        }
        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEdgeDragMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (!m_edgeDragData.pressedEdge)
        {
            return false;
        }

        SetCursor(m_edgeDragData.pressedEdge->GetSizeCursor());

        auto mouseMovement = Vector2f32(mouseEvent.position) - m_edgeDragData.prevMousePosition;
        bool movedEdge = false;

        if (m_edgeDragData.pressedEdge->direction == Direction::Horizontal)
        {
            movedEdge = HandleDirectionalEdgeMovement<Direction::Horizontal>(*m_edgeDragData.pressedEdge, mouseMovement.y);
        }
        else
        {
            movedEdge = HandleDirectionalEdgeMovement<Direction::Vertical>(*m_edgeDragData.pressedEdge, mouseMovement.x);
        }

        if (movedEdge)
        {
            m_forceUpdateBounds = true;
            m_edgeDragData.prevMousePosition = mouseEvent.position;
        }

        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleEdgeDragMouseReleaseEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        SetCursor(Mouse::Cursor::Normal);
        ActivateNormalUpdate();
        SetSkinState(State{});

        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleLeafDragMouseMoveEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (!m_leafDragData.dragIsActivated)
        {
            const float distance = (m_leafDragData.initialMousePosition - mouseEvent.position).Length();
            if (distance < widgetDragActivationDistance)
            {
                return false;
                
            }
            m_leafDragData.dragIsActivated = true;
        }

        SetCursor(Mouse::Cursor::SizeAll);

        auto* leaf = FindIntersectingLeaf(mouseEvent.position);
        if (!leaf || leaf == m_leafDragData.pressedLeaf)
        {
            SetSkinState(State{ });
            return false;
        }

        Bounds2f32 dockingBounds = {};
        if (GetDockingPositionInElement(mouseEvent.position, *leaf, dockingBounds, m_leafDragData.dockingPosition))
        {
            m_leafDragData.dockingLeaf = leaf;
            SetSkinState(State{ State::LeafDragState{ dockingBounds } });
        }
        else
        {
            m_leafDragData.dockingLeaf = nullptr;
            SetSkinState(State{ });
        }

        return true;
    }

    template<typename TSkin>
    bool Docker<TSkin>::HandleLeafDragMouseReleaseEvent(const WidgetEvent::MouseEvent& mouseEvent)
    {
        if (m_leafDragData.dockingLeaf && m_leafDragData.pressedLeaf && m_leafDragData.dockingLeaf != m_leafDragData.pressedLeaf)
        {
            auto* fromElementPtr = m_leafDragData.pressedLeaf->AsElement();
            auto* toElementPtr = m_leafDragData.dockingLeaf->AsElement();

            if (fromElementPtr && toElementPtr)
            {
                ElementPointer fromElement = ExtractElement(fromElementPtr);
                if (fromElement)
                {
                    m_leafDragData.pressedLeaf->widgetData->SetGrantedBounds({});

                    toElementPtr = m_leafDragData.dockingLeaf->AsElement(); // Get as element again. Very important to do after a call to ExtractElement.
                    auto newEdge = toElementPtr->InsertElement(std::move(fromElement), m_leafDragData.dockingPosition);
                    if (newEdge)
                    {
                        m_edges.insert(std::move(newEdge));
                    }

                    m_forceUpdateBounds = true;
                }
            }
        }

        SetCursor(Mouse::Cursor::Normal);
        ActivateNormalUpdate();
        SetSkinState(State{});

        return true;
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VEdgeDirection>
    bool Docker<TSkin>::HandleDirectionalEdgeMovement(Edge& edge, float movement)
    {   
        constexpr Direction flippedDirection = FlipDirection(VEdgeDirection);
        
        auto* prevElement = edge.m_prevElement;
        auto* nextElement = edge.m_nextElement;

        movement = std::max(
            movement,
            GetDirectionalWidth<flippedDirection>(prevElement->minSize) - GetDirectionalWidth<flippedDirection>(prevElement->renderSize));
        
        movement = std::min(
            movement,
            GetDirectionalWidth<flippedDirection>(nextElement->renderSize) - GetDirectionalWidth<flippedDirection>(nextElement->minSize));
              
        if (movement == 0.0f)
        {
            return false;
        }        

        if (prevElement)
        {
            AddDirectionalHeight<VEdgeDirection>(prevElement->requestedSize, movement);
        }
        if (nextElement)
        {
            AddDirectionalHeight<VEdgeDirection>(nextElement->requestedSize, -movement);
        }
         
        return true;
    }

    template<typename TSkin>
    typename Docker<TSkin>::Edge* Docker<TSkin>::FindIntersectingEdge(const Vector2f32& point)
    {
        for (auto& edge : m_edges)
        {
            if (edge->bounds.Intersects(point))
            {
                return edge.get();
            }
        }
        return nullptr;
    }

    template<typename TSkin>
    typename Docker<TSkin>::Leaf* Docker<TSkin>::FindIntersectingDraggableLeaf(const Vector2f32& point)
    {
        for (auto* leaf : m_leafs)
        {
            auto* draggableWidget = leaf->draggableWidget;
            if (draggableWidget && draggableWidget->GetDragBounds().Intersects(point))
            {
                return leaf;
            }
        }

        return nullptr;
    }

    template<typename TSkin>
    typename Docker<TSkin>::Leaf* Docker<TSkin>::FindIntersectingLeaf(const Vector2f32& point)
    {
        for (auto* leaf : m_leafs)
        {
            if (leaf->AsElement()->bounds.Intersects(point))
            {
                return leaf;
            }
        }
        return nullptr;
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateBounds()
    {
        if (!m_rootElement)
        {
            return;
        }

        CalculateElementBounds(*m_rootElement.get(), GetGrantedBounds());
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateElementBounds(Element& element, const Bounds2f32& grantedBounds)
    {
        element.VisitData([&](auto& data)
        {
            CalculateElementBounds(element , *data.get(), grantedBounds);
        });
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateElementBounds(Element& element, Leaf& leaf, const Bounds2f32& grantedBounds)
    {
        leaf.widgetData->SetGrantedBounds(grantedBounds);
    }

    template<typename TSkin>
    void Docker<TSkin>::CalculateElementBounds(Element& element, Grid& grid, const Bounds2f32& grantedBounds)
    {
        if (grid.direction == Direction::Horizontal)
        {
            return CalculateElementBounds<Direction::Horizontal>(element, grid, grantedBounds);
        }
        else
        {
            return CalculateElementBounds<Direction::Vertical>(element, grid, grantedBounds);
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    void Docker<TSkin>::CalculateElementBounds(Element& element, Grid& grid, const Bounds2f32& grantedBounds)
    {
        if (grid.elements.empty())
        {
            return;
        }

        const auto grantedSize = grantedBounds.GetSize();
        const auto spacingTotalWidth = grid.elements.size() > 1 ? static_cast<float>(grid.elements.size() - 1) * spacing : 0.0f;
        const auto grantedWidth = GetDirectionalWidth<VGridDirection>(grantedSize) - spacingTotalWidth;
        const auto requestedWidth = GetDirectionalWidth<VGridDirection>(element.requestedSize);
        auto widthDiff = grantedWidth - requestedWidth;

        // Distribute dynamic size.
        if (!grid.dynamicElements.empty())
        {
            auto partialWidthDiff = widthDiff / static_cast<float>(grid.dynamicElements.size());
            for (auto it = grid.dynamicElements.rbegin(); it != grid.dynamicElements.rend(); it++)
            {
                auto& childElement = *(*it);

                auto childPartialWidthDiff = std::max(
                    partialWidthDiff, 
                    GetDirectionalWidth<VGridDirection>(childElement.minSize) - GetDirectionalWidth<VGridDirection>(childElement.requestedSize));

                AddDirectionalWidth<VGridDirection>(childElement.requestedSize, childPartialWidthDiff);
                childElement.renderSize = childElement.requestedSize;

                widthDiff -= childPartialWidthDiff;
            }
        }

        // Distribute remaining diff
        for (auto it = grid.elements.begin(); it != grid.elements.end(); it++)
        {
            auto& childElement = *it->get();

            if (childElement.IsDynamic())
            {
                continue;
            }
                
            auto childWidthDiff = std::max(
                widthDiff, 
                GetDirectionalWidth<VGridDirection>(childElement.minSize) - GetDirectionalWidth<VGridDirection>(childElement.requestedSize));

            childElement.renderSize = childElement.requestedSize;
            AddDirectionalWidth<VGridDirection>(childElement.renderSize, childWidthDiff);

            widthDiff -= childWidthDiff;
        }

        // Set bounds.
        constexpr Direction flippedDirection = FlipDirection(VGridDirection);
        auto boundsLeft = grantedBounds;
        auto elementBounds = Bounds2f32{ };

        auto it = grid.elements.rbegin();
        for (; it != grid.elements.rend(); it++)
        {
            if (boundsLeft.IsEmpty())
            {
                break;
            }

            auto& childElement = *it->get();

            DirectionalShrinkBounds<VGridDirection>(boundsLeft, elementBounds, GetDirectionalWidth<VGridDirection>(childElement.renderSize));
            DirectionalShrinkBounds<VGridDirection>(boundsLeft, spacing);

            childElement.bounds = elementBounds;
            SetElementPrevEdgeBounds<flippedDirection>(childElement, grantedBounds);

            CalculateElementBounds(childElement, elementBounds); 
        }

        HideElements(it, grid.elements.rend());

        element.template UpdateConstraintsFromChildren<VGridDirection>();
    }

    template<typename TSkin>
    template<typename TIterator>
    void Docker<TSkin>::HideElements(TIterator begin, TIterator end)
    {
        // FIX, should implement partial tree node removal and use it instead of SetGrantedBounds.
        for (; begin != end; begin++)
        {
            auto& element = *begin;

            if (element->GetType() == ElementType::Leaf) // Leaf
            {
                auto* leaf = element->GetLeaf();
                leaf->widgetData->SetGrantedBounds({ 0.0f, 0.0f, 0.0f, 0.0f });
            }
            else // Grid
            {
                auto* grid = element->GetGrid();
                HideElements(grid->elements.rbegin(), grid->elements.rend());
            }
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    constexpr void Docker<TSkin>::DirectionalShrinkBounds(Bounds2f32& bounds, float shrinkValue)
    {
        if constexpr (VGridDirection == Direction::Horizontal)
        {
            float shrinkPosition = std::max(bounds.left, bounds.right - shrinkValue);
            bounds.right -= std::abs(shrinkPosition - bounds.right);
        }
        else
        {
            float shrinkPosition = std::max(bounds.top, bounds.bottom - shrinkValue);
            bounds.bottom -= std::abs(shrinkPosition - bounds.bottom);
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VGridDirection>
    constexpr void Docker<TSkin>::DirectionalShrinkBounds(Bounds2f32& bounds, Bounds2f32& shrinkedBounds, float shrinkValue)
    {
        if constexpr (VGridDirection == Direction::Horizontal)
        {
            float shrinkPosition = std::max(bounds.left, bounds.right - shrinkValue);
            shrinkValue = std::abs(shrinkPosition - bounds.right);

            shrinkedBounds = {
                bounds.right - shrinkValue, bounds.top, bounds.right, bounds.bottom
            };

            bounds.right -= shrinkValue;
        }
        else
        {
            float shrinkPosition = std::max(bounds.top, bounds.bottom - shrinkValue);
            shrinkValue = std::abs(shrinkPosition - bounds.bottom);

            shrinkedBounds = {
                bounds.left, bounds.bottom - shrinkValue, bounds.right, bounds.bottom
            };

            bounds.bottom -= shrinkValue;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VEdgeDirection>
    void Docker<TSkin>::SetElementPrevEdgeBounds(Element& element, const Bounds2f32& grantedBounds)
    {
        auto* prevEdge = element.GetPrevEdge();
        if (prevEdge)
        {
            float edgeWidthHalf = edgeWidth * 0.5f;
            float spacingHalf = spacing * 0.5f;

            if constexpr (VEdgeDirection == Direction::Horizontal)
            {
                prevEdge->bounds = Bounds2f32(grantedBounds.left, element.bounds.top, grantedBounds.right, element.bounds.top)
                    .AddMargins({ 0.0f, edgeWidthHalf, 0.0f, edgeWidthHalf })
                    .Move({ 0.0f, -spacingHalf });
            }
            else
            {
                prevEdge->bounds = Bounds2f32(element.bounds.left, grantedBounds.top, element.bounds.left, grantedBounds.bottom)
                    .AddMargins({ edgeWidthHalf, 0.0f, edgeWidthHalf, 0.0f })
                    .Move({ -spacingHalf, 0.0f });
            }
        }
    }

    template<typename TSkin>
    constexpr typename Docker<TSkin>::Direction Docker<TSkin>::FlipDirection(const Direction direction)
    {
        return direction == Direction::Horizontal ? Direction::Vertical : Direction::Horizontal;
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr float Docker<TSkin>::GetDirectionalWidth(const Vector2f32& size)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            return size.x;
        }
        else
        {
            return size.y;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr float Docker<TSkin>::GetDirectionalHeight(const Vector2f32& size)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            return size.y;
        }
        else
        {
            return size.x;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr void Docker<TSkin>::SetDirectionalWidth(Vector2f32& size, const float width)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            size.x = width;
        }
        else
        {
            size.y = width;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr void Docker<TSkin>::SetDirectionalHeight(Vector2f32& size, const float height)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            size.y = height;
        }
        else
        {
            size.x = height;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr void Docker<TSkin>::AddDirectionalWidth(Vector2f32& size, const float width)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            size.x += width;
        }
        else
        {
            size.y += width;
        }
    }

    template<typename TSkin>
    template<typename Docker<TSkin>::Direction VDirection>
    constexpr void Docker<TSkin>::AddDirectionalHeight(Vector2f32& size, const float height)
    {
        if constexpr (VDirection == Direction::Horizontal)
        {
            size.y += height;
        }
        else
        {
            size.x += height;
        }
    }

    template<typename TSkin>
    bool Docker<TSkin>::GetDockingPositionInElement(
        const Vector2f32& mousePosition,
        Leaf& leaf,
        Bounds2f32& dockingBounds,
        DockingPosition& position)
    {
        auto* element = leaf.AsElement();
        if (!element)
        {
            return false;
        }

        auto oneThirdSize = element->bounds.GetSize() / 3.0f;

        // Wow, WTF, optimize this, please...
        const auto leftBounds = Bounds2f32{ 
            element->bounds.left,
            element->bounds.top + oneThirdSize.y,
            element->bounds.left + oneThirdSize.x,
            element->bounds.bottom - oneThirdSize.y
        };
        if (leftBounds.Intersects(mousePosition))
        {
            dockingBounds = leftBounds;
            position = DockingPosition::Left;
            return true;
        }

        const auto topBounds = Bounds2f32{
           element->bounds.left + oneThirdSize.x,
           element->bounds.top,
           element->bounds.right - oneThirdSize.x,
           element->bounds.top + oneThirdSize.y
        };
        if (topBounds.Intersects(mousePosition))
        {
            dockingBounds = topBounds;
            position = DockingPosition::Top;
            return true;
        }

        const auto rightBounds = Bounds2f32{
           element->bounds.right - oneThirdSize.x,
           element->bounds.top + oneThirdSize.y,
           element->bounds.right,
           element->bounds.bottom - oneThirdSize.y
        };
        if (rightBounds.Intersects(mousePosition))
        {
            dockingBounds = rightBounds;
            position = DockingPosition::Right;
            return true;
        }

        const auto bottomBounds = Bounds2f32{
           element->bounds.left + oneThirdSize.x,
           element->bounds.bottom - oneThirdSize.y,
           element->bounds.right - oneThirdSize.x,
           element->bounds.bottom
        };
        if (bottomBounds.Intersects(mousePosition))
        {
            dockingBounds = bottomBounds;
            position = DockingPosition::Bottom;
            return true;
        }

        return false;
    }

    template<typename TSkin>
    Docker<TSkin>::EdgeDragData::EdgeDragData() :
        pressedEdge(nullptr),
        prevMousePosition(0.0f, 0.0f)
    { }

    template<typename TSkin>
    void Docker<TSkin>::EdgeDragData::Reset()
    {
        pressedEdge = nullptr;
        prevMousePosition = { 0.0f, 0.0f };
    }

    template<typename TSkin>
    Docker<TSkin>::LeafDragData::LeafDragData() :
        pressedLeaf(nullptr),
        initialMousePosition(0.0f, 0.0f),
        dockingLeaf(nullptr),
        dockingPosition(DockingPosition::Left),
        dragIsActivated(false)
    {}
    
    template<typename TSkin>
    void Docker<TSkin>::LeafDragData::Reset()
    {
        pressedLeaf = nullptr;
        initialMousePosition = {0.0f, 0.0f};
        dockingLeaf = nullptr;
        dockingPosition = DockingPosition::Left;
        dragIsActivated = false;
    }

}
