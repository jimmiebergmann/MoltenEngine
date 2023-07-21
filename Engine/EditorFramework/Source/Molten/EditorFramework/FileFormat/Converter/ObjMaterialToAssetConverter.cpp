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

#include "Molten/EditorFramework/FileFormat/Converter/ObjMaterialToAssetConverter.hpp"
#include "Molten/EditorFramework/FileFormat/Builder/MaterialAssetFileBuilder.hpp"

namespace Molten::EditorFramework
{

    static Uuid FindTextureGlobalId(
        const std::optional<ObjMaterialFile::MaterialTexture>& objMaterialTexture,
        const ConvertToMaterialAssetFileOptions& options)
    {
        auto it = std::find_if(options.textures.begin(), options.textures.end(), [&](auto& texture)
        {
            return texture.materialTexture == &objMaterialTexture;
        });

        return it != options.textures.end() ? it->assetGlobalid : Uuid{};
    }

    Expected<MaterialAssetFile, ConvertToMaterialAssetFileError> ConvertToMaterialAssetFile(
        const ObjMaterialFile::Material& objMaterial,
        const ConvertToMaterialAssetFileOptions& options)
    {
        auto materialAssetFile = MaterialAssetFile{};
        materialAssetFile.name = objMaterial.name;

        using File = MaterialAssetFile;

        auto builder = MaterialAssetFileBuilder{ materialAssetFile };

        // Color
        auto colorOutputNode = builder.AddOutputNode<File::EntryPointOutputType::Color>();

        const auto ambientColor = objMaterial.ambientColor.value_or(Vector3f32{ 1.0f, 1.0f, 1.0f });
        auto ambientColorNode = builder.AddConstantNode<File::DataType::Vector3f32>(ambientColor);
        
        builder.LinkNodes<>(ambientColorNode, colorOutputNode);
       
        // Opacity
        if (objMaterial.dissolve)
        {
            auto opacityOutputNode = builder.AddOutputNode<File::EntryPointOutputType::Opacity>();
            auto opacityNode = builder.AddConstantNode<File::DataType::Float32>(objMaterial.dissolve.value());

            builder.LinkNodes<>(opacityNode, opacityOutputNode);
        }

        // Normal
        if (objMaterial.bumpTexture)
        {
            auto bumpTextureGlobalId = FindTextureGlobalId(objMaterial.bumpTexture, options);

            auto normalOutputNode = builder.AddOutputNode<File::EntryPointOutputType::Normal>();
            auto uvInputNode = builder.AddVertexInputNode<File::VertexInputType::Uv>(0);
            auto bumpSamplerNode = builder.AddBuiltInFunctionNode<File::BuiltInFunctionType::SampleTexture, File::DataType::Sampler2D, File::DataType::Vector2f32>();

            builder.SetNodeInput<0>(File::Sampler2D{ bumpTextureGlobalId }, bumpSamplerNode);
            builder.LinkNodes<1>(uvInputNode, bumpSamplerNode);
        }

        return materialAssetFile;
    }

}