/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
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
#include "Molten/EditorFramework/FileFormat/ProjectFile.hpp"
#include "Molten/Utility/Template.hpp"
#include <sstream>

namespace Molten
{
    TEST(FileFormat, ProjectFile_Success)
    {
        {
            std::string content = R"(
            {
            }
            )";

            auto stream = std::stringstream{ content };
            auto projectFile = ReadProjectFile(stream);
            
            ASSERT_TRUE(projectFile.IsValid());
            ASSERT_EQ(projectFile.Value().warnings.size(), size_t{ 3 });
            ASSERT_EQ(projectFile.Value().warnings.at(0), ProjectFileWarningCode::MissingFileVersion);
            ASSERT_EQ(projectFile.Value().warnings.at(1), ProjectFileWarningCode::MissingEngineVersion);
            ASSERT_EQ(projectFile.Value().warnings.at(2), ProjectFileWarningCode::MissingGlobalId);
        }
        {
            std::string content = R"(
            {
	            "file_version": "1.2.3",
	            "engine_version": "4.5.6",
	            "global_id": "78563412-ab90-efcd-2143-658709badcfe",
	            "description": "Hello world"
            }
            )";

            auto stream = std::stringstream{ content };
            auto projectFile = ReadProjectFile(stream);
            ASSERT_TRUE(projectFile.IsValid());

            const auto project = projectFile.Value();
            EXPECT_TRUE(project.warnings.empty());
            EXPECT_EQ(project.data.fileVersion, Version(1, 2, 3));
            EXPECT_EQ(project.data.engineVersion, Version(4, 5, 6));
            EXPECT_NE(project.data.globalId, Uuid{});
            EXPECT_STREQ(project.data.description.c_str(), "Hello world");
        }
        {
            std::string content = R"(
            {
	            "file_version": "1.2.3",
	            "engine_version": "4.5.6",
	            "global_id": "78563412-ab90-efcd-2143-658709badcfe"
            }
            )";

            auto stream = std::stringstream{ content };
            auto projectFile = ReadProjectFile(stream);
            ASSERT_TRUE(projectFile.IsValid());

            const auto project = projectFile.Value();
            EXPECT_TRUE(project.warnings.empty());
            EXPECT_EQ(project.data.fileVersion, Version(1, 2, 3 ));
            EXPECT_EQ(project.data.engineVersion, Version(4, 5, 6));
            EXPECT_NE(project.data.globalId, Uuid{});
            EXPECT_STREQ(project.data.description.c_str(), "");
        }
    }

    TEST(FileFormat, ProjectFile_Error_FileVersion)
    {
        {
            std::string content = R"(
            {
	            "engine_version": "4.5.6",
	            "global_id": "78563412-ab90-efcd-2143-658709badcfe",
	            "description": "Hello world"
            }
            )";

            auto stream = std::stringstream{ content };
            auto projectFile = ReadProjectFile(stream);

            ASSERT_TRUE(projectFile.IsValid());
            ASSERT_EQ(projectFile.Value().warnings.size(), size_t{ 1 });
            ASSERT_EQ(projectFile.Value().warnings.front(), ProjectFileWarningCode::MissingFileVersion);
        }
        {
            std::string content = R"(
            {
	            "file_version": "1a.2.3",
	            "engine_version": "4.5.6",
	            "global_id": "78563412-ab90-efcd-2143-658709badcfe",
	            "description": "Hello world"
            }
            )";

            auto stream = std::stringstream{ content };
            auto projectFile = ReadProjectFile(stream);
            ASSERT_FALSE(projectFile.IsValid());

            ASSERT_TRUE(VariantEqualsValue<ProjectFileErrorCode>(projectFile.Error(), ProjectFileErrorCode::InvalidFileVersion));
        }
    }

    TEST(FileFormat, ProjectFile_Error_EngineVersion)
    {
        {
            std::string content = R"(
            {
                "file_version": "1.2.3",
	            "global_id": "78563412-ab90-efcd-2143-658709badcfe",
	            "description": "Hello world"
            }
            )";

            auto stream = std::stringstream{ content };
            auto projectFile = ReadProjectFile(stream);

            ASSERT_TRUE(projectFile.IsValid());
            ASSERT_EQ(projectFile.Value().warnings.size(), size_t{ 1 });
            ASSERT_EQ(projectFile.Value().warnings.front(), ProjectFileWarningCode::MissingEngineVersion);
        }
        {
            std::string content = R"(
            {
	            "file_version": "1.2.3",
	            "engine_version": "4a.5.6",
	            "global_id": "78563412-ab90-efcd-2143-658709badcfe",
	            "description": "Hello world"
            }
            )";

            auto stream = std::stringstream{ content };
            auto projectFile = ReadProjectFile(stream);
            ASSERT_FALSE(projectFile.IsValid());

            ASSERT_TRUE(VariantEqualsValue<ProjectFileErrorCode>(projectFile.Error(), ProjectFileErrorCode::InvalidEngineVersion));
        }
    }

    TEST(FileFormat, ProjectFile_Error_GlobalId)
    {
        {
            std::string content = R"(
            {
                "file_version": "1.2.3",
                "engine_version": "4.5.6",
	            "description": "Hello world"
            }
            )";

            auto stream = std::stringstream{ content };
            auto projectFile = ReadProjectFile(stream);
            
            ASSERT_TRUE(projectFile.IsValid());
            ASSERT_EQ(projectFile.Value().warnings.size(), size_t{ 1 });
            ASSERT_EQ(projectFile.Value().warnings.front(), ProjectFileWarningCode::MissingGlobalId);

        }
        {
            std::string content = R"(
            {
	            "file_version": "1.2.3",
	            "engine_version": "4.5.6",
	            "global_id": "test",
	            "description": "Hello world"
            }
            )";

            auto stream = std::stringstream{ content };
            auto projectFile = ReadProjectFile(stream);
            ASSERT_FALSE(projectFile.IsValid());

            ASSERT_TRUE(VariantEqualsValue<ProjectFileErrorCode>(projectFile.Error(), ProjectFileErrorCode::InvalidGlobalId));
        }
    }

}