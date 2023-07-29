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
#include "Molten/EditorFramework/FileFormat/Builder/MaterialAssetFileBuilder.hpp"

namespace Molten::EditorFramework
{

    TEST(FileFormat, MaterialAssetFileBuilder)
    {
        using File = MaterialAssetFile;
        using DataType = File::DataType;
        using OperatorType = File::OperatorType;
        using BuiltInFunctionType = File::BuiltInFunctionType;
        using Builder = MaterialAssetFileBuilder;
        using Validator = MaterialAssetFileValidator;

        auto materialAssetFile = File{};
        auto entry = Builder{ materialAssetFile };

        auto func = entry.AddFunction("func");
        [[maybe_unused]] auto func_output_1 = func.AddOutputNode<DataType::Matrix4x4f32>();

        auto entry_param_1 = entry.AddParameterNode<DataType::Vector4f32>("param_1");
        
        auto entry_const_1 = entry.AddConstantNode<DataType::Vector3f32>({ 0.2f, 0.4f, 0.8f });
        auto entry_const_2 = entry.AddConstantNode<DataType::Float32>(0.9f);
        auto entry_const_3 = entry.AddConstantNode<DataType::Vector4f32>({ 0.3f, 0.4f, 0.5f, 0.6f });
        
        auto entry_composite_1 = entry.AddCompositeNode<DataType::Vector4f32, DataType::Vector3f32, DataType::Float32>();
        entry.LinkNodes<0>(entry_const_1, entry_composite_1);
        entry.LinkNodes<1>(entry_const_2, entry_composite_1);

        auto op_1 = entry.AddOperatorNode<OperatorType::Addition, DataType::Vector4f32, DataType::Vector4f32>();
        entry.LinkNodes<0>(entry_const_3, op_1);
        entry.LinkNodes<1>(entry_composite_1, op_1);
        
        auto min_func_1 = entry.AddBuiltInFunctionNode<BuiltInFunctionType::Min, DataType::Vector4f32, DataType::Vector4f32>();
        entry.LinkNodes<0>(entry_param_1, min_func_1);
        entry.LinkNodes<1>(op_1, min_func_1);

        auto entry_component_1 = entry.AddComponentNode<DataType::Vector4f32, 0, 2, 3>();
        entry.LinkNodes(min_func_1, entry_component_1);

        auto entry_color_output = entry.AddOutputNode<File::EntryPointOutputType::Color>();
        entry.LinkNodes(entry_component_1, entry_color_output);
    }

}