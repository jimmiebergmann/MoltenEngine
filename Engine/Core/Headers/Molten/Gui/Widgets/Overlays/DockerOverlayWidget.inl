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

namespace Molten::Gui
{

    // Docker overlay implementations.
    template<typename TTheme>
    DockerOverlay<TTheme>::DockerOverlay(WidgetMixinDescriptor<TTheme, DockerOverlay>& desc) :
        WidgetMixin<TTheme, DockerOverlay>(desc)
    {}

    template<typename TTheme>
    void DockerOverlay<TTheme>::PostUpdate()
    {
        Vector2f32 newPosition;

        std::visit([&](const auto& element)
            {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, Position::Pixels>)
                {
                    newPosition.x = element.value;
                }
            }, this->position.x);

        std::visit([&](const auto& element)
            {
                using T = std::decay_t<decltype(element)>;
                if constexpr (std::is_same_v<T, Position::Pixels>)
                {
                    newPosition.y = element.value;
                }
            }, this->position.y);

        this->SetPosition(newPosition);
    }

}