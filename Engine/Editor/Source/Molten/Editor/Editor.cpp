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


#include "Molten/Editor/Editor.hpp"
#include "Molten/Logger.hpp"

#include "Molten/Graphics/Gui/CanvasRenderer.hpp"
#include "Molten/Graphics/Gui/Layers/SingleRootLayer.hpp"
#include "Molten/Graphics/Gui/Widgets/ButtonWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/GridWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/PaneWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/DockerWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/LabelWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/ViewportWidget.hpp"
#include "Molten/Graphics/Gui/Widgets/MenuBarWidget.hpp"

#include "Molten/Graphics/Gui2/Style.hpp"
#include "Molten/Graphics/Gui2/Widgets/ButtonWidget.hpp"

#include "Molten/EditorFramework/Project.hpp"
#include "Molten/EditorFramework/FileFormat/Mesh/ObjMeshFile.hpp"
#include "Molten/EditorFramework/FileFormat/Image/TgaImageFile.hpp"
#include "Molten/EditorFramework/FileFormat/Converter/ObjMeshToAssetConverter.hpp"
#include "Molten/EditorFramework/FileFormat/Converter/TgaImageToAssetConverter.hpp"
#include "Molten/EditorFramework/FileFormat/Converter/ObjMaterialToAssetConverter.hpp"
#include "Molten/EditorFramework/FileFormat/Validator/MaterialAssetFileValidator.hpp"

#include <random>
#include <set>

namespace Molten::Editor
{

    // Editor implementations.
    Editor::Editor(Semaphore& cancellationSemaphore) :
        m_isRunning(false),
        m_cancellationSemaphore(cancellationSemaphore),
        m_windowTitle("Molten Editor"),
        m_fpsTracker(8),
        m_threadPool(0, 2, 0)
    {}

    Editor::~Editor()
    {
        if(m_thread.joinable())
        {
            m_thread.join();
        }
    }

    bool Editor::Open(const EditorDescriptor& descriptor)
    {
        m_logger = descriptor.logger;

        Semaphore startedSemaphore;

        m_thread = std::thread([&]()
        {
            m_isRunning = true;
            const EditorDescriptor descriptorCopy = descriptor;

            startedSemaphore.NotifyOne();

            if (!Load(descriptorCopy))
            {
                Exit();
                return;
            }

            

            /*Clock tickTimer;
            while(m_isRunning)
            {
                m_fpsLimiter.Reset();               

                m_deltaTime = tickTimer.GetTime();
                m_fpsTracker.RegisterSampleFrame(m_deltaTime);
                tickTimer.Reset();

                if (!Tick() || !m_isRunning)
                {
                    Exit();
                    return;
                }

                m_fpsLimiter.PrecisionSleep();                
            }*/
        });

        startedSemaphore.Wait();
        return true;
    }

    bool Editor::Load(const EditorDescriptor& descriptor)
    {
        const auto projectDirectory = std::filesystem::path{ "my_project" };

        std::filesystem::remove_all(projectDirectory); // Temp

        if (!LoadProject(projectDirectory))
        {
            Logger::WriteInfo(m_logger.get(), "Could not load project, creating new project.");

            if (!CreateProject(projectDirectory))
            {
                Logger::WriteError(m_logger.get(), "Failed to create project.");
                return false;
            }

            if (!LoadProject(projectDirectory))
            {
                Logger::WriteError(m_logger.get(), "Failed to load project file after creation.");
                return false;
            }
        }



        //if (!LoadWindow(descriptor) ||
        //    !LoadRenderer(descriptor) ||
        //    !LoadRenderPasses() /*||
        //    !LoadGui()*/)
        //{
        //    return false;
        //}

        /*const auto windowUnfocusedSleepTime = Seconds(
            1.0 / (descriptor.windowUnfocusedFpsLimit.has_value() ?
            static_cast<double>(descriptor.windowUnfocusedFpsLimit.value()) : 15.0)
        );
        m_unfocusedWindowFpsLimiter.SetSleepTime(windowUnfocusedSleepTime);

        if(descriptor.fpsLimit.has_value()) 
        {
            const auto fpsLimit = static_cast<double>(descriptor.fpsLimit.value()) + 0.25;
            m_fpsLimiter.SetSleepTime(Seconds(1.0 / fpsLimit));
        }

        m_window->Show();*/
        return true;
    }

    bool Editor::LoadProject(const std::filesystem::path& filename)
    {
        auto openProjectResult = EditorFramework::Project::Open(filename);
        if (!openProjectResult)
        {
            Logger::WriteError(m_logger.get(), "Failed to open project.");
            return false;
        }

        return true;
    }

    bool Editor::CreateProject(const std::filesystem::path& directory)
    {
        auto createProjectResult = EditorFramework::Project::Create(directory);
        if (createProjectResult != EditorFramework::CreateProjectResult::Success)
        {
            Logger::WriteError(m_logger.get(), "Failed to create project.");
            return false;
        }

        const auto assetDirectory = directory / "Assets";

        // Read obj files.
        const auto objMeshFilename = std::filesystem::path{ "C:/temp/dino_obj/dino.obj" };
        const auto objMeshDirectory = objMeshFilename.parent_path();

        const auto objMeshFileResult = EditorFramework::ReadObjMeshFile(objMeshFilename);
        if (!objMeshFileResult)
        {
            Logger::WriteError(m_logger.get(), "Failed to read obj mesh file.");
            return false;
        }

        auto& objMeshFile = *objMeshFileResult;
        if (objMeshFile.objects.empty())
        {
            Logger::WriteError(m_logger.get(), "Obj mesh file is empty of objects.");
            return false;
        }

        auto objMaterialFiles = std::vector<EditorFramework::ObjMaterialFile>{};
        for (auto& materialFile : objMeshFileResult->materialFiles)
        {
            const auto objMeshMaterialFilename = objMeshDirectory / materialFile;
            auto objMeshMaterialResult = EditorFramework::ReadObjMaterialFile(objMeshMaterialFilename);
            if (!objMeshMaterialResult)
            {
                Logger::WriteError(m_logger.get(), "Failed to read obj material file.");
                return false;
            }

            objMaterialFiles.emplace_back(std::move(objMeshMaterialResult->file));
        }

        auto requiredObjMaterials = [&]() -> std::optional<EditorFramework::ObjMaterialFile::MaterialSharedPointers>
        {
            auto requiredNames = std::set<std::string>{};
            auto loadedMaterials = std::map<std::string, EditorFramework::ObjMaterialFile::MaterialSharedPointer>{};

            for (auto& object : objMeshFile.objects)
            {
                for (auto& group : object->groups)
                {
                    requiredNames.insert(group->material);
                }
            }

            for (auto& objMaterialFile : objMaterialFiles)
            {
                for (auto& objMaterial : objMaterialFile.materials)
                {
                    loadedMaterials.insert({ objMaterial->name, objMaterial });
                }
            }

            auto result = EditorFramework::ObjMaterialFile::MaterialSharedPointers{};

            for (const auto& requiredName : requiredNames)
            {
                auto it = loadedMaterials.find(requiredName);
                if (it == loadedMaterials.end())
                {
                    return std::nullopt;
                }

                result.push_back(it->second);
            }

            return std::make_optional(std::move(result));
        }();

        if (!requiredObjMaterials)
        {
            Logger::WriteError(m_logger.get(), "Failed to get required obj materials.");
            return false;
        }

        auto convertTextures = [&]() -> std::vector<EditorFramework::ConvertToMaterialAssetTexture>
        {
            auto textureFiles = std::set<std::string>{};
            auto result = std::vector<EditorFramework::ConvertToMaterialAssetTexture>{};

            auto addIfHasValue = [&](const std::optional<EditorFramework::ObjMaterialFile::MaterialTexture>& texture)
            {
                if (texture.has_value() && !texture.value().filename.empty())
                {
                    if (textureFiles.contains(texture.value().filename))
                    {
                        return;
                    }
                    textureFiles.emplace(texture.value().filename);
                    result.emplace_back(EditorFramework::ConvertToMaterialAssetTexture{
                        .materialTexture = &texture
                    });
                }
            };

            for (const auto& objMaterial : *requiredObjMaterials)
            {
                addIfHasValue(objMaterial->ambientTexture);
                addIfHasValue(objMaterial->diffuseTexture);
                addIfHasValue(objMaterial->specularTexture);
                addIfHasValue(objMaterial->specularExponentTexture);
                addIfHasValue(objMaterial->dissolveTexture);
                addIfHasValue(objMaterial->bumpTexture);
                addIfHasValue(objMaterial->displacementTexture);
                addIfHasValue(objMaterial->roughnessTexture);
                addIfHasValue(objMaterial->metallicTexture);
                addIfHasValue(objMaterial->emissiveTexture);
            }

            return result;
        }();


        // Read texture files.
        size_t textureIndex = 0;
        for (auto& convertTexture : convertTextures)
        {
            const auto& materialTexture = convertTexture.materialTexture->value();

            auto textureFilename = std::filesystem::path{ materialTexture.filename };
            if (!textureFilename.is_absolute())
            {
                textureFilename = objMeshDirectory / textureFilename;
            }

            if (!textureFilename.has_extension())
            {
                Logger::WriteError(m_logger.get(), "Missing file extension of material texture filename.");
                return false;
            }

            auto textureExtension = textureFilename.extension();
            if (textureExtension != ".tga")
            {
                Logger::WriteError(m_logger.get(), "Unsupported texture file format.");
                return false;
            }

            const auto tgaFileResult = EditorFramework::ReadTgaImageFile(textureFilename);
            if (!tgaFileResult)
            {
                Logger::WriteError(m_logger.get(), "Failed to read tga image file.");
                return false;
            }

            // Convert to asset file.
            const auto tgaImageConvertResult = EditorFramework::ConvertToTextureAsset(
                *tgaFileResult,
                m_randomUuidGenerator());

            if (!tgaImageConvertResult)
            {
                Logger::WriteError(m_logger.get(), "Failed to convert tga image to texture asset.");
                return false;
            }
            const auto textureAssetFile = *tgaImageConvertResult;

            // Write asset file.
            auto textureAssetPath = assetDirectory / (std::string{"texture_"} + std::to_string(textureIndex++) + ".asset");
            auto writeTextureAssetResult = EditorFramework::WriteTextureAssetFile(textureAssetPath, textureAssetFile);
            if (!writeTextureAssetResult)
            {
                Logger::WriteError(m_logger.get(), "Failed to write texture asset.");
                return false;
            }

            convertTexture.assetGlobalid = textureAssetFile.globalId;
        }

        // Convert to asset files.
        const auto objMeshConvertResult = EditorFramework::ConvertToMeshAssetFile(
            *objMeshFile.objects.front(),
            m_randomUuidGenerator());

        if (!objMeshConvertResult)
        {
            Logger::WriteError(m_logger.get(), "Failed to convert obj mesh to mesh asset.");
            return false;
        }

        const auto convertToMaterialAssetFileOptions = EditorFramework::ConvertToMaterialAssetFileOptions
        {
            std::move(convertTextures)
        };

        auto materialAssetFiles = std::vector<EditorFramework::MaterialAssetFile>{};
        for (auto& objMaterial : requiredObjMaterials.value())
        {
            auto objMaterialConvertResult = EditorFramework::ConvertToMaterialAssetFile(
                *objMaterial,
                m_randomUuidGenerator(),
                convertToMaterialAssetFileOptions);

            if (!objMaterialConvertResult)
            {
                Logger::WriteError(m_logger.get(), "Failed to convert obj material to material asset.");
                return false;
            }

            materialAssetFiles.emplace_back(std::move(*objMaterialConvertResult));
        }

        // Write asset files.
        auto meshAssetPath = assetDirectory / "mesh.asset";
        auto writeMeshAssetResult = EditorFramework::WriteMeshAssetFile(meshAssetPath, *objMeshConvertResult);
        if (!writeMeshAssetResult)
        {
            Logger::WriteError(m_logger.get(), "Failed to write mesh asset.");
            return false;
        }

        auto materialAssetPaths = std::vector<std::filesystem::path>{};
        for (auto& materialAssetFile : materialAssetFiles)
        {
            auto materialAssetPath = assetDirectory / (std::string{"material_"} + std::to_string(materialAssetPaths.size()) + ".asset");
            auto writeMaterialAssetResult = EditorFramework::WriteMaterialAssetFile(materialAssetPath, materialAssetFile);
            if (!writeMaterialAssetResult)
            {
                Logger::WriteError(m_logger.get(), "Failed to write material asset.");
                return false;
            }

            materialAssetPaths.emplace_back(materialAssetPath);
        }

        // Read files.
        // TODO: Remove, no need to read...
        auto readMeshAssetResult = EditorFramework::ReadMeshAssetFile(meshAssetPath);
        if (!readMeshAssetResult)
        {
            Logger::WriteError(m_logger.get(), "Failed to read mesh asset.");
            return false;
        }

        for (auto& materialAssetPath : materialAssetPaths)
        {
            auto readMaterialAssetResult = EditorFramework::ReadMaterialAssetFile(materialAssetPath);
            if (!readMaterialAssetResult)
            {
                Logger::WriteError(m_logger.get(), "Failed to read material asset.");
                return false;
            }

            auto validateMaterialAssetResult = EditorFramework::ValidateMaterialAssetFile(*readMaterialAssetResult);
            if (!validateMaterialAssetResult)
            {
                Logger::WriteError(m_logger.get(), "Failed to validate material asset.");
                return false;
            }

        }

        return true;
    }

    //bool Editor::LoadWindow(const EditorDescriptor&)
    //{
    //    WindowDescriptor windowDescriptor;
    //    windowDescriptor.size = { 1600, 1200 };
    //    windowDescriptor.title = m_windowTitle;
    //    windowDescriptor.enableDragAndDrop = true;
    //    windowDescriptor.logger = m_logger.get();

    //    m_window = Window::Create(windowDescriptor);
    //    if (!m_window || !m_window->IsOpen())
    //    {
    //        Logger::WriteError(m_logger.get(), "Failed to create editor window.");
    //        return false;
    //    }

    //    m_window->OnResize.Connect([&](Vector2ui32)
    //    {
    //        Tick();
    //    });

    //    m_window->OnDpiChange.Connect([&](Vector2ui32 dpi)
    //    {
    //        m_logger->Write(Logger::Severity::Info, "Changed DPI: " + std::to_string(dpi.x) + ", " + std::to_string(dpi.y));
    //    });

    //    m_window->OnScaleChange.Connect([&](Vector2f32 scale)
    //    {
    //        m_logger->Write(Logger::Severity::Info, "Changed scale: " + std::to_string(scale.x) + ", " + std::to_string(scale.y));
    //    });

    //    m_window->OnFilesDropEnter = [&](const std::vector<std::filesystem::path>& files) -> bool
    //    {
    //        return ValidateFileDrops(files);
    //    };
    //    m_window->OnFilesDrop = [&](const std::vector<std::filesystem::path>& files)
    //    {          
    //        return ProcessFileDrops(files);
    //    };

    //    return true;
    //}

    //bool Editor::LoadRenderer(const EditorDescriptor& descriptor)
    //{
    //    auto* logger = descriptor.enableGpuLogging ? m_logger.get() : nullptr;

    //    const auto api = descriptor.backendRendererApi.has_value() ? 
    //        descriptor.backendRendererApi.value() : Renderer::BackendApi::Vulkan;

    //    const auto apiVersion = descriptor.backendRendererApiVersion.has_value() ?
    //        descriptor.backendRendererApiVersion.value() : Version(1, 0);

    //    const auto rendererDesc = RendererDescriptor{ *m_window, apiVersion, logger };

    //    m_renderer = Renderer::Create(api, rendererDesc);
    //    if (!m_renderer)
    //    {
    //        Logger::WriteError(m_logger.get(), "Failed to create renderer.");
    //        return false;
    //    }

    //    return true;
    //}

    //bool Editor::LoadRenderPasses()
    //{
    //    /*auto renderPass = m_renderer->GetSwapChainRenderPass();
    //    renderPass->SetRecordFunction([&](auto& commandBuffer) {
    //       m_canvasRenderer->SetCommandBuffer(commandBuffer);
    //        //m_rootView->canvasRenderer->SetCommandBuffer(commandBuffer);
    //        //m_rootView->canvas->Draw();

    //       m_canvasRenderer->SetCommandBuffer(commandBuffer);
    //       m_canvas->Draw();
    //    });*/

    //    return true;
    //}

    /*bool Editor::LoadGui()
    {
        const auto canvasRendererDesc = Gui2::CanvasRendererDescriptor{
            .backendRenderer = *m_renderer.get(),
            .logger = m_logger.get()
        };

        m_canvasRenderer = Gui2::CanvasRenderer::Create(canvasRendererDesc);
        if (!m_canvasRenderer)
        {
            return false;
        }

        auto style = std::make_shared<Gui2::Style>();

        m_canvas = std::make_unique<Gui2::Canvas>(m_canvasRenderer, style);

        [[maybe_unused]] auto button1 = m_canvas->CreateChild<Gui2::Button>();

        auto button2Style = std::make_shared<Gui2::ButtonStyle>();
        button2Style->colors.normal = { 0.0f, 1.0f, 0.0f, 1.0f };

        [[maybe_unused]] auto button2 = button1->CreateChild<Gui2::Button>(button2Style);

        auto button3Style = std::make_shared<Gui2::ButtonStyle>();
        button3Style->colors.normal = { 0.0f, 0.0f, 1.0f, 1.0f };

        [[maybe_unused]] auto button3 = button2->CreateChild<Gui2::Button>(button3Style);

        return true;
    }*/

   /* bool Editor::LoadGuiViews()
    {
        m_fontNameRepository.AddSystemDirectories();

        const auto rootViewDesc = RootViewDescriptor{
           .renderer = *m_renderer,
           .fontNameRepository = m_fontNameRepository,
           .logger = m_logger.get()
        };

        if (m_rootView = RootView::Create(rootViewDesc); !m_rootView)
        {
            return false;
        }

        const auto sceneViewDesc = SceneViewDescriptor{
            .renderer = *m_renderer,
            .rootWidget = *m_rootView->pageView,
            .deltaTime = m_deltaTime,
            .logger = m_logger.get()
        };

        if (m_sceneView = SceneView::Create(sceneViewDesc); !m_sceneView)
        {
            return false;
        }

        return true;
    }

    bool Editor::LoadGuiSignals()
    {
        m_sceneView->dockerWidget->onCursorChange.Connect([&](Mouse::Cursor cursor) {
            m_window->SetCursor(cursor);
        });

        m_sceneView->viewportWidget->onResize.Connect([&](const auto size) {
            m_sceneView->sceneViewport->Resize(size);
        });
        m_sceneView->viewportWidget->onIsVisible.Connect([&]() {
            if (m_sceneView->sceneViewport->renderPass)
            {
                m_renderPasses.push_back(m_sceneView->sceneViewport->renderPass);
            }
        });

        return true;
    }*/

    void Editor::Exit()
    {
        if (m_renderer)
        {
            m_renderer->WaitForDevice();
        }

        m_isRunning = false;
        m_cancellationSemaphore.NotifyAll();
    }

    bool Editor::Tick()
    {
        //m_preUpdateCallbacks.Dispatch();

        if(!UpdateWindow())
        {
            return false;
        }
        if(!HandleWindowFocus())
        {
            return true;
        }

        m_renderPasses.clear();

        //UpdateCanvas();

        //m_postUpdateCallbacks.Dispatch();

        m_renderPasses.push_back(m_renderer->GetSwapChainRenderPass());

        if(!m_renderer->DrawFrame(m_renderPasses))
        {
            return false;
        }

        return true;
    }

    bool Editor::UpdateWindow()
    {
        if(m_windowTitleUpdateClock.GetTime() >= Seconds(1.0f))
        {
            const auto averageFps = static_cast<int64_t>(1.0 / m_fpsTracker.GetAverageFrameTime().AsSeconds<double>());
            const auto minFps = static_cast<int64_t>(1.0 / m_fpsTracker.GetMaxFrameTime().AsSeconds<double>());
            const auto maxFps = static_cast<int64_t>(1.0 / m_fpsTracker.GetMinFrameTime().AsSeconds<double>());

            m_window->SetTitle(
                m_windowTitle + " - avg FPS: " + std::to_string(averageFps) + 
                " | min FPS: " + std::to_string(minFps) +
                " | max FPS: " + std::to_string(maxFps));

            /*m_avgFpsLabel->text.Set("Avg FPS : " + std::to_string(averageFps));
            m_minFpsLabel->text.Set("Min FPS : " + std::to_string(minFps));
            m_maxFpsLabel->text.Set("Max FPS : " + std::to_string(maxFps));*/

            m_fpsTracker.ResetFrameSamples();

            m_windowTitleUpdateClock.Reset();
        }
       
        m_window->Update();
        if (!m_window->IsOpen())
        {
            return false;
        }
        return true;
    }

    bool Editor::HandleWindowFocus()
    {
        const auto windowSize = m_window->GetSize();
        const bool isMinimized = !windowSize.x || !windowSize.y;
        if(isMinimized || !m_window->IsFocused())
        {
            m_unfocusedWindowFpsLimiter.Sleep();
            m_unfocusedWindowFpsLimiter.Reset();
            return !isMinimized;
        }

        return true;
    }

    /*void Editor::UpdateCanvas()
    {
        auto& userInput = m_window->GetUserInput();
        //auto& canvas = *m_rootView->canvas;

        UserInput::Event inputEvent;
        while (userInput.PollEvent(inputEvent))
        {
            m_canvas->PushUserInputEvent(inputEvent);
        }

        m_canvas->SetSize(m_window->GetSize());
        m_canvas->SetScale(m_window->GetScale());
        m_canvas->Update();
    }*/

    /*void Editor::UpdateCanvas()
    {
       auto& userInput = m_window->GetUserInput();
        auto& canvas = *m_rootView->canvas;

        UserInput::Event inputEvent;
        while (userInput.PollEvent(inputEvent))
        {
            canvas.PushUserInputEvent(inputEvent);
        }

        canvas.SetSize(m_window->GetSize());
        canvas.SetScale(m_window->GetScale());
        canvas.Update(m_deltaTime);
    }*/

    bool Editor::ValidateFileDrops(const std::vector<std::filesystem::path>& files)
    {
        if (files.size() != 1)
        {
            return false;
        }

        const auto& file = files.front();
        if (file.extension() != ".obj")
        {
            return false;
        }

        Logger::WriteInfo(m_logger.get(), "Dragging obj file: " + file.string());

        return true;
    }

    bool Editor::ProcessFileDrops(const std::vector<std::filesystem::path>& files)
    {
        if (files.size() != 1)
        {
            return false;
        }

        /*const auto& file = files.front();
        Logger::WriteInfo(m_logger.get(), "Dropping obj file: " + file.string());

        [[maybe_unused]] auto dummy = m_threadPool.Execute([&, filename = file]() {
            ObjMeshFile objFile;
            ObjMeshFileReader objFileReader;

            m_preUpdateCallbacks.Add([&]() {
                m_loadingProgressBar->value = 0.0;
            });

            auto onProgressConnection = objFileReader.onProgress.Connect([&](const double progress) {
                m_postUpdateCallbacks.Add([this, progress]() {
                    m_loadingProgressBar->value = progress;
                });
            });

            const Clock clock;
            const auto result = objFileReader.ReadFromFile(objFile, filename, m_threadPool);
            const auto readTime = clock.GetTime();

            onProgressConnection.Disconnect();

            if (!result.IsSuccessful())
            {
                Logger::WriteError(m_logger.get(), "Model loading failed:" + result.GetError().message);
                return;
            }

            Logger::WriteInfo(m_logger.get(), "Read file in: " + std::to_string(readTime.AsSeconds<float>()) + "s.");
            Logger::WriteInfo(m_logger.get(), "Objects: " + std::to_string(objFile.objects.size()));

            m_postUpdateCallbacks.Add([&]() {
                Logger::WriteInfo(m_logger.get(), "Model successfully loaded!");
            });
        });*/

        return true;
    }

}
