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

namespace Curse
{

    namespace Ecs
    {


        /// Implementations of system base class.
        template<typename ContextType>
        inline void SystemBase<ContextType>::OnRegister()
        { }

        template<typename ContextType>
        inline void SystemBase<ContextType>::OnCreateEntity(Entity<ContextType>)
        { }

        template<typename ContextType>
        inline void SystemBase<ContextType>::OnDestroyEntity(Entity<ContextType>)
        { }

        template<typename ContextType>
        inline SystemBase<ContextType>::SystemBase() :
            m_entityCount(0),
            m_componentGroup(nullptr)
        { }

        template<typename ContextType>
        inline void SystemBase<ContextType>::InternalOnRegister(Private::ComponentGroup<ContextType>* componentGroup)
        {
            m_componentGroup = componentGroup;
            m_entityCount = componentGroup->entityCount;
            OnRegister();
        }

        template<typename ContextType>
        inline void SystemBase<ContextType>::InternalOnCreateEntity(Entity<ContextType> entity)
        {
            ++m_entityCount;
            OnCreateEntity(entity);
        }

        template<typename ContextType>
        inline void SystemBase<ContextType>::InternalOnDestroyEntity(Entity<ContextType> entity)
        {
            --m_entityCount;
            OnDestroyEntity(entity);
        }


        /// Implementations of system class.
        template<typename ContextType, typename DerivedSystem, typename ... RequiredComponents>
        template<typename Comp>
        Comp& System<ContextType, DerivedSystem, RequiredComponents...>::GetComponent(const size_t entityIndex)
        {
            static_assert(TemplateArgumentsContains<Comp, RequiredComponents...>(),
                "Provided type for GetComponent is not available for this system.");

            const size_t componentIndex = (entityIndex * SystemBase<ContextType>::m_componentGroup->componentsPerEntity) + Private::ComponentIndex<Comp, RequiredComponents...>::index;
            return *static_cast<Comp*>(SystemBase<ContextType>::m_componentGroup->components[componentIndex]);
        }

        template<typename ContextType, typename DerivedSystem, typename ... RequiredComponents>
        inline size_t System<ContextType, DerivedSystem, RequiredComponents...>::GetEntityCount() const
        {
            return SystemBase<ContextType>::m_entityCount;
        }

    }

}