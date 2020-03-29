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

#include "Test.hpp"
#include "Curse/Ecs/EcsContext.hpp"
#include "Curse/Math/Vector.hpp"



//CURSE_COMPONENT(MyComp, )

namespace Curse
{

    namespace Ecs
    {

        CURSE_ECS_CONTEXT(TestContext)
        {
            TestContext() :
                Context<TestContext>(ContextDescriptor{4000})
            {}
        };

        using TestEntity = Entity<Context<TestContext>>;

        CURSE_ECS_COMPONENT(TestTranslation, TestContext)
        {
            Vector3f32 position;
            Vector3f32 scale;
        };
        CURSE_ECS_COMPONENT(TestPhysics, TestContext)
        {
            Vector3f32 velocity;
            float weight;
        };
        CURSE_ECS_COMPONENT(TestCharacter, TestContext)
        {
            char name[50];
        };

        CURSE_ECS_SYSTEM(TestSystem, TestContext, TestTranslation, TestPhysics, TestCharacter)
        {

            void OnRegister() override
            {
                ++onRegisterCount;
            }

            void OnCreateEntity(TestEntity entity) override
            {
                ++onCreatedEntityCount;
            }
 
            void OnDestroyEntity(TestEntity entity) override
            {
                ++onDestroyedEntityCount;
            }

            void Process(const Time & deltaTime) override
            {
                loopedEntities = 0;
                for (size_t i = 0; i < GetEntityCount(); i++)
                {
                    auto& trans = GetComponent<TestTranslation>(i);
                    trans.position = {};
                    trans.scale = {};

                    auto& phys = GetComponent<TestPhysics>(i);
                    phys.velocity = {};
                    phys.weight = 0.0f;

                    auto& character = GetComponent<TestCharacter>(i);
                    std::memcpy(character.name, "test", 5);

                    loopedEntities++;
                }
            }

            static inline size_t onRegisterCount = 0;
            static inline size_t onCreatedEntityCount = 0;
            static inline size_t onDestroyedEntityCount = 0;
            static inline size_t loopedEntities = 0;

        };

        TEST(Ecs, Systems)
        {
            EXPECT_EQ(TestTranslation::componentTypeId, ComponentTypeId(0));
            EXPECT_EQ(TestPhysics::componentTypeId, ComponentTypeId(1));
            EXPECT_EQ(TestCharacter::componentTypeId, ComponentTypeId(2));

            TestContext context;
            TestSystem testSystem;

            EXPECT_EQ(TestSystem::onRegisterCount, size_t(0));
            context.RegisterSystem(testSystem);
            EXPECT_EQ(TestSystem::onRegisterCount, size_t(1));

            EXPECT_EQ(TestSystem::onCreatedEntityCount, size_t(0));
            auto e1 = context.CreateEntity<TestTranslation, TestPhysics, TestCharacter>();
            EXPECT_EQ(TestSystem::onCreatedEntityCount, size_t(1));
            
            auto e2 = context.CreateEntity<TestTranslation, TestCharacter>();
            auto e3 = context.CreateEntity<TestTranslation, TestPhysics>();
            auto e4 = context.CreateEntity<>();
            auto e5 = context.CreateEntity<TestPhysics>();

            EXPECT_EQ(TestSystem::onCreatedEntityCount, size_t(1));
            auto e6 = context.CreateEntity<TestTranslation, TestPhysics, TestCharacter>();
            EXPECT_EQ(TestSystem::onCreatedEntityCount, size_t(2));

            testSystem.Process(Seconds<float>(1.0));
            EXPECT_EQ(TestSystem::loopedEntities, size_t(2));

            EXPECT_EQ(TestSystem::onDestroyedEntityCount, size_t(0));
            context.DestroyEntity(e1);
            EXPECT_EQ(TestSystem::onDestroyedEntityCount, size_t(1));

            testSystem.Process(Seconds<float>(1.0));
            EXPECT_EQ(TestSystem::loopedEntities, size_t(1));

            EXPECT_EQ(TestSystem::onCreatedEntityCount, size_t(2));
            auto e7 = context.CreateEntity<TestTranslation, TestPhysics, TestCharacter>();
            EXPECT_EQ(TestSystem::onCreatedEntityCount, size_t(3));

            testSystem.Process(Seconds<float>(1.0));
            EXPECT_EQ(TestSystem::loopedEntities, size_t(2));

            context.AddComponents<TestCharacter>(e3);
            EXPECT_EQ(TestSystem::onCreatedEntityCount, size_t(4));

            testSystem.Process(Seconds<float>(1.0));
            EXPECT_EQ(TestSystem::loopedEntities, size_t(3));

        }
    }

}