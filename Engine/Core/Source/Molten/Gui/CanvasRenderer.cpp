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

#include "Molten/Gui/CanvasRenderer.hpp"
#include "Molten/Renderer/Renderer.hpp"
#include "Molten/Renderer/Pipeline.hpp"
#include "Molten/Renderer/Shader/Visual/VisualShaderScript.hpp"
#include "Molten/Renderer/Shader/Generator/GlslShaderGenerator.hpp"
#include "Molten/Logger.hpp"
#include <array>

namespace Molten::Gui
{

    CanvasRendererPointer CanvasRenderer::Create(Molten::Renderer& renderer, Logger* logger, const Vector2f32& size)
    {
        return std::make_shared<CanvasRenderer>(renderer, logger, size);
    }

    CanvasRenderer::CanvasRenderer(Molten::Renderer& renderer, Logger* logger, const Vector2f32& size) :
        m_logger(logger),
        m_backendRenderer(renderer)
    {
        if (size.x != 0.0f && size.y != 0.0f)
        {
            m_projection = Matrix4x4f32::Orthographic(0.0f, size.x, size.y, 0.0f, 1.0f, -1.0f);
        }
        
        LoadRectRenderInstance();
    }

    CanvasRenderer::~CanvasRenderer()
    {
        Close();
    }

    void CanvasRenderer::Close()
    {
        DestroyRenderInstance(m_rectInstance);
    }

    void CanvasRenderer::Resize(const Vector2f32& size)
    {
        m_projection = Matrix4x4f32::Orthographic(0.0f, size.x, size.y, 0.0f, 1.0f, -1.0f);
    }

    void CanvasRenderer::BeginDraw()
    {
        while (!m_positionStack.empty())
        {
            m_positionStack.pop();
        }
        m_positionStack.push({ 0.0f, 0.0f });
    }

    void CanvasRenderer::PushPosition(const Vector2f32& position)
    {
        Vector2f32 newPosition = (!m_positionStack.empty() ? m_positionStack.top() : Vector2f32{ 0.0f, 0.0f }) + position;
        m_positionStack.push(newPosition);
    }

    void CanvasRenderer::PopPosition()
    {
        if (!m_positionStack.empty())
        {
            m_positionStack.pop();
        }
    }

    void CanvasRenderer::DrawRect(const Bounds2f32& bounds, const Vector4f32& color)
    {
        m_backendRenderer.BindPipeline(m_rectInstance.pipeline);

        m_backendRenderer.PushConstant(m_rectInstance.projectionLocation, m_projection);
        m_backendRenderer.PushConstant(m_rectInstance.positionLocation, bounds.low);
        m_backendRenderer.PushConstant(m_rectInstance.sizeLocation, bounds.high - bounds.low);
        m_backendRenderer.PushConstant(m_rectInstance.colorLocation, color);

        m_backendRenderer.DrawVertexBuffer(m_rectInstance.indexBuffer, m_rectInstance.vertexBuffer);
    }

    void CanvasRenderer::EndDraw()
    {}

    CanvasRenderer::RenderInstance::RenderInstance() :
        vertexScript(nullptr),
        fragmentScript(nullptr),
        projectionLocation(0),
        positionLocation(0),
        sizeLocation(0),
        colorLocation(0)
    { }

    void CanvasRenderer::LoadRectRenderInstance()
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
        m_rectInstance.vertexBuffer = m_backendRenderer.CreateVertexBuffer(vertexPositionBufferDesc);
        if (!m_rectInstance.vertexBuffer)
        {
            throw Exception("Failed to create position vertex buffer.");
        }

        IndexBufferDescriptor indexBufferDesc;
        indexBufferDesc.indexCount = static_cast<uint32_t>(indices.size());
        indexBufferDesc.data = static_cast<const void*>(indices.data());
        indexBufferDesc.dataType = IndexBuffer::DataType::Uint16;
        m_rectInstance.indexBuffer = m_backendRenderer.CreateIndexBuffer(indexBufferDesc);
        if (!m_rectInstance.indexBuffer)
        {
            throw Exception("Failed to create index buffer.");
        }

        static const uint8_t textureData[4][4] = {
            { 255, 0, 0, 255 }, { 0, 255, 0, 255 },
            { 0, 0, 255, 255 }, { 255, 255, 255, 255 }
        };

        TextureDescriptor textureDesc;
        textureDesc.dimensions = {2, 2};
        textureDesc.data = textureData;
        m_rectInstance.texture = m_backendRenderer.CreateTexture(textureDesc);

        m_rectInstance.vertexScript = new Shader::Visual::VertexScript();
        m_rectInstance.fragmentScript = new Shader::Visual::FragmentScript();

        { // Vertex
            auto& script = *m_rectInstance.vertexScript;

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
            auto& script = *m_rectInstance.fragmentScript;

            auto& pushConstants = script.GetPushConstants();
            auto& vertexColor = pushConstants.AddMember<Vector4f32>(4);

            auto& outputs = script.GetOutputInterface();      
            auto& outColor = outputs.AddMember<Vector4f32>();

            outColor.Connect(vertexColor);
        }

        // Debug
        {
            Shader::GlslGenerator::GlslTemplate glslTemplates;
            std::vector<Shader::Visual::Script*> visualScripts = { m_rectInstance.vertexScript, m_rectInstance.fragmentScript };
            if (!Shader::GlslGenerator::GenerateGlslTemplate(glslTemplates, visualScripts, m_logger))
            {
                return;
            }

            Shader::GlslGenerator glslGenerator;
            auto vertGlsl = glslGenerator.Generate(*m_rectInstance.vertexScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);
            auto fragGlsl = glslGenerator.Generate(*m_rectInstance.fragmentScript, Shader::GlslGenerator::Compability::SpirV, &glslTemplates);

            std::string vertStr(vertGlsl.begin(), vertGlsl.end());
            std::string fragStr(fragGlsl.begin(), fragGlsl.end());

            Logger::WriteInfo(m_logger, "vert -------------------------------------");
            Logger::WriteInfo(m_logger, vertStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
            Logger::WriteInfo(m_logger, "frag -------------------------------------");
            Logger::WriteInfo(m_logger, fragStr);
            Logger::WriteInfo(m_logger, "-------------------------------------");
        }

        PipelineDescriptor pipelineDesc;
        pipelineDesc.cullMode = Pipeline::CullMode::None;
        pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
        pipelineDesc.topology = Pipeline::Topology::TriangleList;
        pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;   
        pipelineDesc.vertexScript = m_rectInstance.vertexScript;
        pipelineDesc.fragmentScript = m_rectInstance.fragmentScript;
        m_rectInstance.pipeline = m_backendRenderer.CreatePipeline(pipelineDesc);
        if (!m_rectInstance.pipeline)
        {
            throw Exception("Failed to create gui pipeline");
        }

        m_rectInstance.projectionLocation = m_backendRenderer.GetPushConstantLocation(m_rectInstance.pipeline, 1);
        m_rectInstance.positionLocation = m_backendRenderer.GetPushConstantLocation(m_rectInstance.pipeline, 2);
        m_rectInstance.sizeLocation = m_backendRenderer.GetPushConstantLocation(m_rectInstance.pipeline, 3);
        m_rectInstance.colorLocation = m_backendRenderer.GetPushConstantLocation(m_rectInstance.pipeline, 4);
    }

    void CanvasRenderer::DestroyRenderInstance(RenderInstance& instance)
    {
        instance.pipeline.reset();
        instance.vertexBuffer.reset();
        instance.indexBuffer.reset();
        instance.texture.reset();

        delete instance.vertexScript;
        instance.vertexScript = nullptr;
        delete instance.fragmentScript;
        instance.fragmentScript = nullptr;
    }

}