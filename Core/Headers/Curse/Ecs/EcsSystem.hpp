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

#ifndef CURSE_CORE_ECS_ECSSYSTEM_HPP
#define CURSE_CORE_ECS_ECSSYSTEM_HPP

#include "Curse/Ecs/Ecs.hpp"
#include "Curse/Ecs/EcsEntity.hpp"
#include "Curse/Ecs/EcsComponent.hpp"
#include "Curse/Ecs/EcsSignature.hpp"
#include "Curse/System/Time.hpp"

namespace Curse
{

    namespace Ecs
    {

        /**
        * @brief Base class of system.
        *        Multiple functions are available for overloading, for example OnAddEntity.
        *        The user is not supposed to inherit from this class, but the System<...> class.
        * @see System.
        */
        template<typename ContextType>
        class SystemBase
        {

        public:

            /**
            * @brief Callback function, being called when the system is registered in the context.
            */
            virtual void OnRegister();

            /**
            * @brief Callback function, being called when a new entity of interest is created.
            */
            virtual void OnCreateEntity(Entity<ContextType> entity);

            /**
            * @brief Callback function, being called when a new entity of interest is destroyed, and thus removed.
            */
            virtual void OnDestroyEntity(Entity<ContextType> entity);

            /**
            * @brief Function for overriding, handling processing of entities.
            */
            virtual void Process(const Time& deltaTime) = 0;

        protected:

            SystemBase();

            size_t m_entityCount;
            Private::ComponentGroup<ContextType>* m_componentGroup;

        private:

            void InternalOnRegister(Private::ComponentGroup<ContextType>* componentGroup);
            void InternalOnCreateEntity(Entity<ContextType> entity);
            void InternalOnDestroyEntity(Entity<ContextType> entity);

            friend typename ContextType; ///< Friend class
        };


        /*
        * @brief System class, processing entities of interest.
        *        Entities of interest is specified in the template parameter list, by providing a set a components.
        * 
        * Make sure to register all systems to the context before creating any entities.
        * It is not possible to unregister a system from a context.
        */
        template<typename ContextType, typename DerivedSystem, typename ... RequiredCompnents>
        class System : public SystemBase<ContextType>
        {

        public:

            static inline const Signature signature = CreateSignature<RequiredCompnents...>();

            /**
            * @brief Get component by entity index.
            */
            template<typename Comp>
            Comp& GetComponent(const size_t entityIndex);

            /**
            * @brief Get number of entities being monitored by this system.
            */
            size_t GetEntityCount() const;

        protected:

        private:

            friend typename ContextType; ///< Friend class.


        };

    }

}

#include "Curse/Ecs/EcsSystem.inl"

#endif