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
        EXPECT_EQ(result.GetError().lineNumber, 0);
    }

    TEST(FileFormat, ObjMeshFile)
    {
        ThreadPool threadPool;

        ObjMeshFile objFile;

        {
            Molten::Test::Benchmarker bm("Model");
            //const auto result = objFile.ReadFromFile("C:/temp/Sponza/Sponza.obj", threadPool);
            const auto result = objFile.ReadFromFile("../Engine/Test/Data/ObjMesh/TestCubes.obj", threadPool);

            EXPECT_TRUE(result.IsSuccessful());
        }
    }

}