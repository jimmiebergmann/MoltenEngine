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

#include "Test.hpp"
#include "Molten/System/ThreadPool.hpp"
#include "Molten/FileFormat/Mesh/ObjMeshFile.hpp"

namespace Molten
{
    TEST(FileFormat, ObjMeshFile_OpenFileError)
    {
        ThreadPool threadPool;

        ObjMeshFile objFile;
        const auto result = objFile.ReadFromFile("../Engine/Test/Data/ObjMesh/ThisFileDoesNotExist.obj", threadPool);

        EXPECT_FALSE(result.IsSuccessful());
        EXPECT_EQ(result.GetError().code, TextFileFormatResult::OpenFileError);
        EXPECT_EQ(result.GetError().lineNumber, size_t{ 0 });
    }

    TEST(FileFormat, ObjMeshFile)
    {
        ThreadPool threadPool(1);
        ObjMeshFile objFile;
        TextFileFormatResult result;

        EXPECT_NO_THROW(result = objFile.ReadFromFile("../Engine/Test/Data/ObjMesh/TestCubes.obj", threadPool));
        ASSERT_TRUE(result.IsSuccessful());

        ASSERT_EQ(objFile.objects.size(), size_t{ 3 });
        {
            auto& object = objFile.objects[0];
            ASSERT_NE(object, nullptr);

            EXPECT_STREQ(object->name.c_str(), "Cube.001");

            ASSERT_EQ(object->vertices.size(), size_t{ 8 });
            EXPECT_VECTOR3_NEAR(object->vertices[0], Vector3f32(1.0f, 1.0f, 0.5f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->vertices[1], Vector3f32(1.0f, -1.0f, 0.5f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->vertices[2], Vector3f32(1.0f, 1.0f, 2.5f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->vertices[3], Vector3f32(1.0f, -1.0f, 2.5f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->vertices[4], Vector3f32(-1.0f, 1.0f, 0.5f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->vertices[5], Vector3f32(-1.0f, -1.0f, 0.5f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->vertices[6], Vector3f32(-1.0f, 1.0f, 2.5f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->vertices[7], Vector3f32(-1.0f, -1.0f, 2.5f), 1e-4);

            ASSERT_EQ(object->textureCoordinates.size(), size_t{ 14 });
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[0], Vector2f32(0.625f, 0.5f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[1], Vector2f32(0.875f, 0.5f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[2], Vector2f32(0.875f, 0.75f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[3], Vector2f32(0.625f, 0.75f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[4], Vector2f32(0.375f, 0.75f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[5], Vector2f32(0.625f, 1.0f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[6], Vector2f32(0.375f, 1.0f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[7], Vector2f32(0.375f, 0.0f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[8], Vector2f32(0.625f, 0.0f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[9], Vector2f32(0.625f, 0.25f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[10], Vector2f32(0.375f, 0.25f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[11], Vector2f32(0.125f, 0.5f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[12], Vector2f32(0.375f, 0.5f), 1e-4);
            EXPECT_VECTOR2_NEAR(object->textureCoordinates[13], Vector2f32(0.125f, 0.75f), 1e-4);

            ASSERT_EQ(object->normals.size(), size_t{ 9 });
            EXPECT_VECTOR3_NEAR(object->normals[0], Vector3f32(0.0f, 1.0f, 0.0f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->normals[1], Vector3f32(0.5773f, -0.5773f, 0.5773f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->normals[2], Vector3f32(0.5773f, 0.5773f, 0.5773f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->normals[3], Vector3f32(-0.5773f, 0.5773f, 0.5773f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->normals[4], Vector3f32(-0.5773f, -0.5773f, 0.5773f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->normals[5], Vector3f32(-0.5773f, 0.5773f, -0.5773f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->normals[6], Vector3f32(-0.5773f, -0.5773f, -0.5773f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->normals[7], Vector3f32(0.5773f, -0.5773f, -0.5773f), 1e-4);
            EXPECT_VECTOR3_NEAR(object->normals[8], Vector3f32(0.5773f, 0.5773f, -0.5773f), 1e-4);


            ASSERT_EQ(object->groups.size(), size_t{ 1 });
            auto& group = object->groups[0];
            ASSERT_NE(group, nullptr);
            EXPECT_STREQ(group->name.c_str(), "");
            EXPECT_STREQ(group->material.c_str(), "Material.001");

            ASSERT_EQ(group->smoothingGroups.size(), size_t{ 2 });
            {
                auto& smoothGroup = group->smoothingGroups[0];
                ASSERT_NE(smoothGroup, nullptr);
                EXPECT_EQ(smoothGroup->id, uint32_t{ 0 });

                ASSERT_EQ(smoothGroup->triangles.size(), size_t{ 2 });
                {
                    auto& triangle = smoothGroup->triangles[0];

                    EXPECT_EQ(triangle.vertexIndices[0], uint32_t{ 1 });
                    EXPECT_EQ(triangle.vertexIndices[1], uint32_t{ 5 });
                    EXPECT_EQ(triangle.vertexIndices[2], uint32_t{ 7 });

                    EXPECT_EQ(triangle.textureCoordinateIndices[0], uint32_t{ 1 });
                    EXPECT_EQ(triangle.textureCoordinateIndices[1], uint32_t{ 2 });
                    EXPECT_EQ(triangle.textureCoordinateIndices[2], uint32_t{ 3 });

                    EXPECT_EQ(triangle.normalIndices[0], uint32_t{ 1 });
                    EXPECT_EQ(triangle.normalIndices[1], uint32_t{ 1 });
                    EXPECT_EQ(triangle.normalIndices[2], uint32_t{ 1 });
                }
                {
                    auto& triangle = smoothGroup->triangles[1];

                    EXPECT_EQ(triangle.vertexIndices[0], uint32_t{ 3 });
                    EXPECT_EQ(triangle.vertexIndices[1], uint32_t{ 1 });
                    EXPECT_EQ(triangle.vertexIndices[2], uint32_t{ 7 });

                    EXPECT_EQ(triangle.textureCoordinateIndices[0], uint32_t{ 4 });
                    EXPECT_EQ(triangle.textureCoordinateIndices[1], uint32_t{ 1 });
                    EXPECT_EQ(triangle.textureCoordinateIndices[2], uint32_t{ 3 });

                    EXPECT_EQ(triangle.normalIndices[0], uint32_t{ 1 });
                    EXPECT_EQ(triangle.normalIndices[1], uint32_t{ 1 });
                    EXPECT_EQ(triangle.normalIndices[2], uint32_t{ 1 });
                }
            }
            {
                auto& smoothGroup = group->smoothingGroups[1];
                ASSERT_NE(smoothGroup, nullptr);
                EXPECT_EQ(smoothGroup->id, uint32_t{ 1 });

                ASSERT_EQ(smoothGroup->triangles.size(), size_t{ 10 });
                {
                    auto& triangle = smoothGroup->triangles[0];

                    EXPECT_EQ(triangle.vertexIndices[0], uint32_t{ 4 });
                    EXPECT_EQ(triangle.vertexIndices[1], uint32_t{ 3 });
                    EXPECT_EQ(triangle.vertexIndices[2], uint32_t{ 7 });

                    EXPECT_EQ(triangle.textureCoordinateIndices[0], uint32_t{ 5 });
                    EXPECT_EQ(triangle.textureCoordinateIndices[1], uint32_t{ 4 });
                    EXPECT_EQ(triangle.textureCoordinateIndices[2], uint32_t{ 6 });

                    EXPECT_EQ(triangle.normalIndices[0], uint32_t{ 2 });
                    EXPECT_EQ(triangle.normalIndices[1], uint32_t{ 3 });
                    EXPECT_EQ(triangle.normalIndices[2], uint32_t{ 4 });
                }
                {
                    auto& triangle = smoothGroup->triangles[1];

                    EXPECT_EQ(triangle.vertexIndices[0], uint32_t{ 8 });
                    EXPECT_EQ(triangle.vertexIndices[1], uint32_t{ 4 });
                    EXPECT_EQ(triangle.vertexIndices[2], uint32_t{ 7 });

                    EXPECT_EQ(triangle.textureCoordinateIndices[0], uint32_t{ 7 });
                    EXPECT_EQ(triangle.textureCoordinateIndices[1], uint32_t{ 5 });
                    EXPECT_EQ(triangle.textureCoordinateIndices[2], uint32_t{ 6 });

                    EXPECT_EQ(triangle.normalIndices[0], uint32_t{ 5 });
                    EXPECT_EQ(triangle.normalIndices[1], uint32_t{ 2 });
                    EXPECT_EQ(triangle.normalIndices[2], uint32_t{ 4 });
                }

                // Skipping test for the rest...
            }        
        }
        {
            auto& object = objFile.objects[1];
            ASSERT_NE(object, nullptr);

            EXPECT_STREQ(object->name.c_str(), "Cube.002");

            EXPECT_EQ(object->vertices.size(), size_t{ 8 });
            EXPECT_EQ(object->textureCoordinates.size(), size_t{ 14 });
            EXPECT_EQ(object->normals.size(), size_t{ 6 });

            ASSERT_EQ(object->groups.size(), size_t{ 1 });
            {
                auto& group = object->groups[0];
                ASSERT_NE(group, nullptr);
                EXPECT_STREQ(group->name.c_str(), "");
                EXPECT_STREQ(group->material.c_str(), "Material.002");

                ASSERT_EQ(group->smoothingGroups.size(), size_t{ 1 });
                auto& smoothGroup = group->smoothingGroups[0];
                ASSERT_NE(smoothGroup, nullptr);
                EXPECT_EQ(smoothGroup->id, uint32_t{ 0 });
                EXPECT_EQ(smoothGroup->triangles.size(), size_t{ 12 });

                // Skipping test for the rest...
            }           
        }
        {
            auto& object = objFile.objects[2];
            ASSERT_NE(object, nullptr);

            EXPECT_STREQ(object->name.c_str(), "Cube.003");

            EXPECT_EQ(object->vertices.size(), size_t{ 8 });
            EXPECT_EQ(object->textureCoordinates.size(), size_t{ 14 });
            EXPECT_EQ(object->normals.size(), size_t{ 10 });

            ASSERT_EQ(object->groups.size(), size_t{ 2 });
            {
                {
                    auto& group = object->groups[0];
                    ASSERT_NE(group, nullptr);
                    EXPECT_STREQ(group->name.c_str(), "");
                    EXPECT_STREQ(group->material.c_str(), "Material.002");

                    ASSERT_EQ(group->smoothingGroups.size(), size_t{ 1 });
                    auto& smoothGroup = group->smoothingGroups[0];
                    ASSERT_NE(smoothGroup, nullptr);
                    EXPECT_EQ(smoothGroup->id, uint32_t{ 0 });
                    EXPECT_EQ(smoothGroup->triangles.size(), size_t{ 2 });
                }
                {
                    auto& group = object->groups[1];
                    ASSERT_NE(group, nullptr);
                    EXPECT_STREQ(group->name.c_str(), "Test group");
                    EXPECT_STREQ(group->material.c_str(), "Material.002");

                    ASSERT_EQ(group->smoothingGroups.size(), size_t{ 2 });
                    {
                        auto& smoothGroup = group->smoothingGroups[0];
                        ASSERT_NE(smoothGroup, nullptr);
                        EXPECT_EQ(smoothGroup->id, uint32_t{ 0 });
                        EXPECT_EQ(smoothGroup->triangles.size(), size_t{ 2 });
                    }
                    {
                        auto& smoothGroup = group->smoothingGroups[1];
                        ASSERT_NE(smoothGroup, nullptr);
                        EXPECT_EQ(smoothGroup->id, uint32_t{ 1 });
                        EXPECT_EQ(smoothGroup->triangles.size(), size_t{ 8 });
                    }
                }
            }
        }
    }

   /* TEST(FileFormat, ObjMeshFile_Benchmark)
    {
        ThreadPool threadPool;
        ObjMeshFile objFile;

        Molten::Test::Benchmarker bm("Model");
        const auto result = objFile.ReadFromFile("C:/temp/Sponza/Sponza.obj", threadPool);
        EXPECT_TRUE(result.IsSuccessful());
    }*/

}