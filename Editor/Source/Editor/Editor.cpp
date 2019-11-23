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
    Curse::Texture* texture = nullptr;
    //Curse::Shader* vertexShader = nullptr;
    //Curse::Shader* fragmentShader = nullptr;
    try
    {
        renderer->Open(*window, Curse::Version(1, 1), debugPrinter);
        texture = renderer->CreateTexture();
        //vertexShader = renderer->CreateShader();
        //fragmentShader = renderer->CreateShader(Curse::ShaderDescriptor(Curse::Shader::Type::Fragment, ""));
    }
    catch (Curse::Exception & e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return -1;
    }

    window->Show();
    while (window->IsOpen())
    {
        window->Update();
        renderer->SwapBuffers();
    }

    renderer->DeleteTexture(texture);

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
        std::cout << e.what() << std::endl;
    }

    return -1;
}