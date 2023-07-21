/*
* MIT License
*
* Copyright (c) 2023 Jimmie Bergmann
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
#include "Molten/EditorFramework/FileFormat/Mesh/ObjMeshFile.hpp"

namespace Molten::EditorFramework
{

    TEST(FileFormat, ObjMaterialFile_OpenFileError)
    {
        ObjMaterialFileReader reader;

        const auto result = reader.Read("../Engine/Test/Data/ObjMesh/ThisFileDoesNotExist.mtl");

        ASSERT_FALSE(result);
        EXPECT_EQ(result.Error().code, TextFileFormatErrorCode::OpenFileError);
        EXPECT_EQ(result.Error().line, size_t{ 0 });
        EXPECT_EQ(result.Error().column, size_t{ 0 });
    }

    TEST(FileFormat, ObjMaterialFile)
    {
        ObjMaterialFileReader reader;
        ObjMaterialFileReaderOptions options{ .useWarnings = true, .ignoreUnknownCommands = false, .ignoreDuplicateCommands = true };
         
        Molten::Test::Benchmarker bm("ObjMaterialFile_Benchmark");
        auto result = reader.Read("../Engine/Core/Test/Data/ObjMesh/TestCubes.mtl", options);
        
        EXPECT_EQ(result.Value().warnings.size(), size_t{ 0 });

        auto objMaterialFile = std::move(result.Value().file);
        ASSERT_EQ(objMaterialFile.materials.size(), size_t{ 2 });
        {
            auto& material = objMaterialFile.materials[0];
            ASSERT_NE(material, nullptr);

            EXPECT_STREQ(material->name.c_str(), "Material.001");

            ASSERT_TRUE(material->specularExponent.has_value());
            EXPECT_NEAR(material->specularExponent.value(), 16.5f, 1e-4);

            ASSERT_TRUE(material->ambientColor.has_value());
            EXPECT_VECTOR3_NEAR(material->ambientColor.value(), Vector3f32(1.0f, 1.0f, 1.0f), 1e-4);

            ASSERT_TRUE(material->diffuseColor.has_value());
            EXPECT_VECTOR3_NEAR(material->diffuseColor.value(), Vector3f32(0.8f, 0.8f, 0.8f), 1e-4);

            ASSERT_TRUE(material->specularColor.has_value());
            EXPECT_VECTOR3_NEAR(material->specularColor.value(), Vector3f32(0.5f, 0.5f, 0.5f), 1e-4);

            ASSERT_TRUE(material->emissiveColor.has_value());
            EXPECT_VECTOR3_NEAR(material->emissiveColor.value(), Vector3f32(0.0f, 0.0f, 0.0f), 1e-4);

            ASSERT_TRUE(material->opticalDensity.has_value());
            EXPECT_NEAR(material->opticalDensity.value(), 1.45f, 1e-4);

            ASSERT_TRUE(material->dissolve.has_value());
            EXPECT_NEAR(material->dissolve.value(), 1.0f, 1e-4);
        }
        {
            auto& material = objMaterialFile.materials[1];
            ASSERT_NE(material, nullptr);

            EXPECT_STREQ(material->name.c_str(), "Material.002");

            ASSERT_TRUE(material->specularExponent.has_value());
            EXPECT_NEAR(material->specularExponent.value(), 300.0f, 1e-4);

            ASSERT_TRUE(material->ambientColor.has_value());
            EXPECT_VECTOR3_NEAR(material->ambientColor.value(), Vector3f32(1.0f, 1.0f, 1.0f), 1e-4);

            ASSERT_TRUE(material->diffuseColor.has_value());
            EXPECT_VECTOR3_NEAR(material->diffuseColor.value(), Vector3f32(0.8f, 0.8f, 0.8f), 1e-4);

            ASSERT_TRUE(material->specularColor.has_value());
            EXPECT_VECTOR3_NEAR(material->specularColor.value(), Vector3f32(0.5f, 0.5f, 0.5f), 1e-4);

            ASSERT_TRUE(material->emissiveColor.has_value());
            EXPECT_VECTOR3_NEAR(material->emissiveColor.value(), Vector3f32(0.0f, 0.0f, 0.0f), 1e-4);

            ASSERT_TRUE(material->opticalDensity.has_value());
            EXPECT_NEAR(material->opticalDensity.value(), 1.45f, 1e-4);

            ASSERT_TRUE(material->dissolve.has_value());
            EXPECT_NEAR(material->dissolve.value(), 1.0f, 1e-4);
        }
    }

}