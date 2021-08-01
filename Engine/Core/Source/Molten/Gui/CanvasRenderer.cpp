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

#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Renderer/Pipeline.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Logger.hpp"
#include <array>

namespace Molten::Gui
{

    // Canvas renderer texture implementations.
    /*CanvasRendererTexture::CanvasRendererTexture(CanvasRendererTexture&& canvasRendererTexture) noexcept :
        texture(std::move(canvasRendererTexture.texture)),
        descriptorSet(std::move(canvasRendererTexture.descriptorSet))
    {}

    CanvasRendererTexture& CanvasRendererTexture::operator =(CanvasRendererTexture&& canvasRendererTexture) noexcept
    {
        texture = std::move(canvasRendererTexture.texture);
        descriptorSet = std::move(canvasRendererTexture.descriptorSet);
        return *this;
    }*/


    // Canvas renderer implementations.
    CanvasRendererPointer CanvasRenderer::Create(Renderer& renderer, Logger* logger, const Vector2f32& size)
    {
        return std::make_shared<CanvasRenderer>(renderer, logger, size);
    }

    CanvasRenderer::CanvasRenderer(Renderer& renderer, Logger* logger, const Vector2f32& size) :
        m_logger(logger),
        m_backendRenderer(renderer)
    {
        if (size.x != 0.0f && size.y != 0.0f)
        {
            m_projection = Matrix4x4f32::Orthographic(0.0f, size.x, size.y, 0.0f, 1.0f, -1.0f);
        }


        SamplerDescriptor2D samplerDescriptor;
        samplerDescriptor.magFilter = SamplerFilter::Nearest;
        samplerDescriptor.minFilter = SamplerFilter::Nearest;
        samplerDescriptor.wrapModes = { SamplerWrapMode::Repeat, SamplerWrapMode::Repeat };
        m_sampler2D = m_backendRenderer.CreateSampler(samplerDescriptor);
        if (!m_sampler2D)
        {
            throw Exception("Failed to create texture sampler.");
        }

        LoadColoredRect();
        LoadTexturedRect();
        LoadFontRenderData();
    }

    CanvasRenderer::~CanvasRenderer()
    {
        Close();
    }

    void CanvasRenderer::Close()
    {
        DestroyTexturedRect(m_texturedRect);
        DestroyColoredRect(m_coloredRect);
    }

    void CanvasRenderer::Resize(const Vector2f32& size)
    {
        m_projection = Matrix4x4f32::Orthographic(0.0f, size.x, size.y, 0.0f, 1.0f, -1.0f);
    }

    CanvasRendererTexture CanvasRenderer::CreateTexture(const TextureDescriptor2D& textureDescriptor2D)
    {
        auto texture = m_backendRenderer.CreateTexture(textureDescriptor2D);
        if(texture == nullptr)
        {
            return {};
        }

        const auto descriptorSetDescriptor = DescriptorSetDescriptor{
            &m_texturedRect.pipeline, 0,
            { 0,  { texture, m_sampler2D } }
        };
        auto descriptorSet = m_backendRenderer.CreateDescriptorSet(descriptorSetDescriptor);
        if (!descriptorSet)
        {
            return {};
        }

        CanvasRendererTexture result;
        result.texture = std::move(texture);
        result.dimensions = textureDescriptor2D.dimensions;
        result.descriptorSet = std::move(descriptorSet);

        return result;
    }

    bool CanvasRenderer::UpdateTexture(CanvasRendererTexture& texture, const TextureUpdateDescriptor2D& textureUpdateDescriptor)
    {
        return m_backendRenderer.UpdateTexture(*texture.texture, textureUpdateDescriptor);
    }

    CanvasRendererFontSequence CanvasRenderer::CreateFontSequence(FontGroupedSequence& fontGroupedSequence)
    {
        struct Vertex
        {
            Vector2f32 position;
            Vector2f32 uv;
        };

        struct Triangle
        {
            Vertex vertex[3];
        };

        struct Quad
        {
            Triangle triangle[2];
        };

        static_assert(sizeof(Vertex) == 16);
        static_assert(sizeof(Triangle) == 48);
        static_assert(sizeof(Quad) == 96);

        std::vector<std::unique_ptr<std::vector<Quad>>> quadBuffers;
        CanvasRendererFontSequence fontSequence;

        for (const auto& fontGroup : fontGroupedSequence.groups)
        {
            auto quadBuffer = std::make_unique<std::vector<Quad>>();
            quadBuffer->resize(fontGroup->glyphs.size());

            auto* texture = static_cast<CanvasRendererTexture*>(fontGroup->atlas->metaData);
            const auto textureSize = Vector2f32{ texture->dimensions };

            fontSequence.groups.push_back({});
            auto& currentGroup = fontSequence.groups.back();

            auto* currentQuad = &quadBuffer->front();

            for (const auto& glyph : fontGroup->glyphs)
            {
                currentQuad->triangle[0].vertex[0].position = glyph->bounds.low;
                currentQuad->triangle[0].vertex[1].position = Vector2f32{ glyph->bounds.high.x, glyph->bounds.low.y };
                currentQuad->triangle[0].vertex[2].position = glyph->bounds.high;

                currentQuad->triangle[0].vertex[0].uv = Vector2f32{ glyph->atlasGlyph->textureBounds.low } / textureSize;
                currentQuad->triangle[0].vertex[1].uv = Vector2f32{ static_cast<float>(glyph->atlasGlyph->textureBounds.high.x) / textureSize.x, currentQuad->triangle[0].vertex[0].uv.y};
                currentQuad->triangle[0].vertex[2].uv = Vector2f32{ glyph->atlasGlyph->textureBounds.high } / textureSize;

                currentQuad->triangle[1].vertex[0].position = glyph->bounds.low;
                currentQuad->triangle[1].vertex[1].position = glyph->bounds.high;
                currentQuad->triangle[1].vertex[2].position = Vector2f32{ glyph->bounds.low.x, glyph->bounds.high.y };

                currentQuad->triangle[1].vertex[0].uv = currentQuad->triangle[0].vertex[0].uv;
                currentQuad->triangle[1].vertex[1].uv = currentQuad->triangle[0].vertex[2].uv;
                currentQuad->triangle[1].vertex[2].uv = Vector2f32{ currentQuad->triangle[1].vertex[0].uv.x, static_cast<float>(glyph->atlasGlyph->textureBounds.high.y) / textureSize.y };

                ++currentQuad;
            }

            VertexBufferDescriptor vertexPositionBufferDesc;
            vertexPositionBufferDesc.vertexCount = static_cast<uint32_t>(quadBuffer->size() * 6);
            vertexPositionBufferDesc.vertexSize = sizeof(Vertex);
            vertexPositionBufferDesc.data = static_cast<const void*>(quadBuffer->data());
            auto vertexBuffer = m_backendRenderer.CreateVertexBuffer(vertexPositionBufferDesc);
            if (!vertexBuffer)
            {
                throw Exception("Failed to create vertex buffer for font sequence.");
            }

            currentGroup.texture = texture;
            currentGroup.vertexBuffer = std::move(vertexBuffer);

            quadBuffers.push_back(std::move(quadBuffer));
        }

        return fontSequence;
    }


    SharedRenderResource<RenderPass> CanvasRenderer::GetRenderPass()
    {
        return m_renderPass;
    }

    void CanvasRenderer::BeginDraw()
    {}

    void CanvasRenderer::DrawRect(const Bounds2f32& bounds, const Vector4f32& color)
    {
        m_backendRenderer.BindPipeline(*m_coloredRect.pipeline);

        m_backendRenderer.PushConstant(m_coloredRect.projectionLocation, m_projection);
        m_backendRenderer.PushConstant(m_coloredRect.positionLocation, bounds.low);
        m_backendRenderer.PushConstant(m_coloredRect.sizeLocation, bounds.high - bounds.low);
        m_backendRenderer.PushConstant(m_coloredRect.colorLocation, color);

        m_backendRenderer.DrawVertexBuffer(*m_coloredRect.indexBuffer, *m_coloredRect.vertexBuffer);
    }

    void CanvasRenderer::DrawRect(const Bounds2f32& bounds, CanvasRendererTexture& texture)
    {
        m_backendRenderer.BindPipeline(*m_texturedRect.pipeline);
        m_backendRenderer.BindDescriptorSet(*texture.descriptorSet);

        m_backendRenderer.PushConstant(m_texturedRect.projectionLocation, m_projection);
        m_backendRenderer.PushConstant(m_texturedRect.positionLocation, bounds.low);
        m_backendRenderer.PushConstant(m_texturedRect.sizeLocation, bounds.high - bounds.low);
        m_backendRenderer.PushConstant(m_texturedRect.uvPositionLocation, {0.0f, 0.0f});
        m_backendRenderer.PushConstant(m_texturedRect.uvSizeLocation, { 1.0f, 1.0f });

        m_backendRenderer.DrawVertexBuffer(*m_texturedRect.indexBuffer, *m_texturedRect.vertexBuffer);
    }

    void CanvasRenderer::DrawRect(const Bounds2f32& bounds, const Bounds2f32& textureCoords, CanvasRendererTexture& texture)
    {
        const auto uvPosition = textureCoords.low / Vector2f32{ texture.dimensions };
        const auto uvSize = (textureCoords.high - textureCoords.low) / Vector2f32{ texture.dimensions };

        m_backendRenderer.BindPipeline(*m_texturedRect.pipeline);
        m_backendRenderer.BindDescriptorSet(*texture.descriptorSet);

        m_backendRenderer.PushConstant(m_texturedRect.projectionLocation, m_projection);
        m_backendRenderer.PushConstant(m_texturedRect.positionLocation, bounds.low);
        m_backendRenderer.PushConstant(m_texturedRect.sizeLocation, bounds.high - bounds.low);
        m_backendRenderer.PushConstant(m_texturedRect.uvPositionLocation, uvPosition);
        m_backendRenderer.PushConstant(m_texturedRect.uvSizeLocation, uvSize);

        m_backendRenderer.DrawVertexBuffer(*m_texturedRect.indexBuffer, *m_texturedRect.vertexBuffer);
    }

    void CanvasRenderer::DrawFontSequence(const Vector2f32& position, CanvasRendererFontSequence& fontSequence)
    {
        m_backendRenderer.BindPipeline(*m_fontRenderData.pipeline);

        for(auto& group : fontSequence.groups)
        {
            m_backendRenderer.BindDescriptorSet(*group.texture->descriptorSet);

            m_backendRenderer.PushConstant(m_fontRenderData.projectionLocation, m_projection);
            m_backendRenderer.PushConstant(m_fontRenderData.positionLocation, position);
            m_backendRenderer.DrawVertexBuffer(*group.vertexBuffer);
        }
      
    }

    void CanvasRenderer::EndDraw()
    {}

    CanvasRenderer::ColoredRectData::ColoredRectData() :
        projectionLocation(0),
        positionLocation(0),
        sizeLocation(0),
        colorLocation(0)
    {}

    CanvasRenderer::TexturedRectData::TexturedRectData() :
        projectionLocation(0),
        positionLocation(0),
        sizeLocation(0),
        uvPositionLocation(0),
        uvSizeLocation(0)
    {}

    CanvasRenderer::FontRenderData::FontRenderData() :
        projectionLocation(0),
        positionLocation(0)
    {}

    void CanvasRenderer::LoadColoredRect()
    {      
        const std::array<Vector2f32, 4> vertexData =
        {
            Vector2f32{ 0.0f, 0.0f },
            Vector2f32{ 1.0f, 0.0f },
            Vector2f32{ 1.0f, 1.0f },
            Vector2f32{ 0.0f, 1.0f }
        };

        const std::array<uint16_t, 6> indices =
        {
            0, 1, 2, 
            0, 2, 3
        };

        VertexBufferDescriptor vertexPositionBufferDesc;
        vertexPositionBufferDesc.vertexCount = static_cast<uint32_t>(vertexData.size());
        vertexPositionBufferDesc.vertexSize = sizeof(Vector2f32);
        vertexPositionBufferDesc.data = static_cast<const void*>(vertexData.data());
        m_coloredRect.vertexBuffer = m_backendRenderer.CreateVertexBuffer(vertexPositionBufferDesc);
        if (!m_coloredRect.vertexBuffer)
        {
            throw Exception("Failed to create position vertex buffer.");
        }

        IndexBufferDescriptor indexBufferDesc;
        indexBufferDesc.indexCount = static_cast<uint32_t>(indices.size());
        indexBufferDesc.data = static_cast<const void*>(indices.data());
        indexBufferDesc.dataType = IndexBuffer::DataType::Uint16;
        m_coloredRect.indexBuffer = m_backendRenderer.CreateIndexBuffer(indexBufferDesc);
        if (!m_coloredRect.indexBuffer)
        {
            throw Exception("Failed to create index buffer.");
        }

        Shader::Visual::VertexScript vertexScript;
        Shader::Visual::FragmentScript fragmentScript;

        { // Vertex
            auto& script = vertexScript;

            auto& inputs = script.GetInputInterface();
            auto& vertexPosition = inputs.AddMember<Vector2f32>();

            auto& pushConstants = script.GetPushConstants();
            auto& projection = pushConstants.AddMember<Matrix4x4f32>(1);
            auto& position = pushConstants.AddMember<Vector2f32>(2);
            auto& size = pushConstants.AddMember<Vector2f32>(3);

            auto* outPosition = script.GetVertexOutput();

            auto& vertexScaled = script.CreateOperator<Shader::Visual::Operators::MultVec2f32>();
            vertexScaled.GetInputPin(0)->Connect(vertexPosition);
            vertexScaled.GetInputPin(1)->Connect(size);

            auto& vertexScaledMoved = script.CreateOperator<Shader::Visual::Operators::AddVec2f32>();
            vertexScaledMoved.GetInputPin(0)->Connect(*vertexScaled.GetOutputPin());
            vertexScaledMoved.GetInputPin(1)->Connect(position);

            auto& vertexPositionVec4 = script.CreateFunction<Shader::Visual::Functions::Vec2ToVec4f32>();
            vertexPositionVec4.GetInputPin(0)->Connect(*vertexScaledMoved.GetOutputPin());
            static_cast<Shader::Visual::InputPin<float>*>(vertexPositionVec4.GetInputPin(1))->SetDefaultValue(0.0f);
            static_cast<Shader::Visual::InputPin<float>*>(vertexPositionVec4.GetInputPin(2))->SetDefaultValue(1.0f);

            auto& projectedVertexPosition = script.CreateOperator<Shader::Visual::Operators::MultMat4Vec4f32>();
            projectedVertexPosition.GetInputPin(0)->Connect(projection);
            projectedVertexPosition.GetInputPin(1)->Connect(*vertexPositionVec4.GetOutputPin());

            outPosition->GetInputPin()->Connect(*projectedVertexPosition.GetOutputPin());
        }
        { // Fragment
            auto& script = fragmentScript;

            auto& pushConstants = script.GetPushConstants();
            auto& vertexColor = pushConstants.AddMember<Vector4f32>(4);

            auto& outputs = script.GetOutputInterface();
            auto& outColor = outputs.AddMember<Vector4f32>();

            outColor.Connect(vertexColor);
        }

        // Debug
        /*{
            Shader::GlslGenerator::GlslTemplate glslTemplates;
            std::vector<Shader::Visual::Script*> visualScripts = { m_coloredRect.vertexScript, m_coloredRect.fragmentScript };
            if (!Shader::GlslGenerator::GenerateGlslTemplate(glslTemplates, visualScripts, m_logger))
            {
                return;
            }

            Shader::GlslGenerator glslGenerator;
            auto vertGlsl = glslGenerator.Generate(*m_coloredRect.vertexScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);
            auto fragGlsl = glslGenerator.Generate(*m_coloredRect.fragmentScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);

            std::string vertStr(vertGlsl.begin(), vertGlsl.end());
            std::string fragStr(fragGlsl.begin(), fragGlsl.end());

            Logger::WriteInfo(m_logger, "vert -------------------------------------");
            Logger::WriteInfo(m_logger, vertStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
            Logger::WriteInfo(m_logger, "frag -------------------------------------");
            Logger::WriteInfo(m_logger, fragStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
        }*/

        VisualShaderProgramDescriptor shaderProgramDesc;
        shaderProgramDesc.vertexScript = &vertexScript;
        shaderProgramDesc.fragmentScript = &fragmentScript;
        auto shaderProgram = m_backendRenderer.CreateShaderProgram(shaderProgramDesc);
        if (!shaderProgram)
        {
            throw Exception("Failed to create gui shader program");
        }

        PipelineDescriptor pipelineDesc;
        pipelineDesc.cullMode = Pipeline::CullMode::None;
        pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
        pipelineDesc.topology = Pipeline::Topology::TriangleList;
        pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;   
        pipelineDesc.shaderProgram = shaderProgram;
        m_coloredRect.pipeline = m_backendRenderer.CreatePipeline(pipelineDesc);
        if (!m_coloredRect.pipeline)
        {
            throw Exception("Failed to create gui pipeline");
        }

        m_coloredRect.projectionLocation = m_backendRenderer.GetPushConstantLocation(*m_coloredRect.pipeline, 1);
        m_coloredRect.positionLocation = m_backendRenderer.GetPushConstantLocation(*m_coloredRect.pipeline, 2);
        m_coloredRect.sizeLocation = m_backendRenderer.GetPushConstantLocation(*m_coloredRect.pipeline, 3);
        m_coloredRect.colorLocation = m_backendRenderer.GetPushConstantLocation(*m_coloredRect.pipeline, 4);
    }

    void CanvasRenderer::LoadTexturedRect()
    {
        const std::array<Vector2f32, 4> vertexData =
        {
            Vector2f32{ 0.0f, 0.0f },
            Vector2f32{ 1.0f, 0.0f },
            Vector2f32{ 1.0f, 1.0f },
            Vector2f32{ 0.0f, 1.0f }
        };

        const std::array<uint16_t, 6> indices =
        {
            0, 1, 2,
            0, 2, 3
        };

        VertexBufferDescriptor vertexPositionBufferDesc;
        vertexPositionBufferDesc.vertexCount = static_cast<uint32_t>(vertexData.size());
        vertexPositionBufferDesc.vertexSize = sizeof(Vector2f32);
        vertexPositionBufferDesc.data = static_cast<const void*>(vertexData.data());
        m_texturedRect.vertexBuffer = m_backendRenderer.CreateVertexBuffer(vertexPositionBufferDesc);
        if (!m_texturedRect.vertexBuffer)
        {
            throw Exception("Failed to create position vertex buffer.");
        }

        IndexBufferDescriptor indexBufferDesc;
        indexBufferDesc.indexCount = static_cast<uint32_t>(indices.size());
        indexBufferDesc.data = static_cast<const void*>(indices.data());
        indexBufferDesc.dataType = IndexBuffer::DataType::Uint16;
        m_texturedRect.indexBuffer = m_backendRenderer.CreateIndexBuffer(indexBufferDesc);
        if (!m_texturedRect.indexBuffer)
        {
            throw Exception("Failed to create index buffer.");
        }

        Shader::Visual::VertexScript vertexScript;
        Shader::Visual::FragmentScript fragmentScript;

        { // Vertex
            auto& script = vertexScript;

            auto& inputs = script.GetInputInterface();
            auto& vertexPosition = inputs.AddMember<Vector2f32>();

            auto& pushConstants = script.GetPushConstants();
            auto& projection = pushConstants.AddMember<Matrix4x4f32>(1);
            auto& position = pushConstants.AddMember<Vector2f32>(2);
            auto& size = pushConstants.AddMember<Vector2f32>(3);

            auto& outputs = script.GetOutputInterface();
            auto& outUv = outputs.AddMember<Vector2f32>();
            outUv.Connect(vertexPosition);

            auto* outPosition = script.GetVertexOutput();

            auto& vertexScaled = script.CreateOperator<Shader::Visual::Operators::MultVec2f32>();
            vertexScaled.GetInputPin(0)->Connect(vertexPosition);
            vertexScaled.GetInputPin(1)->Connect(size);

            auto& vertexScaledMoved = script.CreateOperator<Shader::Visual::Operators::AddVec2f32>();
            vertexScaledMoved.GetInputPin(0)->Connect(*vertexScaled.GetOutputPin());
            vertexScaledMoved.GetInputPin(1)->Connect(position);

            auto& vertexPositionVec4 = script.CreateFunction<Shader::Visual::Functions::Vec2ToVec4f32>();
            vertexPositionVec4.GetInputPin(0)->Connect(*vertexScaledMoved.GetOutputPin());
            static_cast<Shader::Visual::InputPin<float>*>(vertexPositionVec4.GetInputPin(1))->SetDefaultValue(0.0f);
            static_cast<Shader::Visual::InputPin<float>*>(vertexPositionVec4.GetInputPin(2))->SetDefaultValue(1.0f);

            auto& projectedVertexPosition = script.CreateOperator<Shader::Visual::Operators::MultMat4Vec4f32>();
            projectedVertexPosition.GetInputPin(0)->Connect(projection);
            projectedVertexPosition.GetInputPin(1)->Connect(*vertexPositionVec4.GetOutputPin());

            outPosition->GetInputPin()->Connect(*projectedVertexPosition.GetOutputPin());
        }
        { // Fragment
            auto& script = fragmentScript;

            auto& pushConstants = script.GetPushConstants();
            auto& uvPosition = pushConstants.AddMember<Vector2f32>(4);
            auto& uvSize = pushConstants.AddMember<Vector2f32>(5);

            auto& inputs = script.GetInputInterface();
            auto& inUv = inputs.AddMember<Vector2f32>();

            auto& descSets = script.GetDescriptorSets();
            auto* descSet = descSets.AddSet(0);
            auto* sampler = descSet->AddBinding<Shader::Sampler2D>(0);

            auto& uvScaled = script.CreateOperator<Shader::Visual::Operators::MultVec2f32>();
            uvScaled.GetInputPin(0)->Connect(inUv);
            uvScaled.GetInputPin(1)->Connect(uvSize);

            auto& uvScaledMoved = script.CreateOperator<Shader::Visual::Operators::AddVec2f32>();
            uvScaledMoved.GetInputPin(0)->Connect(*uvScaled.GetOutputPin());
            uvScaledMoved.GetInputPin(1)->Connect(uvPosition);

            auto& textureColor = script.CreateFunction<Shader::Visual::Functions::Texture2D>();
            textureColor.GetInputPin(0)->Connect(*sampler->GetOutputPin());
            textureColor.GetInputPin(1)->Connect(*uvScaledMoved.GetOutputPin());

            auto& outputs = script.GetOutputInterface();
            auto& outColor = outputs.AddMember<Vector4f32>();

            outColor.Connect(*textureColor.GetOutputPin());
        }

        // Debug
        /*{
            Shader::GlslGenerator::GlslTemplate glslTemplates;
            std::vector<Shader::Visual::Script*> visualScripts = { &vertexScript, &fragmentScript };
            if (!Shader::GlslGenerator::GenerateGlslTemplate(glslTemplates, visualScripts, m_logger))
            {
                return;
            }

            Shader::GlslGenerator glslGenerator;
            auto vertGlsl = glslGenerator.Generate(vertexScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);
            auto fragGlsl = glslGenerator.Generate(fragmentScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);

            std::string vertStr(vertGlsl.begin(), vertGlsl.end());
            std::string fragStr(fragGlsl.begin(), fragGlsl.end());

            Logger::WriteInfo(m_logger, "vert -------------------------------------");
            Logger::WriteInfo(m_logger, vertStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
            Logger::WriteInfo(m_logger, "frag -------------------------------------");
            Logger::WriteInfo(m_logger, fragStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
        }*/

        VisualShaderProgramDescriptor shaderProgramDesc;
        shaderProgramDesc.vertexScript = &vertexScript;
        shaderProgramDesc.fragmentScript = &fragmentScript;
        auto shaderProgram = m_backendRenderer.CreateShaderProgram(shaderProgramDesc);
        if (!shaderProgram)
        {
            throw Exception("Failed to create gui shader program");
        }

        PipelineDescriptor pipelineDesc;
        pipelineDesc.cullMode = Pipeline::CullMode::None;
        pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
        pipelineDesc.topology = Pipeline::Topology::TriangleList;
        pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;
        pipelineDesc.shaderProgram = shaderProgram;
        m_texturedRect.pipeline = m_backendRenderer.CreatePipeline(pipelineDesc);
        if (!m_texturedRect.pipeline)
        {
            throw Exception("Failed to create gui pipeline");
        }

        m_texturedRect.projectionLocation = m_backendRenderer.GetPushConstantLocation(*m_texturedRect.pipeline, 1);
        m_texturedRect.positionLocation = m_backendRenderer.GetPushConstantLocation(*m_texturedRect.pipeline, 2);
        m_texturedRect.sizeLocation = m_backendRenderer.GetPushConstantLocation(*m_texturedRect.pipeline, 3);
        m_texturedRect.uvPositionLocation = m_backendRenderer.GetPushConstantLocation(*m_texturedRect.pipeline, 4);
        m_texturedRect.uvSizeLocation = m_backendRenderer.GetPushConstantLocation(*m_texturedRect.pipeline, 5);
    }

    void CanvasRenderer::LoadFontRenderData()
    {
        Shader::Visual::VertexScript vertexScript;
        Shader::Visual::FragmentScript fragmentScript;

        { // Vertex
            auto& script = vertexScript;

            auto& inputs = script.GetInputInterface();
            auto& inPosition = inputs.AddMember<Vector2f32>();
            auto& inUv = inputs.AddMember<Vector2f32>();

            auto& pushConstants = script.GetPushConstants();
            auto& projection = pushConstants.AddMember<Matrix4x4f32>(0);
            auto& position = pushConstants.AddMember<Vector2f32>(1);

            auto& outputs = script.GetOutputInterface();
            auto& outUv = outputs.AddMember<Vector2f32>();
            outUv.Connect(inUv);

            auto* outPosition = script.GetVertexOutput();

            auto& vertexPosition = script.CreateOperator<Shader::Visual::Operators::AddVec2f32>();
            vertexPosition.GetInputPin(0)->Connect(inPosition);
            vertexPosition.GetInputPin(1)->Connect(position);

            auto& vertexPositionVec4 = script.CreateFunction<Shader::Visual::Functions::Vec2ToVec4f32>();
            vertexPositionVec4.GetInputPin(0)->Connect(*vertexPosition.GetOutputPin());
            static_cast<Shader::Visual::InputPin<float>*>(vertexPositionVec4.GetInputPin(1))->SetDefaultValue(0.0f);
            static_cast<Shader::Visual::InputPin<float>*>(vertexPositionVec4.GetInputPin(2))->SetDefaultValue(1.0f);

            auto& projectedVertexPosition = script.CreateOperator<Shader::Visual::Operators::MultMat4Vec4f32>();
            projectedVertexPosition.GetInputPin(0)->Connect(projection);
            projectedVertexPosition.GetInputPin(1)->Connect(*vertexPositionVec4.GetOutputPin());

            outPosition->GetInputPin()->Connect(*projectedVertexPosition.GetOutputPin());
        }
        { // Fragment
            auto& script = fragmentScript;

            auto& inputs = script.GetInputInterface();
            auto& inUv = inputs.AddMember<Vector2f32>();

            auto& descSets = script.GetDescriptorSets();
            auto* descSet = descSets.AddSet(0);
            auto* sampler = descSet->AddBinding<Shader::Sampler2D>(0);

            auto& outputs = script.GetOutputInterface();
            auto& outColor = outputs.AddMember<Vector4f32>();

            auto& textureColor = script.CreateFunction<Shader::Visual::Functions::Texture2D>();
            textureColor.GetInputPin(0)->Connect(*sampler->GetOutputPin());
            textureColor.GetInputPin(1)->Connect(inUv);

            outColor.Connect(*textureColor.GetOutputPin()); 
        }

        // Debug
        /*{
            Shader::GlslGenerator::GlslTemplate glslTemplates;
            std::vector<Shader::Visual::Script*> visualScripts = { m_fontRenderData.vertexScript, m_fontRenderData.fragmentScript };
            if (!Shader::GlslGenerator::GenerateGlslTemplate(glslTemplates, visualScripts, m_logger))
            {
                return;
            }

            Shader::GlslGenerator glslGenerator;
            auto vertGlsl = glslGenerator.Generate(*m_fontRenderData.vertexScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);
            auto fragGlsl = glslGenerator.Generate(*m_fontRenderData.fragmentScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);

            std::string vertStr(vertGlsl.begin(), vertGlsl.end());
            std::string fragStr(fragGlsl.begin(), fragGlsl.end());

            Logger::WriteInfo(m_logger, "vert -------------------------------------");
            Logger::WriteInfo(m_logger, vertStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
            Logger::WriteInfo(m_logger, "frag -------------------------------------");
            Logger::WriteInfo(m_logger, fragStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
        }*/

        VisualShaderProgramDescriptor shaderProgramDesc;
        shaderProgramDesc.vertexScript = &vertexScript;
        shaderProgramDesc.fragmentScript = &fragmentScript;
        auto shaderProgram = m_backendRenderer.CreateShaderProgram(shaderProgramDesc);
        if (!shaderProgram)
        {
            throw Exception("Failed to create gui shader program");
        }

        PipelineDescriptor pipelineDesc;
        pipelineDesc.cullMode = Pipeline::CullMode::None;
        pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
        pipelineDesc.topology = Pipeline::Topology::TriangleList;
        pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;
        pipelineDesc.shaderProgram = shaderProgram;
        m_fontRenderData.pipeline = m_backendRenderer.CreatePipeline(pipelineDesc);
        if (!m_fontRenderData.pipeline)
        {
            throw Exception("Failed to create gui pipeline");
        }

        m_fontRenderData.projectionLocation = m_backendRenderer.GetPushConstantLocation(*m_fontRenderData.pipeline, 0);
        m_fontRenderData.positionLocation = m_backendRenderer.GetPushConstantLocation(*m_fontRenderData.pipeline, 1);
    }

    void CanvasRenderer::DestroyColoredRect(ColoredRectData& data)
    {
        data.pipeline.reset();
        data.vertexBuffer.reset();
        data.indexBuffer.reset();
    }

    void CanvasRenderer::DestroyTexturedRect(TexturedRectData& data)
    {
        data.pipeline.reset();
        data.vertexBuffer.reset();
        data.indexBuffer.reset();
    }

}