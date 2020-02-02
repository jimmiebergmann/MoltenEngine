#include "Curse/Window/Window.hpp"
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

static void LoadShaders(Curse::Shader::VertexScript& vScript, Curse::Shader::FragmentScript& fScript)
{
    // Vertex shader.
    {
        auto outPos = vScript.GetVertexOutputNode();
        auto outColor = vScript.CreateVaryingOutNode<Curse::Vector4f32>();
        auto pos = vScript.CreateVaryingInNode<Curse::Vector3f32>();
        auto color = vScript.CreateVaryingInNode<Curse::Vector4f32>();

        outPos->GetInputPin()->Connect(*pos->GetOutputPin());
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

static void Run()
{
    window = Curse::Window::Create();
    if (!window->Open("Curse Editor", { 800, 600 }, &logger))
    {
        return;
    }

    auto renderer = Curse::Renderer::Create(Curse::Renderer::BackendApi::Vulkan);
    renderer->Open(*window, Curse::Version(1, 1), &logger);

    Curse::Shader::VertexScript vertexScript;
    Curse::Shader::FragmentScript fragmentScript;
    LoadShaders(vertexScript, fragmentScript);

    auto vertSource = vertexScript.GenerateGlsl();
    auto fragSource = fragmentScript.GenerateGlsl();

    auto verSpirv = renderer->CompileShaderProgram(Curse::ShaderFormat::Glsl, Curse::ShaderType::Vertex, vertSource, Curse::ShaderFormat::SpirV);
    auto fragSpirv = renderer->CompileShaderProgram(Curse::ShaderFormat::Glsl, Curse::ShaderType::Fragment, fragSource, Curse::ShaderFormat::SpirV);
    
    Curse::Shader::Program* vertexShader = renderer->CreateShaderProgram({ Curse::ShaderType::Vertex, verSpirv.data(), verSpirv.size() });
    Curse::Shader::Program* fragmentShader = renderer->CreateShaderProgram({ Curse::ShaderType::Fragment, fragSpirv.data(), fragSpirv.size() });

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

    Curse::PipelineDescriptor pipelineDesc;
    pipelineDesc.topology = Curse::Pipeline::Topology::TriangleList;
    pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Fill;
    pipelineDesc.frontFace = Curse::Pipeline::FrontFace::Clockwise;
    pipelineDesc.cullMode = Curse::Pipeline::CullMode::Back;
    pipelineDesc.shaderPrograms = { vertexShader, fragmentShader };
    pipelineDesc.vertexBindings = { vertexBinding };

    Curse::Pipeline* pipeline = renderer->CreatePipeline(pipelineDesc);
    
    auto renderFunction = [&]()
    {      
        renderer->Resize(window->GetSize());
        renderer->BeginDraw();

        renderer->BindPipeline(pipeline);
        renderer->DrawVertexBuffers(indexBuffer, vertexBuffer, 1);

        renderer->EndDraw();
    };

    //Curse::Clock resizeTimer;
    auto resizeCallback = [&](Curse::Vector2ui32 )
    {
        static Curse::Clock resizeTimer;
        if (resizeTimer.GetTime() >= Curse::Seconds(0.1f))
        {
            resizeTimer.Reset();
            renderFunction();
        }
    };

    window->OnMaximize.Connect(resizeCallback);
    window->OnMinimize.Connect(resizeCallback);
    window->OnResize.Connect(resizeCallback);
   
    window->Show();

    //Curse::Vector2f32 windowPos = window->GetPosition();

    //size_t ticks = 0;
    Curse::Clock deltaClock;
    Curse::Clock runClock;
    //float deltaTime = 0.0f;
    while (window->IsOpen())
    {
        //deltaTime = deltaClock.GetTime().AsSeconds<float>();
        deltaClock.Reset();
            
        /*const float resizeSpeed = 2.0f;
        const float resizeMagniture = 200.0f;
        float resize = (std::sin(runClock.GetTime().AsSeconds<float>() * resizeSpeed) + 1.0f) / 2.0f * resizeMagniture;
        auto newSize = Curse::Vector2f32{ 400.0f + resize, 400.0f + resize };
        auto newPos = Curse::Vector2f32{ 400.0f - (resize / 2.0f), 400.0f - (resize/2.0f) };
        window->Move(newPos);*/
        //window->Resize(newSize);


        //logger.Write(Curse::Logger::Severity::Debug, std::to_string(resize));

        //window->Move(windowPos);
        //window->Resize({300, 300});

        window->Update();
        if (!window->IsOpen())
        {
            break;
        }

        renderFunction();
    }

    renderer->WaitForDevice();
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