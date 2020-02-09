#include "Curse/Window/Window.hpp"

#include "Curse/Gui/GuiCanvas.hpp"
#include "Curse/Gui/Control/GuiButtonControl.hpp"

#include "Curse/Renderer/Renderer.hpp"
#include "Curse/Renderer/Shader/ShaderScript.hpp"
#include "Curse/System/Exception.hpp"
#include "Curse/System/FileSystem.hpp"
#include "Curse/Memory/Pointer.hpp"
#include "Curse/System/Clock.hpp"
#include "Curse/Logger.hpp"
#include <iostream>

static Curse::Logger logger;
static Curse::Window* window = nullptr;

struct UniformBuffer
{
    Curse::Vector3f32 position;
};

static void LoadShaders(Curse::Shader::VertexScript& vScript, Curse::Shader::FragmentScript& fScript)
{
    // Vertex shader.
    {
        auto outPos = vScript.GetVertexOutputNode();
        auto outColor = vScript.CreateVaryingOutNode<Curse::Vector4f32>();
        auto vPos = vScript.CreateVaryingInNode<Curse::Vector3f32>();
        auto color = vScript.CreateVaryingInNode<Curse::Vector4f32>();

        auto uniforms = vScript.CreateUniformBlock(0);
        auto pos = uniforms->CreateUniformNode<Curse::Vector3f32>();
        
        auto addPos = vScript.CreateOperatorNode<Curse::Vector3f32>(Curse::Shader::Operator::Addition);
        addPos->GetInputPin(0)->Connect(*vPos->GetOutputPin());
        addPos->GetInputPin(1)->Connect(*pos->GetOutputPin());

        outPos->GetInputPin()->Connect(*addPos->GetOutputPin());
        outColor->GetInputPin()->Connect(*color->GetOutputPin());
    }

    // Fragment shader.
    {
        auto output = fScript.CreateVaryingOutNode<Curse::Vector4f32>();
        auto color = fScript.CreateVaryingInNode<Curse::Vector4f32>();
        auto mult = fScript.CreateOperatorNode<Curse::Vector4f32>(Curse::Shader::Operator::Multiplication);
        auto add = fScript.CreateOperatorNode<Curse::Vector4f32>(Curse::Shader::Operator::Addition);
        auto const1 = fScript.CreateConstantNode<Curse::Vector4f32>({ 0.0f, 0.0f, 0.3f, 0.0f });
        auto const2 = fScript.CreateConstantNode<Curse::Vector4f32>({ 1.0f, 0.5f, 0.0f, 1.0f });
        auto cos = fScript.CreateFunctionNode<Curse::Shader::Function::CosVec4f32>();

        output->GetInputPin()->Connect(*add->GetOutputPin());
        add->GetInputPin(0)->Connect(*mult->GetOutputPin());
        add->GetInputPin(1)->Connect(*const1->GetOutputPin());
        mult->GetInputPin(0)->Connect(*color->GetOutputPin());
        mult->GetInputPin(1)->Connect(*cos->GetOutputPin());
        cos->GetInputPin()->Connect(*const2->GetOutputPin());
    }
}

void LoadGui(Curse::Gui::Canvas& canvas)
{
    auto& plane = canvas.GetPlane();

    auto button = new Curse::Gui::Button;
    button->SetPosiion({100.0f, 200.0f});
    button->SetSize({ 30.0f, 50.0f });

    plane.Add(*button);
}

static void Run()
{
    window = Curse::Window::Create();
    Curse::Vector2ui32 windowSize = { 800, 600 };
    std::string windowTitle = "Curse Editor";
    if (!window->Open(windowTitle, windowSize, &logger))
    {
        return;
    }

    auto renderer = Curse::Renderer::Create(Curse::Renderer::BackendApi::Vulkan);
    renderer->Open(*window, Curse::Version(1, 1), &logger);

    Curse::Gui::Canvas canvas(*renderer, windowSize);
    LoadGui(canvas);

    Curse::Shader::VertexScript vertexScript;
    Curse::Shader::FragmentScript fragmentScript;
    LoadShaders(vertexScript, fragmentScript);

    auto shaderSourceVec = vertexScript.GenerateGlsl();
    std::string shaderSource(shaderSourceVec.begin(), shaderSourceVec.end());

    Curse::Shader::Program* vertexShader = renderer->CreateShaderProgram(vertexScript);
    Curse::Shader::Program* fragmentShader = renderer->CreateShaderProgram(fragmentScript);

    struct Vertex
    {
        Curse::Vector3f32 position;
        Curse::Vector4f32 color;
    };

    static const uint32_t vertexCount = 4;
    static const Vertex vertices[vertexCount] =
    {
        { { -0.5f, -0.5, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
        { { 0.5f, -0.5, 0.0f },  { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.5f, 0.5, 0.0f },   { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { -0.5f, 0.5, 0.0f },  { 1.0f, 0.0f, 1.0f, 1.0f } }
    };

    static const uint32_t indexCount = 6;
    static const uint16_t indices[indexCount] =
    {
        0, 1, 2, 0, 2, 3
    };

    Curse::VertexBufferDescriptor vertexBufferDesc;
    vertexBufferDesc.vertexCount =  vertexCount;
    vertexBufferDesc.vertexSize = sizeof(Vertex);
    vertexBufferDesc.data = static_cast<const void*>(&vertices[0]);
    Curse::VertexBuffer* vertexBuffer = renderer->CreateVertexBuffer(vertexBufferDesc);

    Curse::IndexBufferDescriptor indexBufferDesc;
    indexBufferDesc.indexCount = indexCount;
    indexBufferDesc.data = static_cast<const void*>(indices); 
    indexBufferDesc.dataType = Curse::IndexBuffer::DataType::Uint16;
    Curse::IndexBuffer* indexBuffer = renderer->CreateIndexBuffer(indexBufferDesc);

    Curse::UniformBufferDescriptor uniformBufferDesc;
    uniformBufferDesc.size = 512;//sizeof(UniformBuffer) * 2;
    Curse::UniformBuffer* uniformBuffer = renderer->CreateUniformBuffer(uniformBufferDesc);

    Curse::Pipeline::VertexAttribute vertexAttrib1;
    vertexAttrib1.location = 0;
    vertexAttrib1.offset = offsetof(Vertex, position);
    vertexAttrib1.format = Curse::Pipeline::AttributeFormat::R32_G32_B32_Float;   

    Curse::Pipeline::VertexAttribute vertexAttrib2;
    vertexAttrib2.location = 1;
    vertexAttrib2.offset = offsetof(Vertex, color);
    vertexAttrib2.format = Curse::Pipeline::AttributeFormat::R32_G32_B32_A32_Float;  

    Curse::Pipeline::VertexBinding vertexBinding;
    vertexBinding.binding = 0;
    vertexBinding.stride = sizeof(Vertex);
    vertexBinding.attributes.push_back(vertexAttrib1);
    vertexBinding.attributes.push_back(vertexAttrib2);

    Curse::Pipeline::UniformBinding uniformBinding;
    uniformBinding.binding = 0;
    uniformBinding.shaderType = Curse::ShaderType::Vertex;

    Curse::PipelineDescriptor pipelineDesc;
    pipelineDesc.topology = Curse::Pipeline::Topology::TriangleList;
    pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Fill;
    pipelineDesc.frontFace = Curse::Pipeline::FrontFace::Clockwise;
    pipelineDesc.cullMode = Curse::Pipeline::CullMode::Back;
    pipelineDesc.shaderPrograms = { vertexShader, fragmentShader };
    pipelineDesc.vertexBindings = { vertexBinding };
    pipelineDesc.uniformBindings = { uniformBinding };

    Curse::Pipeline* pipeline = renderer->CreatePipeline(pipelineDesc);
    
    Curse::UniformBlockDescriptor uniformBlockDesc;
    uniformBlockDesc.offset = 0;
    uniformBlockDesc.size = sizeof(UniformBuffer);
    uniformBlockDesc.binding = 0;
    uniformBlockDesc.buffer = uniformBuffer;
    uniformBlockDesc.pipeline = pipeline;
    Curse::UniformBlock* uniformBlock = renderer->CreateUniformBlock(uniformBlockDesc);

    auto renderFunction = [&]()
    {     
        static Curse::Clock runTimer;
        float runTime = runTimer.GetTime().AsSeconds<float>();

        static Curse::Clock fpsTimer;
        static uint32_t fps = 0;
        fps++;
        if (fpsTimer.GetTime() >= Curse::Seconds(1.0f))
        {
            fpsTimer.Reset();
            window->SetTitle(windowTitle + " - " + std::to_string(fps) + "FPS");
            fps = 0;
        }
        
        renderer->Resize(window->GetSize());
        renderer->BeginDraw();

        renderer->BindPipeline(pipeline);

        UniformBuffer bufferData1;
        bufferData1.position = { std::sin(runTime * 3.0f) * 0.25f, 0.0f, 0.0f };
        UniformBuffer bufferData2;
        bufferData2.position = { 0.0f, std::cos(runTime * 3.0f) * 0.25f, 0.0f };

        renderer->UpdateUniformBuffer(uniformBuffer, 0, sizeof(UniformBuffer), &bufferData1);
        renderer->UpdateUniformBuffer(uniformBuffer, 256, sizeof(UniformBuffer), &bufferData2);

        renderer->BindUniformBlock(uniformBlock, 256);
        renderer->DrawVertexBuffer(indexBuffer, vertexBuffer);

        renderer->BindUniformBlock(uniformBlock, 0);
        renderer->DrawVertexBuffer(indexBuffer, vertexBuffer);

        canvas.Draw();

        renderer->EndDraw();
    };

    auto resizeCallback = [&](Curse::Vector2ui32 )
    {
        renderFunction();
    };

    window->OnMaximize.Connect(resizeCallback);
    window->OnMinimize.Connect(resizeCallback);
    window->OnResize.Connect(resizeCallback);
   
    window->Show();

    while (window->IsOpen())
    {
        window->Update();
        if (!window->IsOpen())
        {
            break;
        }

        Curse::UserInput userInput = window->GetUserInput();

        /*Curse::UserInput::Event event;
        while (userInput.PollEvent(event))
        {
            switch (event.type)
            {
            case Curse::UserInput::Event::Type::MouseButtonPressed: logger.Write(Curse::Logger::Severity::Info,
                "Button pressed: " + std::to_string(static_cast<size_t>(event.mouseButtonEvent.button)) +
                " pos: " + std::to_string(event.mouseButtonEvent.position.x) + " " + std::to_string(event.mouseButtonEvent.position.y));
                break;
            case Curse::UserInput::Event::Type::MouseButtonDown: logger.Write(Curse::Logger::Severity::Info,
                "Button down: " + std::to_string(static_cast<size_t>(event.mouseButtonEvent.button)) +
                " pos: " + std::to_string(event.mouseButtonEvent.position.x) + " " + std::to_string(event.mouseButtonEvent.position.y));
                break;
            case Curse::UserInput::Event::Type::MouseButtonReleased: logger.Write(Curse::Logger::Severity::Info,
                "Button released: " + std::to_string(static_cast<size_t>(event.mouseButtonEvent.button)) +
                " pos: " + std::to_string(event.mouseButtonEvent.position.x) + " " + std::to_string(event.mouseButtonEvent.position.y));
                break;
            case Curse::UserInput::Event::Type::MouseMove : logger.Write(Curse::Logger::Severity::Info,
                "Mouse move: " + std::to_string(event.mouseMoveEvent.position.x) + " " + std::to_string(event.mouseMoveEvent.position.y));
                break;
            case Curse::UserInput::Event::Type::KeyPressed: logger.Write(Curse::Logger::Severity::Info,
                "Key pressed: " + std::to_string((int)event.keyboardEvent.key));
                break;
            case Curse::UserInput::Event::Type::KeyDown: logger.Write(Curse::Logger::Severity::Info,
                "Key down: " + std::to_string((int)event.keyboardEvent.key));
                break;
            case Curse::UserInput::Event::Type::KeyReleased: logger.Write(Curse::Logger::Severity::Info,
                "Key released: " + std::to_string((int)event.keyboardEvent.key));
                break;
            default:
                logger.Write(Curse::Logger::Severity::Info, "Unkown event.");
                break;
            }
        }*/

        renderFunction();
    }

    renderer->WaitForDevice();
    renderer->DestroyUniformBlock(uniformBlock);
    renderer->DestroyUniformBuffer(uniformBuffer);
    renderer->DestroyVertexBuffer(vertexBuffer);
    renderer->DestroyIndexBuffer(indexBuffer);
    renderer->DestroyShaderProgram(vertexShader);
    renderer->DestroyShaderProgram(fragmentShader);
    renderer->DestroyPipeline(pipeline);
    delete renderer;
    delete window;
}

int main()
{
    logger.Write(Curse::Logger::Severity::Info, "Starting Curse Editor v" + CURSE_VERSION.AsString(false));

    try
    {
        Run();
    }
    catch (Curse::Exception & e)
    {
        logger.Write(Curse::Logger::Severity::Error, "Error: " + e.GetMessage());
        logger.Write(Curse::Logger::Severity::Info, "Closing Curse Editor.");
        return -1;
    }

    return 0;
}