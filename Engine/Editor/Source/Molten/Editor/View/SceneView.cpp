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


#include "Molten/Editor/View/SceneView.hpp"
#include "Molten/Graphics/Gui/Canvas.hpp"
#include "Molten/Graphics/Gui/Layer.hpp"

namespace Molten::Editor
{

	std::unique_ptr<SceneView> SceneView::Create(const SceneViewDescriptor& descriptor)
	{
		auto result = std::unique_ptr<SceneView>(new SceneView{});

		auto* docker = descriptor.rootWidget.CreateChild<Gui::Docker>();
		result->dockerWidget = docker;

		docker->margin = { 4.0f, 4.0f, 4.0f, 4.0f };

		//docker->onCursorChange.Connect([&](Mouse::Cursor cursor) {
		//	//m_window->SetCursor(cursor);
		//});

		result->viewportWidget = docker->CreateChild<Gui::Viewport>(Gui::DockingPosition::Right);
		/*sceneViewport->onResize.Connect([&, viewport = sceneViewport](const auto size) {
			//OnSceneViewportResize(viewport, size);
		});
		sceneViewport->onIsVisible.Connect([&]() {
			//m_renderPasses.push_back(m_viewportRenderPass);
		});*/

		docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Bottom, "Assets")->size = { Gui::Size::Pixels{ 250.0f }, Gui::Size::Pixels{ 300.0f } };

		auto inspector = docker->CreateChild<Gui::Pane>(Gui::DockingPosition::Right, "Inspector");
		inspector->size = { Gui::Size::Pixels{ 350.0f }, Gui::Size::Pixels{ 200.0f } };
		inspector->padding = { 3.0f, 3.0f, 3.0f, 3.0f };

		auto vertGrid = inspector->CreateChild<Gui::Grid>(Gui::GridDirection::Vertical);
		vertGrid->CreateChild<Gui::Label>("Location:", 18)->position = { Gui::Position::Fixed::Center, Gui::Position::Fixed::Center };

		auto button = vertGrid->CreateChild<Gui::Button>();
		button->size.x = Gui::Size::Fit::Content;
		button->size.y = Gui::Size::Fit::Content;
		button->onPress.Connect([&](auto) {
			//Logger::WriteInfo(m_logger.get(), "You pressed me!");
			});
		button->CreateChild<Gui::Label>("Click me!", 18);

		/*m_avgFpsLabel = vertGrid->CreateChild<Gui::Label>("", 18);
		m_minFpsLabel = vertGrid->CreateChild<Gui::Label>("", 18);
		m_maxFpsLabel = vertGrid->CreateChild<Gui::Label>("", 18);

		m_loadingProgressBar = vertGrid->CreateChild<Gui::ProgressBar>();
		m_loadingProgressBar->position = { Gui::Position::Pixels{ 0.0f }, Gui::Position::Pixels{ 100.0f } };*/

		const auto sceneViewportDesc = SceneViewportDescriptor{
			.renderer = descriptor.renderer,
			.viewportWidget = *result->viewportWidget,
			.deltaTime = descriptor.deltaTime,
			.logger = descriptor.logger
		};

		if (result->sceneViewport = SceneViewport::Create(sceneViewportDesc); !result->sceneViewport)
		{
			return {};
		}

        return result;
    }

}
