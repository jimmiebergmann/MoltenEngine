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
    CanvasRendererTexture::CanvasRendererTexture(CanvasRendererTexture&& canvasRendererTexture) noexcept :
        texture(std::move(canvasRendererTexture.texture)),
        descriptorSet(std::move(canvasRendererTexture.descriptorSet))
    {}

    CanvasRendererTexture& CanvasRendererTexture::operator =(CanvasRendererTexture&& canvasRendererTexture) noexcept
    {
        texture = std::move(canvasRendererTexture.texture);
        descriptorSet = std::move(canvasRendererTexture.descriptorSet);
        return *this;
    }


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
        
        LoadColoredRect();
        LoadTexturedRect();
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
            { 0,  { texture, *m_texturedRect.sampler2D } }
        };
        auto descriptorSet = m_backendRenderer.CreateDescriptorSet(descriptorSetDescriptor);
        if (!descriptorSet)
        {
            return {};
        }

        CanvasRendererTexture result;
        result.texture = std::move(texture);
        result.descriptorSet = std::move(descriptorSet);

        return result;
    }

    bool CanvasRenderer::UpdateTexture(CanvasRendererTexture& texture, const TextureUpdateDescriptor2D& textureUpdateDescriptor)
    {
        return m_backendRenderer.UpdateTexture(*texture.texture, textureUpdateDescriptor);
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

        m_backendRenderer.DrawVertexBuffer(*m_texturedRect.indexBuffer, *m_texturedRect.vertexBuffer);
    }

    void CanvasRenderer::EndDraw()
    {}

    CanvasRenderer::ColoredRectData::ColoredRectData() :
        vertexScript(nullptr),
        fragmentScript(nullptr),
        projectionLocation(0),
        positionLocation(0),
        sizeLocation(0),
        colorLocation(0)
    {}

    CanvasRenderer::TexturedRectData::TexturedRectData() :
        vertexScript(nullptr),
        fragmentScript(nullptr),
        projectionLocation(0),
        positionLocation(0),
        sizeLocation(0)
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

        static const uint8_t textureData[4][4] = {
            { 255, 0, 0, 255 }, { 0, 255, 0, 255 },
            { 0, 0, 255, 255 }, { 255, 255, 255, 255 }
        };

        m_coloredRect.vertexScript = new Shader::Visual::VertexScript();
        m_coloredRect.fragmentScript = new Shader::Visual::FragmentScript();

        { // Vertex
            auto& script = *m_coloredRect.vertexScript;

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
            auto& script = *m_coloredRect.fragmentScript;

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

        PipelineDescriptor pipelineDesc;
        pipelineDesc.cullMode = Pipeline::CullMode::None;
        pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
        pipelineDesc.topology = Pipeline::Topology::TriangleList;
        pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;   
        pipelineDesc.vertexScript = m_coloredRect.vertexScript;
        pipelineDesc.fragmentScript = m_coloredRect.fragmentScript;
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

        SamplerDescriptor2D samplerDescriptor;
        samplerDescriptor.magFilter = SamplerFilter::Nearest;
        samplerDescriptor.minFilter = SamplerFilter::Nearest;
        samplerDescriptor.wrapModes = { SamplerWrapMode::Repeat, SamplerWrapMode::Repeat };
        
        m_texturedRect.sampler2D = m_backendRenderer.CreateSampler(samplerDescriptor);
        if (!m_texturedRect.sampler2D)
        {
            throw Exception("Failed to create texture sampler.");
        }

        m_texturedRect.vertexScript = new Shader::Visual::VertexScript();
        m_texturedRect.fragmentScript = new Shader::Visual::FragmentScript();

        { // Vertex
            auto& script = *m_texturedRect.vertexScript;

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
            auto& script = *m_texturedRect.fragmentScript;

            auto& inputs = script.GetInputInterface();
            auto& inUv = inputs.AddMember<Vector2f32>();

            auto& descSets = script.GetDescriptorSets();
            auto* descSet = descSets.AddSet(0);
            auto* sampler = descSet->AddBinding<Shader::Sampler2D>(0);

            auto& textureColor = script.CreateFunction<Shader::Visual::Functions::Texture2D>();
            textureColor.GetInputPin(0)->Connect(*sampler->GetOutputPin());
            textureColor.GetInputPin(1)->Connect(inUv);

            auto& outputs = script.GetOutputInterface();
            auto& outColor = outputs.AddMember<Vector4f32>();

            outColor.Connect(*textureColor.GetOutputPin());
        }

        // Debug
        /*{
            Shader::GlslGenerator::GlslTemplate glslTemplates;
            std::vector<Shader::Visual::Script*> visualScripts = { m_texturedRect.vertexScript, m_texturedRect.fragmentScript };
            if (!Shader::GlslGenerator::GenerateGlslTemplate(glslTemplates, visualScripts, m_logger))
            {
                return;
            }

            Shader::GlslGenerator glslGenerator;
            auto vertGlsl = glslGenerator.Generate(*m_texturedRect.vertexScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);
            auto fragGlsl = glslGenerator.Generate(*m_texturedRect.fragmentScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);

            std::string vertStr(vertGlsl.begin(), vertGlsl.end());
            std::string fragStr(fragGlsl.begin(), fragGlsl.end());

            Logger::WriteInfo(m_logger, "vert -------------------------------------");
            Logger::WriteInfo(m_logger, vertStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
            Logger::WriteInfo(m_logger, "frag -------------------------------------");
            Logger::WriteInfo(m_logger, fragStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
        }*/

        PipelineDescriptor pipelineDesc;
        pipelineDesc.cullMode = Pipeline::CullMode::None;
        pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
        pipelineDesc.topology = Pipeline::Topology::TriangleList;
        pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;
        pipelineDesc.vertexScript = m_texturedRect.vertexScript;
        pipelineDesc.fragmentScript = m_texturedRect.fragmentScript;
        m_texturedRect.pipeline = m_backendRenderer.CreatePipeline(pipelineDesc);
        if (!m_texturedRect.pipeline)
        {
            throw Exception("Failed to create gui pipeline");
        }

        m_texturedRect.projectionLocation = m_backendRenderer.GetPushConstantLocation(*m_texturedRect.pipeline, 1);
        m_texturedRect.positionLocation = m_backendRenderer.GetPushConstantLocation(*m_texturedRect.pipeline, 2);
        m_texturedRect.sizeLocation = m_backendRenderer.GetPushConstantLocation(*m_texturedRect.pipeline, 3);
    }

    void CanvasRenderer::DestroyColoredRect(ColoredRectData& data)
    {
        data.pipeline.reset();
        data.vertexBuffer.reset();
        data.indexBuffer.reset();

        delete data.vertexScript;
        data.vertexScript = nullptr;
        delete data.fragmentScript;
        data.fragmentScript = nullptr;
    }

    void CanvasRenderer::DestroyTexturedRect(TexturedRectData& data)
    {
        data.pipeline.reset();
        data.vertexBuffer.reset();
        data.indexBuffer.reset();

        delete data.vertexScript;
        data.vertexScript = nullptr;
        delete data.fragmentScript;
        data.fragmentScript = nullptr;
    }

}