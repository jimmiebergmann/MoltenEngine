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

#include "Molten/Editor/Viewport/SceneViewport.hpp"

namespace Molten::Editor
{

    std::unique_ptr<SceneViewport> SceneViewport::Create(const SceneViewportDescriptor& descriptor)
	{
        auto result = std::unique_ptr<SceneViewport>(new SceneViewport{ descriptor.renderer, descriptor.viewportWidget });

        struct Vertex
        {
            Vector3f32 position;
            Vector4f32 color;
        };

        const std::array<Vertex, 8> vertexData =
        {
            Vertex{ { -10.0f, 10.0f, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f} },
            Vertex{ { 10.0f, 10.0f, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f} },
            Vertex{ { 10.0f, -10.0f, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f} },
            Vertex{ { -10.0f, -10.0f, 0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f} },

            Vertex{ { -15.0f, 10.0f, -20.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
            Vertex{ { 5.0f, 10.0f, -20.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
            Vertex{ { 5.0f, -10.0f, -20.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
            Vertex{ { -15.0f, -10.0f, -20.0f }, { 1.0f, 0.0f, 0.0f, 1.0f} },
        };

        const std::array<uint16_t, 12> indices =
        {
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7
        };

        VertexBufferDescriptor vertexPositionBufferDesc;
        vertexPositionBufferDesc.vertexCount = static_cast<uint32_t>(vertexData.size());
        vertexPositionBufferDesc.vertexSize = sizeof(Vertex);
        vertexPositionBufferDesc.data = static_cast<const void*>(vertexData.data());
        result->vertexBuffer = descriptor.renderer.CreateVertexBuffer(vertexPositionBufferDesc);
        if (!result->vertexBuffer)
        {
            return nullptr;
        }

        IndexBufferDescriptor indexBufferDesc;
        indexBufferDesc.indexCount = static_cast<uint32_t>(indices.size());
        indexBufferDesc.data = static_cast<const void*>(indices.data());
        indexBufferDesc.dataType = IndexBuffer::DataType::Uint16;
        result->indexBuffer = descriptor.renderer.CreateIndexBuffer(indexBufferDesc);
        if (!result->indexBuffer)
        {
            return nullptr;
        }

        Shader::Visual::VertexScript vertexScript;
        Shader::Visual::FragmentScript fragmentScript;

        { // Vertex
            auto& script = vertexScript;

            auto& inputs = script.GetInputInterface();
            auto& vertexPosition = inputs.AddMember<Vector3f32>();
            auto& vertexColor = inputs.AddMember<Vector4f32>();

            auto& pushConstants = script.GetPushConstants();
            auto& projection = pushConstants.AddMember<Matrix4x4f32>(0);

            auto& positionComposit = script.CreateComposite<Shader::Visual::Composites::Vec4f32FromVec3f32Float32>();
            positionComposit.GetInput<0>().Connect(vertexPosition);
            positionComposit.GetInput<1>().SetDefaultValue(1.0f);

            auto& projectedVertexPosition = script.CreateOperator<Shader::Visual::Operators::MultMat4f32Vec4f32>();
            projectedVertexPosition.GetLeftInput().Connect(projection);
            projectedVertexPosition.GetRightInput().Connect(positionComposit.GetOutput());

            auto* outPosition = script.GetVertexOutput();
            outPosition->GetInputPin()->ConnectBase(*projectedVertexPosition.GetOutputPin());

            auto& outputs = script.GetOutputInterface();
            auto& outColor = outputs.AddMember<Vector4f32>();
            outColor.Connect(vertexColor);
        }
        { // Fragment
            auto& script = fragmentScript;

            auto& inputs = script.GetInputInterface();
            auto& vertexColor = inputs.AddMember<Vector4f32>();

            auto& outputs = script.GetOutputInterface();
            auto& outColor = outputs.AddMember<Vector4f32>();

            outColor.Connect(vertexColor);
        }

        VisualShaderProgramDescriptor shaderProgramDesc;
        shaderProgramDesc.vertexScript = &vertexScript;
        shaderProgramDesc.fragmentScript = &fragmentScript;
        result->shaderProgram = descriptor.renderer.CreateShaderProgram(shaderProgramDesc);
        if (!result->shaderProgram)
        {
            return nullptr;
        }

        return result;
	}

    bool SceneViewport::Resize(const Vector2ui32 size)
    {
        auto createTextures = [&]()->std::array<SharedRenderResource<FramedTexture2D>, 2>
        {
            const auto dimensions = Vector2ui32{
                bufferCapacityPolicy[0].GetCapacity(),
                bufferCapacityPolicy[1].GetCapacity()
            };

            TextureDescriptor2D colorTextureDesc = {};
            colorTextureDesc.dimensions = dimensions;
            colorTextureDesc.type = TextureType::Color;
            colorTextureDesc.initialUsage = TextureUsage::Attachment;
            colorTextureDesc.format = ImageFormat::URed8Green8Blue8Alpha8;
            colorTextureDesc.internalFormat = ImageFormat::URed8Green8Blue8Alpha8;

            auto colorTexture = m_renderer.CreateFramedTexture(colorTextureDesc);
            if (!colorTexture)
            {
                return {};
            }

            TextureDescriptor2D depthTextureDesc = {};
            depthTextureDesc.dimensions = dimensions;
            depthTextureDesc.type = TextureType::DepthStencil;
            depthTextureDesc.initialUsage = TextureUsage::Attachment;
            depthTextureDesc.format = ImageFormat::SDepthFloat24StencilUint8;
            depthTextureDesc.internalFormat = ImageFormat::SDepthFloat24StencilUint8;

            auto depthTexture = m_renderer.CreateFramedTexture(depthTextureDesc);
            if (!depthTexture)
            {
                return {};
            }

            return { std::move(colorTexture), std::move(depthTexture) };
        };

        const Vector2b capacitySetSizeResult = {
            bufferCapacityPolicy[0].SetSize(size.x),
            bufferCapacityPolicy[1].SetSize(size.y)
        };

        if (!renderPass)
        {
            auto textures = createTextures();
            colorTexture = std::move(textures[0]);
            depthTexture = std::move(textures[1]);

            m_viewportWidget.SetTexture(colorTexture);

            const RenderPassDescriptor renderPassDesc = {
                size,
                {
                    {
                        colorTexture,
                        RenderPassAttachmentColorClearValue{ { 0.0f, 0.0f, 0.0f, 0.0f} },
                        TextureType::Color,
                        TextureUsage::Attachment,
                        TextureType::Color,
                        TextureUsage::ReadOnly
                    },
                    {
                        depthTexture,
                        RenderPassAttachmentDepthStencilClearValue{ 1.0f, uint8_t{0} },
                        TextureType::DepthStencil,
                        TextureUsage::Attachment
                    }
                }
            };

            renderPassUpdateDesc = {
                size,
                {
                    {
                        colorTexture,
                        TextureType::Color,
                        TextureUsage::Attachment,
                        TextureType::Color,
                        TextureUsage::ReadOnly
                    },
                    {
                        depthTexture,
                        TextureType::DepthStencil,
                        TextureUsage::Attachment
                    }
                }
            };

            renderPass = m_renderer.CreateRenderPass(renderPassDesc);
            if (!renderPass)
            {
                return false;
            }

            renderPass->SetRecordFunction([&](CommandBuffer& commandBuffer) {
                Draw(commandBuffer, Time::Zero);
            });

            PipelineDescriptor pipelineDesc;
            pipelineDesc.cullMode = Pipeline::CullMode::None;
            pipelineDesc.polygonMode = Pipeline::PolygonMode::Fill;
            pipelineDesc.topology = Pipeline::Topology::TriangleList;
            pipelineDesc.frontFace = Pipeline::FrontFace::Clockwise;
            pipelineDesc.renderPass = renderPass;
            pipelineDesc.shaderProgram = shaderProgram;
            pipeline = m_renderer.CreatePipeline(pipelineDesc);
            if (!pipeline)
            {
                return false;
            }

            projectionLocation = m_renderer.GetPushConstantLocation(*pipeline, 0);
        }
        else
        {
            renderPassUpdateDesc.dimensions = size;

            if (const bool createNewTexture = capacitySetSizeResult.x || capacitySetSizeResult.y; createNewTexture)
            {
                auto textures = createTextures();
                colorTexture = std::move(textures[0]);
                depthTexture = std::move(textures[1]);

                m_viewportWidget.SetTexture(colorTexture);

                renderPassUpdateDesc.attachments[0].texture = colorTexture;
                renderPassUpdateDesc.attachments[1].texture = depthTexture;
            }

            if (!m_renderer.UpdateRenderPass(*renderPass, renderPassUpdateDesc))
            {
                return false;
            }
        }

        return true;
    }

    SceneViewport::SceneViewport(
        Renderer& renderer,
        Gui::Viewport<Gui::EditorTheme>& viewportWidget
    ) :
        m_renderer(renderer),
        m_viewportWidget(viewportWidget)
    {}

    void SceneViewport::Draw(CommandBuffer& commandBuffer, Time deltaTime)
    {
        // Very temporary scene viewport rendering.
        static float totalDelta = 0.0f;
        totalDelta += deltaTime.AsSeconds<float>();
        const auto lookAtX = std::sin(totalDelta * 4.0f) * 20.0f;

        const auto projectionMatrix = Matrix4x4f32::Perspective(Degrees(60), 1.0f, 0.1f, 100.0f);
        const auto viewMatrix = Matrix4x4f32::LookAtPoint({ 0.0f, 0.0f, 60.0f }, { lookAtX, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        const auto finalProjecton = projectionMatrix * viewMatrix;

        commandBuffer.BindPipeline(*pipeline);
        commandBuffer.PushConstant(projectionLocation, finalProjecton);
        commandBuffer.DrawVertexBuffer(*indexBuffer, *vertexBuffer);
    }

}
