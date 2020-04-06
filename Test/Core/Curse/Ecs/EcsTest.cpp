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
#include <type_traits>


namespace Curse
{

    namespace Ecs
    {

        CURSE_ECS_CONTEXT(TestContext)
        {
            TestContext(const ContextDescriptor& descriptor = ContextDescriptor(4000)) :
                Context<TestContext>(descriptor)
            {}
        };

        using TestEntity = Entity<Context<TestContext>>;

        static size_t g_testTranslationConstructorCalls = 0;
        static size_t g_testPhysicsConstructorCalls = 0;
        static size_t g_testCharacterConstructorCalls = 0;

        CURSE_ECS_COMPONENT(TestTranslation, TestContext)
        {
            TestTranslation() :
                position(0, 0, 0), scale(0, 0, 0)
            {
                g_testTranslationConstructorCalls++;
            }

            Vector3i32 position;
            Vector3i32 scale;
        };
        CURSE_ECS_COMPONENT(TestPhysics, TestContext)
        {
            TestPhysics() :
                velocity(0, 0, 0), weight(0)
            {
                g_testPhysicsConstructorCalls++;
            }

            Vector3i32 velocity;
            int32_t weight;
        };
        CURSE_ECS_COMPONENT(TestCharacter, TestContext)
        {
            TestCharacter() :
                name("")
            {
                g_testCharacterConstructorCalls++;
            }

            char name[50];
        };

        CURSE_ECS_SYSTEM(TestPhysicsSystem, TestContext, TestTranslation, TestPhysics)
        {

            struct Data
            {
                TestTranslation translation;
                TestPhysics physics;
            };

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
                // Dummy imp. Check the Test* methods.
            }

            void TestLoopEntities()
            {
                loopedEntities = 0;
                for (size_t i = 0; i < GetEntityCount(); i++)
                {
                    loopedEntities++;
                }
            }

            void TestCheckEntities(const std::vector<Data> & data)
            {
                ASSERT_EQ(data.size(), GetEntityCount()); 

                loopedEntities = 0;
                for (size_t i = 0; i < GetEntityCount(); i++)
                {
                    loopedEntities++;

                    auto& trans = GetComponent<TestTranslation>(i);
                    auto& phys = GetComponent<TestPhysics>(i);
                    
                    EXPECT_EQ(trans.position, (data[i].translation.position));
                    EXPECT_EQ(trans.scale, (data[i].translation.scale));
                    EXPECT_EQ(phys.velocity, (data[i].physics.velocity));
                    EXPECT_EQ(phys.weight, (data[i].physics.weight));
                }
            }

            void ResetStats()
            {
                onRegisterCount = 0;
                onCreatedEntityCount = 0;
                onDestroyedEntityCount = 0;
                loopedEntities = 0;
            }

            size_t onRegisterCount = 0;
            size_t onCreatedEntityCount = 0;
            size_t onDestroyedEntityCount = 0;
            size_t loopedEntities = 0;

        };

        CURSE_ECS_SYSTEM(TestPlayerSystem, TestContext, TestTranslation, TestPhysics, TestCharacter)
        {

            struct Data
            {
                TestTranslation translation;
                TestPhysics physics;
                TestCharacter character;
            };

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

            void Process(const Time& deltaTime) override
            {
                // Dummy imp. Check the Test* methods.
            }

            void TestLoopEntities()
            {
                loopedEntities = 0;
                for (size_t i = 0; i < GetEntityCount(); i++)
                {
                    loopedEntities++;
                }
            }

            void TestCheckEntities(const std::vector<Data>& data)
            {
                ASSERT_EQ(data.size(), GetEntityCount());

                loopedEntities = 0;
                for (size_t i = 0; i < GetEntityCount(); i++)
                {
                    loopedEntities++;

                    auto& trans = GetComponent<TestTranslation>(i);
                    auto& phys = GetComponent<TestPhysics>(i);
                    auto& chara = GetComponent<TestCharacter>(i);

                    EXPECT_EQ(trans.position, (data[i].translation.position));
                    EXPECT_EQ(trans.scale, (data[i].translation.scale));
                    EXPECT_EQ(phys.velocity, (data[i].physics.velocity));
                    EXPECT_EQ(phys.weight, (data[i].physics.weight));
                    EXPECT_STREQ(chara.name, (data[i].character.name));
                }
            }

            void ResetStats()
            {
                onRegisterCount = 0;
                onCreatedEntityCount = 0;
                onDestroyedEntityCount = 0;
                loopedEntities = 0;
            }

            size_t onRegisterCount = 0;
            size_t onCreatedEntityCount = 0;
            size_t onDestroyedEntityCount = 0;
            size_t loopedEntities = 0;

        };


        TEST(ECS, CreateEntity)
        {
            EXPECT_EQ(TestTranslation::componentTypeId, ComponentTypeId(0));
            EXPECT_EQ(TestPhysics::componentTypeId, ComponentTypeId(1));
            EXPECT_EQ(TestCharacter::componentTypeId, ComponentTypeId(2));

            TestContext context;

            TestPhysicsSystem testPhysicsSystem1;
            TestPhysicsSystem testPhysicsSystem2;
            TestPlayerSystem testPlayerSystem;

            // Register systems.
            EXPECT_EQ(testPhysicsSystem1.onRegisterCount, size_t(0));
            EXPECT_EQ(testPhysicsSystem2.onRegisterCount, size_t(0));
            EXPECT_EQ(testPlayerSystem.onRegisterCount, size_t(0));
            context.RegisterSystem(testPhysicsSystem1);
            EXPECT_EQ(testPhysicsSystem1.onRegisterCount, size_t(1));
            EXPECT_EQ(testPhysicsSystem2.onRegisterCount, size_t(0));
            EXPECT_EQ(testPlayerSystem.onRegisterCount, size_t(0));
            context.RegisterSystem(testPhysicsSystem2);
            EXPECT_EQ(testPhysicsSystem1.onRegisterCount, size_t(1));
            EXPECT_EQ(testPhysicsSystem2.onRegisterCount, size_t(1));
            EXPECT_EQ(testPlayerSystem.onRegisterCount, size_t(0));
            context.RegisterSystem(testPlayerSystem);
            EXPECT_EQ(testPhysicsSystem1.onRegisterCount, size_t(1));
            EXPECT_EQ(testPhysicsSystem2.onRegisterCount, size_t(1));
            EXPECT_EQ(testPlayerSystem.onRegisterCount, size_t(1));

            // Already registered system.
            context.RegisterSystem(testPlayerSystem);
            EXPECT_EQ(testPhysicsSystem1.onRegisterCount, size_t(1));
            EXPECT_EQ(testPhysicsSystem2.onRegisterCount, size_t(1));
            EXPECT_EQ(testPlayerSystem.onRegisterCount, size_t(1));

            // Create/destroy empty entities.
            {
                auto e1 = context.CreateEntity<>();
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(e1.GetEntityId(), EntityId(0));

                auto e2 = context.CreateEntity<>();
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(e2.GetEntityId(), EntityId(1));

                EXPECT_EQ(e1.GetComponent<TestTranslation>(), nullptr);
                EXPECT_EQ(e1.GetComponent<TestPhysics>(), nullptr);
                EXPECT_EQ(e1.GetComponent<TestCharacter>(), nullptr);
                EXPECT_EQ(e2.GetComponent<TestPhysics>(), nullptr);
                EXPECT_EQ(e2.GetComponent<TestTranslation>(), nullptr);
                EXPECT_EQ(e2.GetComponent<TestCharacter>(), nullptr);

                testPhysicsSystem1.TestLoopEntities();
                testPhysicsSystem2.TestLoopEntities();
                testPlayerSystem.TestLoopEntities();
                EXPECT_EQ(testPhysicsSystem1.loopedEntities, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.loopedEntities, size_t(0));
                EXPECT_EQ(testPlayerSystem.loopedEntities, size_t(0));

                context.DestroyEntity(e1);
                context.DestroyEntity(e2);

                EXPECT_EQ(testPhysicsSystem1.onDestroyedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onDestroyedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onDestroyedEntityCount, size_t(0));
            }

            testPhysicsSystem1.ResetStats();
            testPhysicsSystem2.ResetStats();
            testPlayerSystem.ResetStats();

            // Create/destroy entities without any system monitoring them.
            {
                auto e1 = context.CreateEntity<TestPhysics>();
                
                EXPECT_TRUE((std::is_same<decltype(e1), TestEntity>::value));
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(e1.GetEntityId(), EntityId(0));

                auto e2 = context.CreateEntity<TestPhysics>();
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(e2.GetEntityId(), EntityId(1));

                auto e3 = context.CreateEntity<TestTranslation>();
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(e3.GetEntityId(), EntityId(2));


                ASSERT_NE(e1.GetComponent<TestPhysics>(), nullptr);
                ASSERT_NE(e2.GetComponent<TestPhysics>(), nullptr);
                ASSERT_NE(e3.GetComponent<TestTranslation>(), nullptr);
                e1.GetComponent<TestPhysics>()->velocity = { 1, 2, 3 };
                e1.GetComponent<TestPhysics>()->weight = 4;
                e2.GetComponent<TestPhysics>()->velocity = { 5, 6, 7 };
                e2.GetComponent<TestPhysics>()->weight = 8;
                e3.GetComponent<TestTranslation>()->position = { 9, 10, 11 };
                e3.GetComponent<TestTranslation>()->scale = { 12, 13, 14 };

                EXPECT_EQ(e1.GetComponent<TestPhysics>()->velocity, Vector3i32(1, 2, 3));
                EXPECT_EQ(e1.GetComponent<TestPhysics>()->weight, int32_t(4));
                EXPECT_EQ(e2.GetComponent<TestPhysics>()->velocity, Vector3i32(5, 6, 7));
                EXPECT_EQ(e2.GetComponent<TestPhysics>()->weight, int32_t(8));
                EXPECT_EQ(e3.GetComponent<TestTranslation>()->position, Vector3i32(9, 10, 11));
                EXPECT_EQ(e3.GetComponent<TestTranslation>()->scale, Vector3i32(12, 13, 14));

                testPhysicsSystem1.TestLoopEntities();
                testPhysicsSystem2.TestLoopEntities();
                testPlayerSystem.TestLoopEntities();
                EXPECT_EQ(testPhysicsSystem1.loopedEntities, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.loopedEntities, size_t(0));
                EXPECT_EQ(testPlayerSystem.loopedEntities, size_t(0));

                context.DestroyEntity(e1);
                context.DestroyEntity(e2);
                context.DestroyEntity(e3);

                EXPECT_EQ(testPhysicsSystem1.onDestroyedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onDestroyedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onDestroyedEntityCount, size_t(0));
            }

            testPhysicsSystem1.ResetStats();
            testPhysicsSystem2.ResetStats();
            testPlayerSystem.ResetStats();

            // Create/destroy entities(adding components at entity creation) with systems monitoring them.
            {
                auto e1 = context.CreateEntity<TestTranslation, TestPhysics>();
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(1));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(1));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(e1.GetEntityId(), EntityId(0));

                auto e2 = context.CreateEntity<TestTranslation, TestPhysics, TestCharacter>();
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(2));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(2));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(1));
                EXPECT_EQ(e2.GetEntityId(), EntityId(1));

                auto e3 = context.CreateEntity<TestTranslation>();
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(2));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(2));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(1));
                EXPECT_EQ(e3.GetEntityId(), EntityId(2));

                auto e4 = context.CreateEntity<TestPhysics>();
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(2));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(2));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(1));
                EXPECT_EQ(e4.GetEntityId(), EntityId(3));

                auto e5 = context.CreateEntity<TestCharacter>();
                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(2));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(2));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(1));
                EXPECT_EQ(e5.GetEntityId(), EntityId(4));

                testPhysicsSystem1.TestLoopEntities();
                testPhysicsSystem2.TestLoopEntities();
                testPlayerSystem.TestLoopEntities();
                EXPECT_EQ(testPhysicsSystem1.loopedEntities, size_t(2));
                EXPECT_EQ(testPhysicsSystem2.loopedEntities, size_t(2));
                EXPECT_EQ(testPlayerSystem.loopedEntities, size_t(1));

                ASSERT_NE(e1.GetComponent<TestTranslation>(), nullptr);
                ASSERT_NE(e1.GetComponent<TestPhysics>(), nullptr);
                EXPECT_EQ(e1.GetComponent<TestCharacter>(), nullptr);
                ASSERT_NE(e2.GetComponent<TestPhysics>(), nullptr);
                ASSERT_NE(e2.GetComponent<TestTranslation>(), nullptr);
                ASSERT_NE(e2.GetComponent<TestCharacter>(), nullptr);
                EXPECT_EQ(e3.GetComponent<TestPhysics>(), nullptr);
                ASSERT_NE(e3.GetComponent<TestTranslation>(), nullptr);
                EXPECT_EQ(e3.GetComponent<TestCharacter>(), nullptr);
                ASSERT_NE(e4.GetComponent<TestPhysics>(), nullptr);
                EXPECT_EQ(e4.GetComponent<TestTranslation>(), nullptr);
                EXPECT_EQ(e4.GetComponent<TestCharacter>(), nullptr);
                EXPECT_EQ(e5.GetComponent<TestPhysics>(), nullptr);
                EXPECT_EQ(e5.GetComponent<TestTranslation>(), nullptr);
                ASSERT_NE(e5.GetComponent<TestCharacter>(), nullptr);

                // Add data
                e1.GetComponent<TestTranslation>()->position = { 1, 2, 3 };
                e1.GetComponent<TestTranslation>()->scale = { 4, 5, 6 };
                e1.GetComponent<TestPhysics>()->velocity = { 7, 8, 9 };
                e1.GetComponent<TestPhysics>()->weight = 10;
                e2.GetComponent<TestTranslation>()->position = { 11, 12, 13 };
                e2.GetComponent<TestTranslation>()->scale = { 14, 15, 16 };
                e2.GetComponent<TestPhysics>()->velocity = { 17, 18, 19 };
                e2.GetComponent<TestPhysics>()->weight = 20;

                const char name[] = "Jimmie Bergmann";
                std::memset(e2.GetComponent<TestCharacter>()->name, 0, 50);
                std::memcpy(e2.GetComponent<TestCharacter>()->name, name, strlen(name));

                // Check data.
                std::vector<TestPhysicsSystem::Data> physSystemData(2);
                physSystemData[0].translation.position = { 1, 2, 3 };
                physSystemData[0].translation.scale = { 4, 5, 6 };
                physSystemData[0].physics.velocity = { 7, 8, 9 };
                physSystemData[0].physics.weight = 10;
                physSystemData[1].translation.position = { 11, 12, 13 };
                physSystemData[1].translation.scale = { 14, 15, 16 };
                physSystemData[1].physics.velocity = { 17, 18, 19 };
                physSystemData[1].physics.weight = 20;

                std::vector<TestPlayerSystem::Data> playerSystemData(1);
                playerSystemData[0].translation.position = { 11, 12, 13 };
                playerSystemData[0].translation.scale = { 14, 15, 16 };
                playerSystemData[0].physics.velocity = { 17, 18, 19 };
                playerSystemData[0].physics.weight = 20;
                std::memcpy(playerSystemData[0].character.name, "Jimmie Bergmann", 16);

                testPhysicsSystem1.TestCheckEntities(physSystemData);
                EXPECT_EQ(testPhysicsSystem1.loopedEntities, size_t(2));
                testPhysicsSystem2.TestCheckEntities(physSystemData);
                EXPECT_EQ(testPhysicsSystem2.loopedEntities, size_t(2));
                testPlayerSystem.TestCheckEntities(playerSystemData);
                EXPECT_EQ(testPlayerSystem.loopedEntities, size_t(1));

                context.DestroyEntity(e1);
                context.DestroyEntity(e2);
                context.DestroyEntity(e3);
                context.DestroyEntity(e4);
                context.DestroyEntity(e5);

                EXPECT_EQ(testPhysicsSystem1.onDestroyedEntityCount, size_t(2));
                EXPECT_EQ(testPhysicsSystem2.onDestroyedEntityCount, size_t(2));
                EXPECT_EQ(testPlayerSystem.onDestroyedEntityCount, size_t(1));
            }

            testPhysicsSystem1.ResetStats();
            testPhysicsSystem2.ResetStats();
            testPlayerSystem.ResetStats();
        }

        TEST(ECS, AddComponents)
        {
            TestContext context;

            TestPhysicsSystem testPhysicsSystem1;
            TestPhysicsSystem testPhysicsSystem2;
            TestPlayerSystem testPlayerSystem;

            context.RegisterSystem(testPhysicsSystem1);
            context.RegisterSystem(testPhysicsSystem2);
            context.RegisterSystem(testPlayerSystem);

            {
                auto e1 = context.CreateEntity();
                e1.AddComponents();
                EXPECT_EQ(e1.GetEntityId(), EntityId(0));
                context.DestroyEntity(e1);
            }
            {
                auto e1 = context.CreateEntity();
                e1.AddComponents();
                EXPECT_EQ(e1.GetEntityId(), EntityId(0));

                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(0));
                
                // Add trans component.
                e1.AddComponents<TestTranslation>();
                auto* trans = e1.GetComponent<TestTranslation>();
                trans->position = { 1, 2, 3 };
                trans->scale = { 4, 5, 6 };

                trans = e1.GetComponent<TestTranslation>();
                EXPECT_EQ(trans->position, Vector3i32(1, 2, 3));
                EXPECT_EQ(trans->scale, Vector3i32(4, 5, 6));

                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(0));
                
                // Add character component.
                e1.AddComponents<TestCharacter>();
                auto* chara = e1.GetComponent<TestCharacter>();
                std::memset(chara->name, 0, 50);
                std::memcpy(chara->name, "Jimmie Bergmann", 16);

                trans = e1.GetComponent<TestTranslation>();
                chara = e1.GetComponent<TestCharacter>();
                EXPECT_EQ(trans->position, Vector3i32(1, 2, 3));
                EXPECT_EQ(trans->scale, Vector3i32(4, 5, 6));
                EXPECT_STREQ(chara->name, "Jimmie Bergmann");

                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(0));

                // Add physics component.
                e1.AddComponents<TestPhysics>();
                auto* phys = e1.GetComponent<TestPhysics>();
                phys->velocity = { 7, 8, 9 };
                phys->weight = 10;

                phys = e1.GetComponent<TestPhysics>();
                trans = e1.GetComponent<TestTranslation>();
                chara = e1.GetComponent<TestCharacter>();

                EXPECT_EQ(trans->position, Vector3i32(1, 2, 3));
                EXPECT_EQ(trans->scale, Vector3i32(4, 5, 6));
                EXPECT_EQ(phys->velocity, Vector3i32(7, 8, 9));
                EXPECT_EQ(phys->weight, int32_t(10));
                EXPECT_STREQ(chara->name, "Jimmie Bergmann");

                EXPECT_EQ(testPhysicsSystem1.onCreatedEntityCount, size_t(1));
                EXPECT_EQ(testPhysicsSystem2.onCreatedEntityCount, size_t(1));
                EXPECT_EQ(testPlayerSystem.onCreatedEntityCount, size_t(1));

                context.DestroyEntity(e1);
            }
        }

        TEST(ECS, LowBlockSize)
        {
            {
                const size_t blockSize = sizeof(TestTranslation) + sizeof(TestPhysics) - 1;
                ContextDescriptor desc(blockSize);
                TestContext context(desc);

                TestPhysicsSystem testPhysicsSystem;
                context.RegisterSystem(testPhysicsSystem);

                EXPECT_NO_THROW(context.CreateEntity<TestTranslation>());
                EXPECT_NO_THROW(context.CreateEntity<TestPhysics>());
                EXPECT_THROW((context.CreateEntity<TestTranslation, TestPhysics>()), Curse::Exception);
                EXPECT_NO_THROW(context.CreateEntity<TestTranslation>());
                EXPECT_NO_THROW(context.CreateEntity<TestPhysics>());
                EXPECT_NO_THROW(context.CreateEntity<TestTranslation>());
                EXPECT_NO_THROW(context.CreateEntity<TestPhysics>());

                auto e = context.CreateEntity<TestTranslation>();
                EXPECT_EQ(e.GetEntityId(), EntityId(6));

            }
        }

        TEST(ECS, RemoveAllComponents)
        {
            TestContext context;

            TestPhysicsSystem testPhysicsSystem1;
            TestPhysicsSystem testPhysicsSystem2;
            TestPlayerSystem testPlayerSystem;

            context.RegisterSystem(testPhysicsSystem1);
            context.RegisterSystem(testPhysicsSystem2);
            context.RegisterSystem(testPlayerSystem);

            {
                auto e1 = context.CreateEntity();
                EXPECT_NO_THROW((e1.RemoveAllComponents()));
            }
            {
                auto e1 = context.CreateEntity<TestTranslation>();

                EXPECT_NO_THROW((e1.RemoveAllComponents()));
                EXPECT_EQ(e1.GetComponent<TestTranslation>(), nullptr);

                EXPECT_NO_THROW((e1.RemoveAllComponents()));
                EXPECT_EQ(e1.GetComponent<TestTranslation>(), nullptr);

                e1.AddComponents<TestTranslation>();
                EXPECT_NE(e1.GetComponent<TestTranslation>(), nullptr);

                EXPECT_NO_THROW((e1.RemoveAllComponents()));
                EXPECT_EQ(e1.GetComponent<TestTranslation>(), nullptr);

            }
        }
        TEST(ECS, RemoveComponents)
        {
            TestContext context;

            TestPhysicsSystem testPhysicsSystem1;
            TestPhysicsSystem testPhysicsSystem2;
            TestPlayerSystem testPlayerSystem;

            context.RegisterSystem(testPhysicsSystem1);
            context.RegisterSystem(testPhysicsSystem2);
            context.RegisterSystem(testPlayerSystem);

            {
                auto e1 = context.CreateEntity();
                EXPECT_NO_THROW((e1.RemoveComponents()));
                EXPECT_NO_THROW((e1.RemoveComponents<TestTranslation>()));
                EXPECT_NO_THROW((e1.RemoveComponents<TestPhysics>()));
                EXPECT_NO_THROW((e1.RemoveComponents<TestTranslation, TestPhysics>()));
            }
            {
                auto e1 = context.CreateEntity<TestTranslation>();

                EXPECT_NO_THROW((e1.RemoveComponents()));
                EXPECT_NE(e1.GetComponent<TestTranslation>(), nullptr);

                EXPECT_NO_THROW((e1.RemoveComponents<TestTranslation>()));
                EXPECT_EQ(e1.GetComponent<TestTranslation>(), nullptr);
            }
            {
                auto e1 = context.CreateEntity<TestTranslation, TestPhysics>();
                auto* trans = e1.GetComponent<TestTranslation>();
                auto* phys = e1.GetComponent<TestPhysics>();
                trans->position = { 1, 2, 3 };
                trans->scale = { 4, 5, 6 };
                phys->velocity = { 7, 8, 9 };
                phys->weight = 10;

                EXPECT_NO_THROW((e1.RemoveComponents()));
                EXPECT_NE(e1.GetComponent<TestTranslation>(), nullptr);
                EXPECT_NE(e1.GetComponent<TestPhysics>(), nullptr);

                EXPECT_NO_THROW((e1.RemoveComponents<TestTranslation>()));
                EXPECT_EQ(e1.GetComponent<TestTranslation>(), nullptr);
                ASSERT_NE(e1.GetComponent<TestPhysics>(), nullptr);
                phys = e1.GetComponent<TestPhysics>();
                EXPECT_EQ(phys->velocity, Vector3i32(7, 8, 9));
                EXPECT_EQ(phys->weight, int32_t(10));

                EXPECT_NO_THROW((e1.RemoveComponents<TestPhysics>()));
                EXPECT_EQ(e1.GetComponent<TestTranslation>(), nullptr);
                EXPECT_EQ(e1.GetComponent<TestPhysics>(), nullptr);
            }
            {
                testPhysicsSystem1.ResetStats();
                testPhysicsSystem2.ResetStats();
                testPlayerSystem.ResetStats();

                EXPECT_EQ(testPhysicsSystem1.onDestroyedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onDestroyedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onDestroyedEntityCount, size_t(0));

                auto e1 = context.CreateEntity<TestTranslation, TestPhysics, TestCharacter>();
                auto e2 = context.CreateEntity<TestTranslation, TestPhysics, TestCharacter>();
                auto e3 = context.CreateEntity<TestTranslation, TestPhysics, TestCharacter>();

                EXPECT_NO_THROW((e2.RemoveComponents()));
                EXPECT_NE(e2.GetComponent<TestTranslation>(), nullptr);
                EXPECT_NE(e2.GetComponent<TestPhysics>(), nullptr);
                EXPECT_NE(e2.GetComponent<TestCharacter>(), nullptr);

                EXPECT_EQ(testPhysicsSystem1.onDestroyedEntityCount, size_t(0));
                EXPECT_EQ(testPhysicsSystem2.onDestroyedEntityCount, size_t(0));
                EXPECT_EQ(testPlayerSystem.onDestroyedEntityCount, size_t(0));

                EXPECT_NO_THROW((e2.RemoveComponents<TestPhysics>()));
                EXPECT_NE(e2.GetComponent<TestTranslation>(), nullptr);
                EXPECT_EQ(e2.GetComponent<TestPhysics>(), nullptr);
                EXPECT_NE(e2.GetComponent<TestCharacter>(), nullptr);

                EXPECT_EQ(testPhysicsSystem1.onDestroyedEntityCount, size_t(1));
                EXPECT_EQ(testPhysicsSystem2.onDestroyedEntityCount, size_t(1));
                EXPECT_EQ(testPlayerSystem.onDestroyedEntityCount, size_t(1));

                EXPECT_NO_THROW((e2.RemoveComponents<TestCharacter>()));
                EXPECT_NE(e2.GetComponent<TestTranslation>(), nullptr);
                EXPECT_EQ(e2.GetComponent<TestPhysics>(), nullptr);
                EXPECT_EQ(e2.GetComponent<TestCharacter>(), nullptr);

                EXPECT_EQ(testPhysicsSystem1.onDestroyedEntityCount, size_t(1));
                EXPECT_EQ(testPhysicsSystem2.onDestroyedEntityCount, size_t(1));
                EXPECT_EQ(testPlayerSystem.onDestroyedEntityCount, size_t(1));

                EXPECT_NO_THROW((e2.RemoveComponents<TestTranslation>()));
                EXPECT_EQ(e2.GetComponent<TestTranslation>(), nullptr);
                EXPECT_EQ(e2.GetComponent<TestPhysics>(), nullptr);
                EXPECT_EQ(e2.GetComponent<TestCharacter>(), nullptr);

                EXPECT_EQ(testPhysicsSystem1.onDestroyedEntityCount, size_t(1));
                EXPECT_EQ(testPhysicsSystem2.onDestroyedEntityCount, size_t(1));
                EXPECT_EQ(testPlayerSystem.onDestroyedEntityCount, size_t(1));
            }
        }


        CURSE_ECS_COMPONENT(TestIndex, TestContext)
        {
            TestIndex() :
                index(-1)
            { }

            int32_t index;
        };

        CURSE_ECS_SYSTEM(TestManyEntitySystem, TestContext, TestTranslation, TestPhysics, TestIndex)
        {

            struct Data
            {
                TestTranslation translation;
                TestPhysics physics;
            };

            void Process(const Time & deltaTime) override
            {
                // Dummy imp. Check the Test* methods.
            }


            void TestCheckEntities(const std::vector<Data> & data)
            {
                ASSERT_EQ(data.size(), GetEntityCount());

                for (size_t i = 0; i < GetEntityCount(); i++)
                {
                    auto& index = GetComponent<TestIndex>(i);
                    auto& trans = GetComponent<TestTranslation>(i);
                    auto& phys = GetComponent<TestPhysics>(i);

                    ASSERT_GE(index.index, int32_t(0));
                    ASSERT_LT(index.index, int32_t(GetEntityCount()));

                    size_t currIndex = static_cast<size_t>(index.index);

                    if (trans.position != (data[currIndex].translation.position))
                    {
                        EXPECT_TRUE(false);
                    }

                    EXPECT_EQ(trans.position, (data[currIndex].translation.position));
                    EXPECT_EQ(trans.scale, (data[currIndex].translation.scale));
                    EXPECT_EQ(phys.velocity, (data[currIndex].physics.velocity));
                    EXPECT_EQ(phys.weight, (data[currIndex].physics.weight));
                }
            }

        };

        TEST(ECS, Many_CreateEntity)
        {
            EXPECT_EQ(TestTranslation::componentTypeId, ComponentTypeId(0));
            EXPECT_EQ(TestPhysics::componentTypeId, ComponentTypeId(1));
            EXPECT_EQ(TestCharacter::componentTypeId, ComponentTypeId(2));
            EXPECT_EQ(TestIndex::componentTypeId, ComponentTypeId(3));

            const size_t loopCount = 500;

            ContextDescriptor desc(300);
            TestContext context(desc);

            TestManyEntitySystem manyEntitiesSystem;
            context.RegisterSystem(manyEntitiesSystem);

            int32_t val = 0;
            for (size_t i = 0; i < loopCount; i++)
            {
                context.CreateEntity();
                context.CreateEntity<TestTranslation>();
                context.CreateEntity<TestPhysics, TestIndex>();
                auto e = context.CreateEntity<TestPhysics, TestTranslation, TestIndex>();
                
                auto index = e.GetComponent<TestIndex>();
                auto phys = e.GetComponent<TestPhysics>();
                auto trans = e.GetComponent<TestTranslation>();

                index->index = static_cast<int32_t>(i);
                phys->velocity = { val, val + 1 , val + 2 };
                phys->weight = val + 3;
                trans->position = { val + 4, val + 5, val + 6 };
                trans->scale = { val + 7, val + 8, val + 9 };

                val += 10;
            }

            val = 0;
            std::vector<TestManyEntitySystem::Data> data(loopCount);
            for (size_t i = 0; i < loopCount; i++)
            {
                data[i].physics.velocity = { val, val + 1, val + 2 };
                data[i].physics.weight = val + 3;
                data[i].translation.position = { val + 4, val + 5, val + 6 };
                data[i].translation.scale = { val + 7, val + 8, val + 9 };

                val += 10;
            }

            manyEntitiesSystem.TestCheckEntities(data);
        }

        TEST(ECS, Many_AddComponent)
        {
            EXPECT_EQ(TestTranslation::componentTypeId, ComponentTypeId(0));
            EXPECT_EQ(TestPhysics::componentTypeId, ComponentTypeId(1));
            EXPECT_EQ(TestCharacter::componentTypeId, ComponentTypeId(2));
            EXPECT_EQ(TestIndex::componentTypeId, ComponentTypeId(3));

            const size_t loopCount = 500;

            ContextDescriptor desc(300);
            TestContext context(desc);

            TestManyEntitySystem manyEntitiesSystem;
            TestPlayerSystem playerSystem;
            context.RegisterSystem(manyEntitiesSystem);
            context.RegisterSystem(playerSystem);

            int32_t val = 0;
            int32_t currentIndex = 0;
            for (size_t i = 0; i < loopCount; i++)
            {
                context.CreateEntity();
                auto e1 = context.CreateEntity();
                auto e2 = context.CreateEntity();
                auto e3 = context.CreateEntity();
                auto e4 = context.CreateEntity();

                e1.AddComponents<TestTranslation>();
                e2.AddComponents<TestPhysics, TestIndex>();
                e3.AddComponents<TestPhysics, TestTranslation, TestIndex>();
                e3.AddComponents<TestCharacter>();

                {
                    auto index = e3.GetComponent<TestIndex>();
                    auto phys = e3.GetComponent<TestPhysics>();
                    auto trans = e3.GetComponent<TestTranslation>();

                    index->index = currentIndex++;
                    phys->velocity = { val, val + 1 , val + 2 };
                    phys->weight = val + 3;
                    trans->position = { val + 4, val + 5, val + 6 };
                    trans->scale = { val + 7, val + 8, val + 9 };

                    val += 10;
                }
                {
                    e4.AddComponents<TestPhysics>();
                    auto phys = e4.GetComponent<TestPhysics>();
                    phys->velocity = { val, val + 1 , val + 2 };
                    phys->weight = val + 3;

                    e4.AddComponents<TestIndex>();
                    auto index = e4.GetComponent<TestIndex>();
                    index->index = currentIndex++;
                    
                    e4.AddComponents<TestTranslation>();
                    auto trans = e4.GetComponent<TestTranslation>();
                    trans->position = { val + 4, val + 5, val + 6 };
                    trans->scale = { val + 7, val + 8, val + 9 };

                    val += 10;
                }
            }

            val = 0;

            const size_t doubleLoopCount = loopCount * 2;
            std::vector<TestManyEntitySystem::Data> data(doubleLoopCount);
            for (size_t i = 0; i < doubleLoopCount; i++)
            {
                data[i].physics.velocity = { val, val + 1, val + 2 };
                data[i].physics.weight = val + 3;
                data[i].translation.position = { val + 4, val + 5, val + 6 };
                data[i].translation.scale = { val + 7, val + 8, val + 9 };
                val += 10;
            }

            manyEntitiesSystem.TestCheckEntities(data);

        }

        TEST(ECS, DuplicateComponent)
        { 
            TestContext context;

            {
                g_testTranslationConstructorCalls = 0;
                g_testPhysicsConstructorCalls = 0;
                g_testCharacterConstructorCalls = 0;

                auto e1 = context.CreateEntity<TestTranslation, TestPhysics, TestCharacter, TestTranslation, TestPhysics, TestCharacter>();

                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(1));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(1));

                e1.AddComponents<TestTranslation, TestPhysics, TestCharacter>();

                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(1));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(1));
            }
            {
                g_testTranslationConstructorCalls = 0;
                g_testPhysicsConstructorCalls = 0;
                g_testCharacterConstructorCalls = 0;

                auto e1 = context.CreateEntity<TestPhysics, TestTranslation, TestTranslation, TestPhysics, TestCharacter, TestCharacter>();

                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(1));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(1));

                e1.AddComponents<TestTranslation, TestPhysics, TestCharacter>();

                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(1));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(1));
            }
            {
                g_testTranslationConstructorCalls = 0;
                g_testPhysicsConstructorCalls = 0;
                g_testCharacterConstructorCalls = 0;

                auto e1 = context.CreateEntity();

                e1.AddComponents<TestTranslation, TestTranslation>();
                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(0));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(0));

                e1.AddComponents<TestTranslation, TestPhysics, TestPhysics>();
                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(1));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(0));

                e1.AddComponents<TestTranslation, TestPhysics, TestCharacter, TestTranslation, TestPhysics, TestPhysics>();
                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(1));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(1));
            }
            {
                g_testTranslationConstructorCalls = 0;
                g_testPhysicsConstructorCalls = 0;
                g_testCharacterConstructorCalls = 0;

                auto e1 = context.CreateEntity();

                e1.AddComponents<TestTranslation>();
                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(0));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(0));

                e1.AddComponents<TestTranslation, TestPhysics>();
                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(1));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(0));

                e1.AddComponents<TestTranslation, TestPhysics, TestCharacter>();
                EXPECT_EQ(g_testTranslationConstructorCalls, size_t(1));
                EXPECT_EQ(g_testPhysicsConstructorCalls, size_t(1));
                EXPECT_EQ(g_testCharacterConstructorCalls, size_t(1));
            }
           
        }

    }

}