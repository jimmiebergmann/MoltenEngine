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
        using BuilderTypes = MaterialAssetFileBuilderTypes;

        auto builder = MaterialAssetFileBuilder{ materialAssetFile };

        using UvInputNodeType = BuilderTypes::OutputDataNode<BuilderTypes::DataTypesWrapper<File::DataType::Vector2f32>>;
        auto getUvInputNode = [&, uvInputNode = std::optional<UvInputNodeType>{}]() mutable
        {
            if (!uvInputNode.has_value())
            {
                uvInputNode = UvInputNodeType{ builder.AddVertexInputNode<File::VertexInputType::Uv>(0) };
            }
            return uvInputNode.value();
        };

        // Color
        using ColorOutputType = BuilderTypes::OutputDataNode<BuilderTypes::DataTypesWrapper<File::DataType::Vector3f32>>;
        auto lastColorOutputNode = std::optional<ColorOutputType>{};

        if (objMaterial.ambientColor)
        {
            const auto ambientColor = objMaterial.ambientColor.value_or(Vector3f32{ 1.0f, 1.0f, 1.0f });
            auto ambientColorNode = builder.AddConstantNode<File::DataType::Vector3f32>(ambientColor);

            lastColorOutputNode = ambientColorNode;
        }

        const auto* colorTexture = 
            objMaterial.diffuseTexture.has_value() ? &objMaterial.diffuseTexture :
                objMaterial.ambientTexture.has_value() ? &objMaterial.ambientTexture : 
                    nullptr;

        if (colorTexture != nullptr)
        {
            auto ambientTextureGlobalId = FindTextureGlobalId(*colorTexture, options);
            auto uvInputNode = getUvInputNode();
            auto ambientSamplerNode = builder.AddBuiltInFunctionNode<File::BuiltInFunctionType::SampleTexture, File::DataType::Sampler2D, File::DataType::Vector2f32>();
        
            builder.SetNodeInput<0>(File::Sampler2D{ ambientTextureGlobalId }, ambientSamplerNode);
            builder.LinkNodes<1>(uvInputNode, ambientSamplerNode);

            auto ambientSamplerNodeVec3 = builder.AddComponentNode<File::DataType::Vector4f32, 0, 1, 2>();
            builder.LinkNodes<>(ambientSamplerNode, ambientSamplerNodeVec3);

            if (lastColorOutputNode.has_value())
            {
                auto colorMult = builder.AddOperatorNode<File::OperatorType::Multiplication, File::DataType::Vector3f32, File::DataType::Vector3f32>();
                builder.LinkNodes<0>(lastColorOutputNode.value(), colorMult);
                builder.LinkNodes<1>(ambientSamplerNodeVec3, colorMult);

                lastColorOutputNode = colorMult;
            }
            else
            {
                lastColorOutputNode = ambientSamplerNodeVec3;
            }
        }

        auto colorOutputNode = builder.AddOutputNode<File::EntryPointOutputType::Color>();
        if (lastColorOutputNode.has_value())
        {          
            builder.LinkNodes(lastColorOutputNode.value(), colorOutputNode);
        }
        else
        {
            builder.SetNodeInput(Vector3f32{1.0f, 1.0f, 1.0f}, colorOutputNode);
        }
             
        // Opacity
        if (objMaterial.dissolve && std::min(*objMaterial.dissolve, 1.0f) < 1.0f)
        {
            auto opacityNode = builder.AddConstantNode<File::DataType::Float32>(objMaterial.dissolve.value());
            auto opacityOutputNode = builder.AddOutputNode<File::EntryPointOutputType::Opacity>();
            builder.LinkNodes<>(opacityNode, opacityOutputNode);
        }

        // Normal
        if (objMaterial.bumpTexture)
        {
            auto bumpTextureGlobalId = FindTextureGlobalId(objMaterial.bumpTexture, options);
            auto uvInputNode = getUvInputNode();
            auto bumpSamplerNode = builder.AddBuiltInFunctionNode<File::BuiltInFunctionType::SampleTexture, File::DataType::Sampler2D, File::DataType::Vector2f32>();

            builder.SetNodeInput<0>(File::Sampler2D{ bumpTextureGlobalId }, bumpSamplerNode);
            builder.LinkNodes<1>(uvInputNode, bumpSamplerNode);

            auto bumpSamplerNodeVec3 = builder.AddComponentNode<File::DataType::Vector4f32, 0, 1, 2>();
            builder.LinkNodes<>(bumpSamplerNode, bumpSamplerNodeVec3);

            auto normalOutputNode = builder.AddOutputNode<File::EntryPointOutputType::Normal>();
            builder.LinkNodes<>(bumpSamplerNodeVec3, normalOutputNode);
        }

        return materialAssetFile;
    }

}