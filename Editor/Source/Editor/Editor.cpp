#include "Curse/Window/Window.hpp"
#include "Curse/Renderer/Renderer.hpp"
#include "Curse/System/Exception.hpp"
#include "Curse/Memory/Pointer.hpp"
#include "Curse/System/Mouse.hpp"
#include <iostream>

static int Run()
{
    auto window = Curse::Window::Create("Curse Editor", Curse::Vector2ui32(800, 600));
    
    auto debugPrinter = [](const std::string & message)
    {
        std::cout << "[Debug] Renderer: " << message << std::endl;
    };

    auto renderer = Curse::Renderer::Create(Curse::Renderer::BackendApi::Vulkan);
    renderer->Open(*window, Curse::Version(1, 1), debugPrinter);
    
    Curse::Texture* texture = renderer->CreateTexture();
    Curse::Shader* vertexShader = renderer->CreateShader({ Curse::Shader::Type::Vertex, "vert.spv" });
    Curse::Shader* fragmentShader = renderer->CreateShader({ Curse::Shader::Type::Fragment, "frag.spv" });

    Curse::PipelineDescriptor pipelineDesc;
    pipelineDesc.shaders.push_back(vertexShader);
    pipelineDesc.shaders.push_back(fragmentShader);
    pipelineDesc.topology = Curse::Pipeline::Topology::TriangleList;
    pipelineDesc.polygonMode = Curse::Pipeline::PolygonMode::Fill;
    pipelineDesc.frontFace = Curse::Pipeline::FrontFace::Clockwise;
    pipelineDesc.cullMode = Curse::Pipeline::CullMode::Back;
    Curse::Pipeline* pipeline = renderer->CreatePipeline(pipelineDesc);
    
    window->Show();
    while (window->IsOpen())
    {
        window->Update();
        renderer->SwapBuffers();
    }

    renderer->DestroyTexture(texture);
    renderer->DestroyShader(vertexShader);
    renderer->DestroyShader(fragmentShader);
    renderer->DestroyPipeline(pipeline);

    return 0;
}

int main()
{
    std::cout << "Curse Editor v" << Curse::Version(CURSE_VERSION_MAJOR, CURSE_VERSION_MINOR, CURSE_VERSION_PATCH).AsString(false) << std::endl;

    try
    {
        return Run();
    }
    catch (Curse::Exception & e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return -1;
}