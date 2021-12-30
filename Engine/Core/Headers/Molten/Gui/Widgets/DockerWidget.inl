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
    // Docket implementation struct.
    template<typename TTheme>
    struct Docker<TTheme>::Impl
    {
        struct NewLeaf;
        struct Leaf;
        struct Grid;
        struct Edge;
        struct Element;

        using NewLeafSharedPtr = std::shared_ptr<NewLeaf>;
        using LeafPtr = std::unique_ptr<Leaf>;
        using GridPtr = std::unique_ptr<Grid>;
        using EdgePtr = std::unique_ptr<Edge>;
        using ElementPtr = std::unique_ptr<Element>;
        using ElementPtrList = std::vector<ElementPtr>;

        enum class Direction
        {
            Horizontal,
            Vertical
        };

        enum class InsertPosition
        {
            First,
            Last
        };

        struct NewLeaf
        {
            NewLeaf(
                const DockingPosition position,
                Widget<TTheme>* widget
            ) :
                position(position),
                widget(widget)
            {}

            DockingPosition position;
            Widget<TTheme>* widget;
        };

        struct Leaf
        {
            explicit Leaf(
                Widget<TTheme>* widget,
                const bool isDynamic
            ) :
                widget(widget),
                draggableWidget(dynamic_cast<DraggableWidget*>(widget)),
                isDynamic(isDynamic),
                element(nullptr)
            {}

            [[nodiscard]] bool IsDynamic() const
            {
                return isDynamic;
            }

            [[nodiscard]] Element* AsElement()
            {
                return element;
            }

            Widget<TTheme>* widget;
            DraggableWidget* draggableWidget;
            bool isDynamic;
            Element* element;
        };

        struct Grid
        {
            explicit Grid(const Direction direction) :
                direction(direction),
                element(nullptr)
            {}

            [[nodiscard]] bool IsDynamic() const
            {
                return !dynamicElements.empty();
            }

            [[nodiscard]] Element* AsElement()
            {
                return element;
            }

            Direction direction;
            ElementPtrList elements;
            std::set<Element*> dynamicElements;
            Element* element;
        };

        struct Edge
        {
            explicit Edge(const Direction direction, Element* prevElement, Element* nextElement) :
                direction(direction),
                prevElement(prevElement),
                nextElement(nextElement)
            {}

            Direction direction;
            AABB2f32 bounds;
            Element* prevElement;
            Element* nextElement;
        };

        struct Element
        {
            using Variant = std::variant<LeafPtr, GridPtr>;

            Element(
                LeafPtr leaf,
                const Vector2f32 requestedSize,
                const Vector2f32 minSize
            ) :
                variant(std::move(leaf)),
                requestedSize(requestedSize),
                minSize(minSize),
                parent(nullptr),
                prevEdge(nullptr),
                nextEdge(nullptr)
            {
                std::get<LeafPtr>(variant)->element = this;
            }

            explicit Element(GridPtr grid) :
                variant(std::move(grid)),
                parent(nullptr),
                prevEdge(nullptr),
                nextEdge(nullptr)
            {
                auto& thisGrid = std::get<GridPtr>(variant);
                thisGrid->element = this;

                for (auto& element : thisGrid->elements)
                {
                    element->parent = this;
                }
            }

            [[nodiscard]] bool IsDynamic() const
            {
                return std::visit([](const auto& value) { return value->IsDynamic(); }, variant);
            }

            Variant variant;
            Vector2f32 requestedSize;
            Vector2f32 minSize;
            Vector2f32 grantedSize;
            Element* parent;
            Edge* prevEdge;
            Edge* nextEdge;
        };

        static constexpr Direction GetInsertDirection(const DockingPosition dockingPosition)
        {
            return dockingPosition == DockingPosition::Left || dockingPosition == DockingPosition::Right ? Direction::Horizontal : Direction::Vertical;
        }

        static constexpr InsertPosition GetInsertPosition(const DockingPosition dockingPosition)
        {
            return dockingPosition == DockingPosition::Left || dockingPosition == DockingPosition::Top ? InsertPosition::First : InsertPosition::Last;
        }

        static constexpr Direction FlipDirection(const Direction direction)
        {
            return direction == Direction::Horizontal ? Direction::Vertical : Direction::Horizontal;
        }

        template<Direction VDirection>
        static constexpr float& GetDirectionalWidth(Vector2f32& size)
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

        template<Direction VDirection>
        static constexpr float& GetDirectionalHeight(Vector2f32& size)
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

        template<Direction VGridDirection>
        static constexpr void ShrinkBoundsBackDirectional(AABB2f32& bounds, float shrinkValue)
        {
            if constexpr (VGridDirection == Direction::Horizontal)
            {
                bounds.size.x -= shrinkValue;
            }
            else
            {
                bounds.size.y -= shrinkValue;
            }
        }

        template<Direction VGridDirection>
        static constexpr void ShrinkBoundsBackDirectional(AABB2f32& bounds, AABB2f32& erasedBounds, float shrinkValue)
        {
            if constexpr (VGridDirection == Direction::Horizontal)
            {
                erasedBounds = {
                    bounds.position + Vector2f32{ bounds.size.x - shrinkValue, 0.0f},
                    { shrinkValue, bounds.size.y }
                };

                bounds.size.x -= shrinkValue;
            }
            else
            {
                erasedBounds = {
                    bounds.position + Vector2f32{ 0.0f, bounds.size.y - shrinkValue},
                    { bounds.size.x, shrinkValue }
                };

                bounds.size.y -= shrinkValue;
            }
        }

        static constexpr Mouse::Cursor GetHoverEdgeCursor(Edge& edge)
        {
            return edge.direction == Direction::Horizontal ? Mouse::Cursor::SizeUpDown : Mouse::Cursor::SizeLeftRight;
        }


        explicit Impl(Docker<TTheme>& docker) :
            m_docker(docker),
            m_currentCursor(Mouse::Cursor::Normal),
            m_mouseInputUpdateFunc(&Impl::HandleDefaultMouseEvent),
            m_pressedEdge(nullptr),
            m_pressedLeaf(nullptr),
            m_dockingLeaf(nullptr),
            m_dockingLeafPosition(DockingPosition::Left)
        {}

        void QueueNewLeaf(Widget<TTheme>& widget)
        {
            auto newLeaf = std::make_shared<NewLeaf>(DockingPosition::Right, &widget);
            m_newLeafQueue.push(newLeaf);
            m_newLeafMap.insert({ &widget, newLeaf });
        }

        void ProcessNewLeafs()
        {
            m_newLeafMap.clear();

            if(m_newLeafQueue.empty())
            {
                return;
            }

            while (!m_newLeafQueue.empty())
            {
                auto& pendingLeaf = m_newLeafQueue.front();
                InsertLeaf(*pendingLeaf);
                m_newLeafQueue.pop();
            }
        }

        void InsertLeaf(NewLeaf& newLeaf)
        {
            auto* widget = newLeaf.widget;

            const bool isDynamic = !VariantEqualsType<Size::Pixels>(widget->size.x) || !VariantEqualsType<Size::Pixels>(widget->size.y);
            auto leaf = std::make_unique<Leaf>(widget, isDynamic);
            m_leafs.insert(leaf.get());
            m_leafMap.insert({ widget, leaf.get() });

            auto requestedSize = m_docker.minCellSize;
            if (VariantEqualsType<Size::Pixels>(widget->size.x))
            {
                requestedSize.x = std::max(m_docker.minCellSize.x, std::get<Size::Pixels>(widget->size.x).value);
            }
            if (VariantEqualsType<Size::Pixels>(widget->size.x))
            {
                requestedSize.y = std::max(m_docker.minCellSize.y, std::get<Size::Pixels>(widget->size.y).value);
            }

            auto newElement = std::make_unique<Element>(std::move(leaf), requestedSize, m_docker.minCellSize);

            if (m_rootElement)
            {
                InsertElement(*m_rootElement, std::move(newElement), newLeaf.position);
            }
            else
            {
                m_rootElement = std::move(newElement);
            }
        }

        void InsertElement(Element& parent, ElementPtr element, const DockingPosition dockingPosition)
        {
            const auto insertDirection = GetInsertDirection(dockingPosition);
            const auto insertPosition = GetInsertPosition(dockingPosition);

            // Parent is a leaf.
            if (VariantEqualsType<GridPtr>(parent.variant))
            {
                if (auto* grid = std::get<GridPtr>(parent.variant).get(); grid->direction == insertDirection)
                {
                    InsertElementInGrid(parent, std::move(element), insertPosition);
                    return;
                }

                TransformGridToFlippedGrid(parent);
                InsertElementInGrid(parent, std::move(element), insertPosition);
                return;
            }

            // Parent is a leaf.
            // Check if parent's parent is a grid and it's direction is matching, insert in parent's parent.
            if (parent.parent && VariantEqualsType<GridPtr>(parent.parent->variant))
            {
                if (auto* grid = std::get<GridPtr>(parent.parent->variant).get(); grid->direction == insertDirection)
                {
                    InsertElementInGrid(*parent.parent, std::move(element), parent, insertPosition);
                    return;
                }
                
            }

            // Transform this leaf to a grid and append.
            TransformLeafToGrid(parent, GetInsertDirection(dockingPosition));
            InsertElementInGrid(parent, std::move(element), insertPosition);
        }

        void InsertElementInGrid(Element& parent, ElementPtr element, const InsertPosition insertPosition)
        {
            MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(parent.variant), "Current element variant is not of type Grid.");

            auto& grid = *std::get<GridPtr>(parent.variant);
            auto it = insertPosition == InsertPosition::First ? grid.elements.begin() : grid.elements.end();
            InsertElementInGrid(parent, std::move(element), it);
        }

        void InsertElementInGrid(Element& parent, ElementPtr element, Element& neighborElement, const InsertPosition insertPosition)
        {
            MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(parent.variant), "Current element variant is not of type Grid.");

            auto& parentGrid = std::get<GridPtr>(parent.variant);

            auto it = std::find_if(parentGrid->elements.begin(), parentGrid->elements.end(), [&](auto& child)
            {
                return child.get() == &neighborElement;
            });

            MOLTEN_DEBUG_ASSERT(it != parentGrid->elements.end(), "Failed to find insert position in grid.");

            if (insertPosition == InsertPosition::Last)
            {
                ++it;
            }

            InsertElementInGrid(parent, std::move(element), it);
        }

        void InsertElementInGrid(Element& parent, ElementPtr element, typename ElementPtrList::iterator it)
        {
            MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(parent.variant), "Current element variant is not of type Grid.");

            auto& parentGrid = *std::get<GridPtr>(parent.variant);

            if (it == parentGrid.elements.begin()) // Insert a front.
            {
                if (auto* nextElement = parentGrid.elements.empty() ? nullptr : it->get(); nextElement)
                {
                    auto newEdge = std::make_unique<Edge>(FlipDirection(parentGrid.direction), element.get(), nextElement);

                    element->nextEdge = newEdge.get();
                    nextElement->prevEdge = newEdge.get();

                    m_edges.insert(std::move(newEdge));
                }
            }
            else if (it == parentGrid.elements.end()) // Insert at back.
            {
                if (auto* prevElement = parentGrid.elements.empty() ? nullptr : std::prev(it)->get(); prevElement)
                {
                    auto newEdge = std::make_unique<Edge>(FlipDirection(parentGrid.direction), prevElement, element.get());

                    prevElement->nextEdge = newEdge.get();
                    element->prevEdge = newEdge.get();

                    m_edges.insert(std::move(newEdge));
                }
            }       
            else // Insert in middle, somewhere.
            {
                auto* prevElement = std::prev(it)->get();
                auto* nextEdge = it->get()->prevEdge;

                auto newEdge = std::make_unique<Edge>(FlipDirection(parentGrid.direction), prevElement, element.get());

                prevElement->nextEdge = newEdge.get();
                element->prevEdge = newEdge.get();
                element->nextEdge = nextEdge;
                nextEdge->prevElement = element.get();

                m_edges.insert(std::move(newEdge));
            }

            element->parent = &parent;

            if (element->IsDynamic())
            {
                AddDynamicElementToParent(*element);
            }

            parentGrid.elements.insert(it, std::move(element));

            UpdateElementConstraints(parent);
            UpdateParentConstraints(parent);
        }

        ElementPtr ExtractElement(Element* element)
        {
            auto extractedElementAndEdge = ExtractElementAndEdge(element);
            auto extractedElement = std::move(extractedElementAndEdge.first);
            auto* extractedEdge = extractedElementAndEdge.second;

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

            return extractedElement;
        }

        std::pair<ElementPtr, Edge*> ExtractElementAndEdge(Element* element)
        {
            if (!element || !element->parent || !VariantEqualsType<GridPtr>(element->parent->variant))
            {
                return { nullptr, nullptr };
            }

            auto* parent = element->parent;
            auto& parentGrid = std::get<GridPtr>(parent->variant);

            // Find extract and erase from grid.
            auto elementIt = std::find_if(parentGrid->elements.begin(), parentGrid->elements.end(), [&](auto& child)
                {
                    return child.get() == element;
                });

            RemoveDynamicElementFromParents(element);

            auto extractedElement = std::move(*elementIt);
            parentGrid->elements.erase(elementIt);

            auto* extractedEdge = ExtractEdge(extractedElement.get());
            extractedElement->parent = nullptr;
            extractedElement->prevEdge = nullptr;
            extractedElement->nextEdge = nullptr;

            if (parentGrid->elements.size() >= 2)
            {
                UpdateConstraintsFromChildren(*parent, parentGrid->direction);
            }
            else
            {
                TransformToChild(*parent);
            }

            return { std::move(extractedElement), extractedEdge };
        }

        Edge* ExtractEdge(Element* element)
        {
            Edge* extactedEdge = nullptr;
            if ((extactedEdge = element->prevEdge) != nullptr)
            {
                auto* nextEdge = element->nextEdge;
                auto* prevElement = extactedEdge->prevElement;

                if (prevElement)
                {
                    prevElement->nextEdge = nextEdge;
                }
                if (nextEdge)
                {
                    nextEdge->prevElement = prevElement;
                }
            }
            else if ((extactedEdge = element->nextEdge) != nullptr)
            {
                auto* prevEdge = element->prevEdge;
                auto* nextElement = extactedEdge->nextElement;

                if (nextElement)
                {
                    nextElement->prevEdge = prevEdge;
                }
                if (prevEdge)
                {
                    prevEdge->nextElement = nextElement;
                }
            }

            if(extactedEdge)
            {
                extactedEdge->prevElement = nullptr;
                extactedEdge->nextElement = nullptr;
            }

            return extactedEdge;
        }

        void RemoveDynamicElementFromParents(Element* element)
        {
            auto* parent = element->parent;

            while (parent && !element->IsDynamic())
            {
                MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(parent->variant), "Parent element variant is not of type Grid.");

                auto* parentGrid = std::get<GridPtr>(parent->variant).get();

                auto it = parentGrid->dynamicElements.find(element);
                if (it == parentGrid->dynamicElements.end())
                {
                    return;
                }

                parentGrid->dynamicElements.erase(it);

                element = parent;
                parent = element->parent;
            }
        }

        void TransformLeafToGrid(Element& element, Direction direction)
        {
            MOLTEN_DEBUG_ASSERT(VariantEqualsType<LeafPtr>(element.variant), "Current element variant is not of type Leaf.");

            auto movedLeaf = std::move(std::get<LeafPtr>(element.variant));
            auto newElement = std::make_unique<Element>(std::move(movedLeaf), element.requestedSize, element.minSize);

            element.variant = std::make_unique<Grid>(direction);
            element.requestedSize = {};
            element.minSize = {};

            InsertElementInGrid(element, std::move(newElement), InsertPosition::First);
        }

        void TransformGridToFlippedGrid(Element& element)
        {
            MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(element.variant), "Current element variant is not of type Grid.");

            auto& grid = std::get<GridPtr>(element.variant);
            const auto flippedDirection = FlipDirection(grid->direction);
            auto newElement = std::make_unique<Element>(std::move(grid));

            element.variant = std::make_unique<Grid>(flippedDirection);
            element.requestedSize = {};
            element.minSize = {};

            InsertElementInGrid(element, std::move(newElement), InsertPosition::First);
        }

        void TransformToChild(Element& element)
        {
            MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(element.variant), "Current element variant is not of type Grid.");

            auto& grid = std::get<GridPtr>(element.variant);

            if (grid->elements.size() != 1)
            {
                return;
            }

            auto& child = grid->elements.front();
            if(VariantEqualsType<LeafPtr>(child->variant))
            {
                TransformToChildLeaf(element);
            }
            else
            {
                TransformToChildGrid(element);
            }
        }

        void TransformToChildLeaf(Element& element)
        {
            MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(element.variant), "Current element variant is not of type Grid.");

            auto& grid = std::get<GridPtr>(element.variant);
            auto* firstElement = grid->elements.front().get();

            MOLTEN_DEBUG_ASSERT(VariantEqualsType<LeafPtr>(firstElement->variant), "Child element variant is not of type Leaf.");

            element.minSize = firstElement->minSize;

            element.requestedSize = grid->direction == Direction::Horizontal ? 
                Vector2f32{ firstElement->requestedSize.x, element.requestedSize.y } :
                Vector2f32{ element.requestedSize.x, firstElement->requestedSize.y };

            element.requestedSize = {
                std::max(element.minSize.x, element.requestedSize.x),
                std::max(element.minSize.y, element.requestedSize.y) };

            auto movedLeaf = std::move(std::get<LeafPtr>(firstElement->variant));
            movedLeaf->element = &element;
            element.variant = std::move(movedLeaf);
        }

        void TransformToChildGrid(Element& element)
        {
            MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(element.variant), "Current element variant is not of type Grid.");

            auto& grid = std::get<GridPtr>(element.variant);
            auto* firstElement = grid->elements.front().get();

            element.minSize = firstElement->minSize;

            element.requestedSize = grid->direction == Direction::Horizontal ? 
                Vector2f32{ firstElement->requestedSize.x, element.requestedSize.y } :
                Vector2f32{ element.requestedSize.x, firstElement->requestedSize.y };

            element.requestedSize = {
                std::max(element.minSize.x, element.requestedSize.x),
                std::max(element.minSize.y, element.requestedSize.y) };

            auto movedGrid = std::move(std::get<GridPtr>(firstElement->variant));
            auto* gridPtr = movedGrid.get();
            movedGrid->element = &element;
            element.variant = std::move(movedGrid);

            for(auto& gridElement : gridPtr->elements)
            {
                gridElement->parent = &element;
            }

            if(element.parent && VariantEqualsType<GridPtr>(element.parent->variant))
            {
                auto& parentGrid = std::get<GridPtr>(element.parent->variant);
                if(parentGrid->direction == gridPtr->direction)
                {
                    TransformToParentGrid(element);
                }
            }
        }

        void TransformToParentGrid(Element& element)
        {
            MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(element.parent->variant), "Parent element variant is not of type Grid.");

            auto& parentGrid = std::get<GridPtr>(element.parent->variant);
            
            auto it = std::find_if(parentGrid->elements.begin(), parentGrid->elements.end(), [&](auto& child)
            {
                return child.get() == &element;
            });

            if(it == parentGrid->elements.end())
            {
                return;
            }
            
            auto* newPrevElement = it->get() != parentGrid->elements.front().get() ? std::prev(it)->get() : nullptr;
            auto* newNextElement = it->get() != parentGrid->elements.back().get() ? std::next(it)->get() : nullptr;
            
            auto extractedElement = std::move(*it);
            it = parentGrid->elements.erase(it);

            MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(extractedElement->variant), "Extracted element variant is not of type Grid.");
            auto& elementGrid = std::get<GridPtr>(extractedElement->variant);

            auto* oldFirstElement = elementGrid->elements.front().get();
            auto* oldLastElement = elementGrid->elements.back().get();

            // Insert this elements to parent grid.
            for(auto eIt = elementGrid->elements.rbegin(); eIt != elementGrid->elements.rend(); ++eIt)
            {
                (*eIt)->parent = element.parent;
                it = parentGrid->elements.insert(it, std::move(*eIt));
            }

            // Erase this elements from parent's dynamic elements.
            auto deIt = parentGrid->dynamicElements.find(&element);
            if(deIt != parentGrid->dynamicElements.end())
            {
                parentGrid->dynamicElements.erase(deIt);
                RemoveDynamicElementFromParents(&element);
            }

            // Add thid elements dynamic elements to this
            for(auto* dynamicElement : elementGrid->dynamicElements)
            {
                parentGrid->dynamicElements.insert(dynamicElement);
                AddDynamicElementToParent(*dynamicElement);
            }

            if (newPrevElement && newPrevElement->nextEdge)
            {
                newPrevElement->nextEdge->nextElement = oldFirstElement;
                oldFirstElement->prevEdge = newPrevElement->nextEdge;
            }
            if (newNextElement && newNextElement->prevEdge)
            {
                newNextElement->prevEdge->prevElement = oldLastElement;
                oldLastElement->nextEdge = newNextElement->prevEdge;
            }
            
            element.prevEdge = nullptr;
            element.nextEdge = nullptr;
        }

        void AddDynamicElementToParent(Element& element)
        {
            auto* elementPtr = &element;
            auto* parent = element.parent;

            while (parent && elementPtr->IsDynamic())
            {
                MOLTEN_DEBUG_ASSERT(VariantEqualsType<GridPtr>(parent->variant), "Parent element variant is not of type Grid.");

                auto* parentGrid = std::get<GridPtr>(parent->variant).get();

                auto it = parentGrid->dynamicElements.find(elementPtr);
                if (it != parentGrid->dynamicElements.end())
                {
                    return;
                }

                parentGrid->dynamicElements.insert(elementPtr);

                elementPtr = parent;
                parent = elementPtr->parent;
            }
        }

        void UpdateParentConstraints(Element& element)
        {
            auto* parent = element.parent;
            while (parent)
            {
                UpdateElementConstraints(*parent);
                parent = parent->parent;
            }
        }

        void UpdateElementConstraints(Element& element)
        {
            if(!VariantEqualsType<GridPtr>(element.variant))
            {
                return;
            }

            auto& grid = std::get<GridPtr>(element.variant);

            element.minSize = { 0.0f, 0.0f };
            element.requestedSize = { 0.0f, 0.0f };

            if (grid->direction == Direction::Horizontal)
            {
                for (auto& child : grid->elements)
                {
                    element.minSize.x += child->minSize.x;
                    element.requestedSize.x += child->requestedSize.x;

                    if (child->minSize.y > element.minSize.y)
                    {
                        element.minSize.y = child->minSize.y;
                    }
                    if (child->requestedSize.y > element.requestedSize.y)
                    {
                        element.requestedSize.y = child->requestedSize.y;
                    }
                }
            }
            else
            {
                for (auto& child : grid->elements)
                {
                    element.minSize.y += child->minSize.y;
                    element.requestedSize.y += child->requestedSize.y;

                    if (child->minSize.x > element.minSize.x)
                    {
                        element.minSize.x = child->minSize.x;
                    }
                    if (child->requestedSize.x > element.requestedSize.x)
                    {
                        element.requestedSize.x = child->requestedSize.x;
                    }
                }
            }
        }

        template<Direction VDirection>
        void UpdateConstraintsFromChildren(Element& element)
        {
            auto oldHeight = GetDirectionalHeight<VDirection>(element.requestedSize);
            UpdateElementConstraints(element);
            GetDirectionalHeight<VDirection>(element.requestedSize) = oldHeight;
        }

        void UpdateConstraintsFromChildren(Element& element, const Direction direction)
        {
            if (direction == Direction::Horizontal)
            {
                UpdateConstraintsFromChildren<Direction::Horizontal>(element);
            }
            else
            {
                UpdateConstraintsFromChildren<Direction::Vertical>(element);
            }
        }

        template<Direction VEdgeDirection>
        void SetElementPrevEdgeBounds(Element& element, const AABB2f32& elementBounds)
        {
            auto* prevEdge = element.prevEdge;
            if (!prevEdge)
            {
                return;
            }

            const auto edgeWidthHalf = m_docker.edgeWidth * 0.5f;
            const auto cellSpacingHalf = m_docker.cellSpacing * 0.5f;

            if constexpr (VEdgeDirection == Direction::Horizontal)
            {
                const auto middlePosition = elementBounds.position.y - cellSpacingHalf;

                prevEdge->bounds = {
                   { elementBounds.position.x, middlePosition - edgeWidthHalf },
                   { elementBounds.size.x, m_docker.edgeWidth }
                };
            }
            else
            {
                const auto middlePosition = elementBounds.position.x - cellSpacingHalf;

                prevEdge->bounds = {
                   { middlePosition - edgeWidthHalf, elementBounds.position.y},
                   { m_docker.edgeWidth, elementBounds.size.y }
                };
            }
        }

        template<typename TIterator>
        void HideElements(TIterator begin, TIterator end)
        {
            for (; begin != end; ++begin)
            {
                auto& element = *begin;

                if(VariantEqualsType<LeafPtr>(element->variant))
                {
                    auto& leaf = std::get<LeafPtr>(element->variant);
                    m_docker.SetGrantedSize(*leaf->widget, { 0.0f, 0.0f });
                }
                else
                {
                    auto& grid = std::get<GridPtr>(element->variant);
                    HideElements(grid->elements.rbegin(), grid->elements.rend());
                }
            }
        }

        void SetCursor(const Mouse::Cursor cursor)
        {
            if (m_currentCursor != cursor)
            {
                m_docker.onCursorChange(cursor);
                m_currentCursor = cursor;
            }
        }

        Edge* FindIntersectingEdge(const Vector2f32& point)
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

        Leaf* FindIntersectingDraggableLeaf(const Vector2f32& point)
        {
            for (auto* leaf : m_leafs)
            {
                if (leaf->draggableWidget && leaf->draggableWidget->GetDragBounds().Intersects(point))
                {
                    return leaf;
                }
            }

            return nullptr;
        }

        Leaf* FindIntersectingLeaf(const Vector2f32& point)
        {
            for (auto* leaf : m_leafs)
            {
                if(leaf->widget->GetBounds().Intersects(point))
                {
                    return leaf;
                }
            }
            return nullptr;
        }

        bool GetLeafDockingPosition(
            const Vector2f32& mousePosition,
            Leaf& leaf,
            AABB2f32& dockingBounds,
            DockingPosition& dockingPosition)
        {
            const auto& leafBounds = leaf.widget->GetBounds();
            const auto leafSize = leafBounds.size;
            const auto relativePosition = mousePosition - leafBounds.position;
            const auto oneThirdSize = leafSize / 3.0f;
            const auto halfSize = leafSize / 2.0f;

            const auto bounds1 = AABB2f32{ { 0.0f, 0.0f }, { oneThirdSize.x, leafSize.y } };
            if (bounds1.Intersects(relativePosition))
            {
                const auto triangle = Triangle2f32{ { 0.0f, 0.0f}, { 0.0f, leafSize.y }, halfSize };
                if (triangle.Intersects(relativePosition))
                {
                    dockingBounds = AABB2f32{ bounds1.position + leafBounds.position, bounds1.size };
                    dockingPosition = DockingPosition::Left;
                    return true;
                }
            }

            const auto bounds2 = AABB2f32{ { leafSize.x - oneThirdSize.x, 0.0f }, { oneThirdSize.x, leafSize.y } };
            if (bounds2.Intersects(relativePosition))
            {
                const auto triangle = Triangle2f32{ { leafSize.x, 0.0f}, leafSize, halfSize };
                if (triangle.Intersects(relativePosition))
                {
                    dockingBounds = AABB2f32{ bounds2.position + leafBounds.position, bounds2.size };
                    dockingPosition = DockingPosition::Right;
                    return true;
                }
            }

            const auto bounds3 = AABB2f32{ { 0.0f, 0.0f }, { leafSize.x, oneThirdSize.y } };
            if (bounds3.Intersects(relativePosition))
            {
                const auto triangle = Triangle2f32{ { 0.0f, 0.0f}, { leafSize.x, 0.0f }, halfSize };
                if (triangle.Intersects(relativePosition))
                {
                    dockingBounds = AABB2f32{ bounds3.position + leafBounds.position, bounds3.size };
                    dockingPosition = DockingPosition::Top;
                    return true;
                }
            }

            const auto bounds4 = AABB2f32{ { 0.0f, leafSize.y - oneThirdSize.y }, { leafSize.x, oneThirdSize.y } };
            if (bounds4.Intersects(relativePosition))
            {
                const auto triangle = Triangle2f32{ { 0.0f, leafSize.y}, leafSize, halfSize };
                if (triangle.Intersects(relativePosition))
                {
                    dockingBounds = AABB2f32{ bounds4.position + leafBounds.position, bounds4.size };
                    dockingPosition = DockingPosition::Bottom;
                    return true;
                }
            }

            return false;
        }

        void CalculateElementBounds()
        {
            if (!m_rootElement)
            {
                return;
            }

            CalculateElementBounds(*m_rootElement.get(), m_contentBounds);
        }

        void CalculateElementBounds(Element& element, const AABB2f32& grantedBounds)
        {
            std::visit([&](auto& data)
            {
                CalculateElementBounds(element, *data, grantedBounds);
            }, element.variant);
        }

        void CalculateElementBounds(Element&, Leaf& leaf, const AABB2f32& grantedBounds)
        {
            m_docker.SetPosition(*leaf.widget, grantedBounds.position);

            if(grantedBounds.IsEmpty())
            {
                leaf.widget->size = { Size::Pixels{ 0 }, Size::Pixels{ 0 } };
                m_docker.SetGrantedSize(*leaf.widget, { 0.0f, 0.0f });
            }
            else
            {
                leaf.widget->size = { Size::Pixels{ grantedBounds.size.x }, Size::Pixels{ grantedBounds.size.y } };
                m_docker.SetGrantedSize(*leaf.widget, grantedBounds.size);
                m_docker.DrawChild(*leaf.widget);
            }
        }

        void CalculateElementBounds(Element& element, Grid& grid, const AABB2f32& grantedBounds)
        {
            if (grid.direction == Direction::Horizontal)
            {
                return CalculateElementBounds<Direction::Horizontal>(element, grid, grantedBounds);
            }
            
            return CalculateElementBounds<Direction::Vertical>(element, grid, grantedBounds);
        }

        template<Direction VGridDirection>
        void CalculateElementBounds(Element& element, Grid& grid, const AABB2f32& grantedBounds)
        {
            if (grid.elements.empty())
            {
                return;
            }

            auto grantedSize = grantedBounds.size;
            const auto spacingTotalWidth = grid.elements.size() > 1 ? static_cast<float>(grid.elements.size() - 1) * m_docker.cellSpacing : 0.0f;
            const auto grantedWidth = GetDirectionalWidth<VGridDirection>(grantedSize) - spacingTotalWidth;
            const auto requestedWidth = GetDirectionalWidth<VGridDirection>(element.requestedSize);
            auto widthDiff = grantedWidth - requestedWidth;

            // Distribute dynamic size.
            if (!grid.dynamicElements.empty())
            {
                auto partialWidthDiff = widthDiff / static_cast<float>(grid.dynamicElements.size());
                for (auto it = grid.dynamicElements.rbegin(); it != grid.dynamicElements.rend(); ++it)
                {
                    auto& childElement = *(*it);

                    auto childPartialWidthDiff = std::max(
                        partialWidthDiff,
                        GetDirectionalWidth<VGridDirection>(childElement.minSize) - GetDirectionalWidth<VGridDirection>(childElement.requestedSize));

                    GetDirectionalWidth<VGridDirection>(childElement.requestedSize) += childPartialWidthDiff;
                    childElement.grantedSize = childElement.requestedSize;

                    widthDiff -= childPartialWidthDiff;
                }
            }

            // Distribute remaining diff
            for (auto it = grid.elements.begin(); it != grid.elements.end(); ++it)
            {
                auto& childElement = *it->get();

                if (childElement.IsDynamic())
                {
                    continue;
                }

                auto childWidthDiff = std::max(
                    widthDiff,
                    GetDirectionalWidth<VGridDirection>(childElement.minSize) - GetDirectionalWidth<VGridDirection>(childElement.requestedSize));

                childElement.grantedSize = childElement.requestedSize;
                GetDirectionalWidth<VGridDirection>(childElement.grantedSize) += childWidthDiff;

                widthDiff -= childWidthDiff;
            }

            // Set bounds.
            auto boundsLeft = grantedBounds;
            auto it = grid.elements.rbegin();

            for (; it != grid.elements.rend(); ++it)
            {
                if (boundsLeft.IsEmpty())
                {
                    break;
                }

                auto& childElement = *it->get();
                auto elementBounds = AABB2f32{ };
                auto shrinkSize = GetDirectionalWidth<VGridDirection>(childElement.grantedSize);
                ShrinkBoundsBackDirectional<VGridDirection>(boundsLeft, elementBounds, shrinkSize);
                ShrinkBoundsBackDirectional<VGridDirection>(boundsLeft, m_docker.cellSpacing);

                constexpr Direction flippedDirection = FlipDirection(VGridDirection);
                SetElementPrevEdgeBounds<flippedDirection>(childElement, elementBounds);

                CalculateElementBounds(childElement, elementBounds);
            }

            HideElements(it, grid.elements.rend());

            UpdateConstraintsFromChildren<VGridDirection>(element);
        }

        void ActivateDefaultState()
        {
            m_mouseInputUpdateFunc = &Impl::HandleDefaultMouseEvent;
            m_pressedEdge = nullptr;
            m_pressedLeaf = nullptr;
            m_dockingLeaf = nullptr;
            SetCursor(Mouse::Cursor::Normal);
        }

        void ActivateEdgeDrag(Edge* edge, const Vector2f32& mousePosition, const Mouse::Button button)
        {
            m_pressedEdge = edge;
            m_mouseStatePosition = mousePosition;
            m_mouseInputUpdateFunc = &Impl::HandleEdgeDragMouseEvent;
            m_docker.GetCanvas()->OverrideMouseEventsUntilMouseRelease(m_docker, button);
            SetCursor(GetHoverEdgeCursor(*edge));
        }

        void ActivateLeafDrag(Leaf* leaf, const Vector2f32& mousePosition, const Mouse::Button button)
        {
            //    /*m_edgeDragData.Reset();
            //    m_leafDragData.Reset();

            //    m_leafDragData.pressedLeaf = pressedLeaf;
            //    m_leafDragData.initialMousePosition = mousePosition;

            //    m_mouseInputUpdateFunc = &Docker<TTheme>::HandleLeafDragMouseEvent;
            //    this->GetData().GetCanvas()->OverrideMouseEventsUntilMouseRelease(*this, button);*/

            m_pressedLeaf = leaf;
            m_mouseStatePosition = mousePosition;
            m_mouseInputUpdateFunc = &Impl::HandleLeafDragMouseEvent;
            m_docker.GetCanvas()->OverrideMouseEventsUntilMouseRelease(m_docker, button);
            SetCursor(Mouse::Cursor::SizeAll);
        }

        bool HandleMouseEvent(const WidgetMouseEvent& widgetMouseEvent)
        {
            return (*this.*m_mouseInputUpdateFunc)(widgetMouseEvent);
        }

        bool HandleDefaultMouseEvent(const WidgetMouseEvent& widgetMouseEvent)
        {
            switch (widgetMouseEvent.type)
            {
                case WidgetMouseEventType::MouseButtonPressed: return HandleDefaultMousePressEvent(widgetMouseEvent);
                case WidgetMouseEventType::MouseMove: return HandleDefaultMouseMoveEvent(widgetMouseEvent);
                case WidgetMouseEventType::MouseLeave: SetCursor(Mouse::Cursor::Normal); break;
                default: break;
            }
            return false;
        }

        bool HandleDefaultMouseMoveEvent(const WidgetMouseEvent& widgetMouseEvent)
        {
            if (auto* hoveredEdge = FindIntersectingEdge(widgetMouseEvent.position); hoveredEdge)
            {
                SetCursor(GetHoverEdgeCursor(*hoveredEdge));
                return true;
            }

            SetCursor(Mouse::Cursor::Normal);
            return false;
        }

        bool HandleDefaultMousePressEvent(const WidgetMouseEvent& widgetMouseEvent)
        {
            if (auto* pressedEdge = FindIntersectingEdge(widgetMouseEvent.position); pressedEdge)
            {
                ActivateEdgeDrag(pressedEdge, widgetMouseEvent.position, widgetMouseEvent.button);
                return true;
            }

            if (auto* pressedLeaf = FindIntersectingDraggableLeaf(widgetMouseEvent.position); pressedLeaf)
            {
                ActivateLeafDrag(pressedLeaf, widgetMouseEvent.position, widgetMouseEvent.button);
                return true;
            }
            return false;
        }

        bool HandleEdgeDragMouseEvent(const WidgetMouseEvent& widgetMouseEvent)
        {
            switch(widgetMouseEvent.type)
            {
                case WidgetMouseEventType::MouseMove: return HandleEdgeDragMouseMoveEvent(widgetMouseEvent);
                case WidgetMouseEventType::MouseButtonReleasedIn:
                case WidgetMouseEventType::MouseButtonReleasedOut: return HandleEdgeDragMouseReleaseEvent();
                default: break;
            }
            return false;
        }

        bool HandleEdgeDragMouseMoveEvent(const WidgetMouseEvent& widgetMouseEvent)
        {
            if (!m_pressedEdge)
            {
                ActivateDefaultState();
                return false;
            }

            SetCursor(GetHoverEdgeCursor(*m_pressedEdge));

            const auto mouseMovement = Vector2f32(widgetMouseEvent.position) - m_mouseStatePosition;
            const bool movedEdge = m_pressedEdge->direction == Direction::Horizontal ?
                HandleDirectionalEdgeMovement<Direction::Horizontal>(*m_pressedEdge, mouseMovement.y) :
                HandleDirectionalEdgeMovement<Direction::Vertical>(*m_pressedEdge, mouseMovement.x);

            if (movedEdge)
            {
                m_mouseStatePosition = widgetMouseEvent.position;
            }

            return true;
        }

        bool HandleEdgeDragMouseReleaseEvent()
        {
            ActivateDefaultState();
            return true;
        }

        template<Direction VEdgeDirection>
        bool HandleDirectionalEdgeMovement(Edge& edge, float movement)
        {
            constexpr Direction flippedDirection = FlipDirection(VEdgeDirection);

            auto* prevElement = edge.prevElement;
            auto* nextElement = edge.nextElement;

            movement = std::max(
                movement,
                GetDirectionalWidth<flippedDirection>(prevElement->minSize) - GetDirectionalWidth<flippedDirection>(prevElement->requestedSize));

            movement = std::min(
                movement,
                GetDirectionalWidth<flippedDirection>(nextElement->requestedSize) - GetDirectionalWidth<flippedDirection>(nextElement->minSize));

            if (movement == 0.0f)
            {
                return false;
            }

            if (prevElement)
            {
                GetDirectionalHeight<VEdgeDirection>(prevElement->requestedSize) += movement;
            }
            if (nextElement)
            {
                GetDirectionalHeight<VEdgeDirection>(nextElement->requestedSize) -= movement;
            }

            return true;
        }

        bool HandleLeafDragMouseEvent(const WidgetMouseEvent& widgetMouseEvent)
        {
            switch (widgetMouseEvent.type)
            {
                case WidgetMouseEventType::MouseMove: return HandleLeafDragMouseMoveEvent(widgetMouseEvent);
                case WidgetMouseEventType::MouseButtonReleasedIn:
                case WidgetMouseEventType::MouseButtonReleasedOut: return HandleLeafDragMouseReleaseEvent();
                default: break;
            }
            return false;
        }

        bool HandleLeafDragMouseMoveEvent(const WidgetMouseEvent& widgetMouseEvent)
        {
            /*if (!m_leafDragData.dragIsActivated)
            {
                const float distance = (m_leafDragData.initialMousePosition - widgetMouseEvent.position).Length();
                if (distance < widgetDragActivationDistance)
                {
                    return false;
        
                }
                m_leafDragData.dragIsActivated = true;
            }*/

            SetCursor(Mouse::Cursor::SizeAll);

            auto* leaf = FindIntersectingLeaf(widgetMouseEvent.position);
            if (!leaf || leaf == m_pressedLeaf)
            {
                return false;
            }

            AABB2f32 dockingBounds = {};
            if (GetLeafDockingPosition(widgetMouseEvent.position, *leaf, dockingBounds, m_dockingLeafPosition))
            {
                m_dockingLeaf = leaf;

                //if(!m_leafDragOverlay)
                {
                    //m_leafDragOverlay = m_docker.GetCanvas()->template CreateOverlayChild<DockerOverlay>();
                }
                
                //m_leafDragOverlay->position = WidgetPosition{ Position::Pixels{ dockingBounds.position.x }, Position::Pixels{ dockingBounds.position.y } };
                //m_leafDragOverlay->size = WidgetSize{ Size::Pixels{ dockingBounds.size.x }, Size::Pixels{ dockingBounds.size.y } };
            }
            else
            {
                //m_leafDragOverlay = {};
                m_dockingLeaf = nullptr;
            }

            return true;
        }

        bool HandleLeafDragMouseReleaseEvent()
        {
            if (m_dockingLeaf && m_pressedLeaf && m_dockingLeaf != m_pressedLeaf)
            {
                auto* fromElement = m_pressedLeaf->AsElement();
                auto* toElement = m_dockingLeaf->AsElement();
                
                if (fromElement && toElement)
                {
                    if (auto extractedElement = ExtractElement(fromElement); extractedElement)
                    {
                        toElement = m_dockingLeaf->AsElement(); // Get as element again. Very important to do after a call to ExtractElement.
                        InsertElement(*toElement, std::move(extractedElement), m_dockingLeafPosition);
                    }
                }
            }
            
            SetCursor(Mouse::Cursor::Normal);
            ActivateDefaultState();

            return true;
        }


        Docker<TTheme>& m_docker;
        ElementPtr m_rootElement;
        AABB2f32 m_contentBounds;
        std::set<EdgePtr> m_edges;
        std::set<Leaf*> m_leafs;
        std::map<Widget<TTheme>*, Leaf*> m_leafMap;
        std::queue<NewLeafSharedPtr> m_newLeafQueue;
        std::map<Widget<TTheme>*, NewLeafSharedPtr> m_newLeafMap;
        Mouse::Cursor m_currentCursor;
        bool(Impl::* m_mouseInputUpdateFunc)(const WidgetMouseEvent&);
        Vector2f32 m_mouseStatePosition;
        Edge* m_pressedEdge;
        Leaf* m_pressedLeaf;
        //ManagedWidget<TTheme, DockerOverlay> m_leafDragOverlay;
        Leaf* m_dockingLeaf;
        DockingPosition m_dockingLeafPosition;
        
    };


    // Docker widget implementations.
    template<typename TTheme>
    Docker<TTheme>::Docker(WidgetMixinDescriptor<TTheme, Docker>& desc) :
        WidgetMixin<TTheme, Docker>(desc),
        edgeWidth(WidgetMixin<TTheme, Docker>::WidgetSkinType::edgeWidth),
        cellSpacing(WidgetMixin<TTheme, Docker>::WidgetSkinType::cellSpacing),
        minCellSize(WidgetMixin<TTheme, Docker>::WidgetSkinType::minCellSize),
        defaultCellSize(WidgetMixin<TTheme, Docker>::WidgetSkinType::defaultCellSize),
        m_impl(*this)
    {}

    template<typename TTheme>
    template<template<typename> typename TWidget, typename ... TArgs>
    TWidget<TTheme>* Docker<TTheme>::CreateChild(
        const DockingPosition dockingPosition,
        TArgs ... args)
    {
        auto* widget = Widget<TTheme>::template CreateChild<TWidget>(std::forward<TArgs>(args)...);

        auto it = m_impl.m_newLeafMap.find(widget);
        if (it == m_impl.m_newLeafMap.end())
        {
            return widget;
        }
        it->second->position = dockingPosition;

        return widget;
    }

    template<typename TTheme>
    void Docker<TTheme>::PreUpdate()
    {
        m_impl.ProcessNewLeafs();

        this->SetPosition(this->GetBounds().position + this->margin.low);
        this->SetSize(this->GetGrantedSize() - this->margin.low - this->margin.high);

        m_impl.m_contentBounds = this->GetBounds();
        m_impl.m_contentBounds.position += this->padding.low;
        m_impl.m_contentBounds.size -= this->padding.low + this->padding.high;

        if(m_impl.m_contentBounds.IsEmpty())
        {
            return;
        }

        m_impl.CalculateElementBounds();

        this->UpdateAllChildren();
    }

    template<typename TTheme>
    void Docker<TTheme>::OnAddChild(Widget<TTheme>& child)
    {
        m_impl.QueueNewLeaf(child);
    }

    template<typename TTheme>
    bool Docker<TTheme>::OnMouseEvent(const WidgetMouseEvent& widgetMouseEvent)
    {
        return m_impl.HandleMouseEvent(widgetMouseEvent);
    }

}
