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

#ifndef MOLTEN_CORE_RENDERER_SHADER_SHADERPROGRAM_HPP
#define MOLTEN_CORE_RENDERER_SHADER_SHADERPROGRAM_HPP

#include "Molten/Renderer/Resource.hpp"
#include "Molten/Renderer/Shader.hpp"
#include <string>
#include <vector>

namespace Molten::Shader
{


    /**
    * @brief Shader stage base class.
    */
    class MOLTEN_API Stage
    {

    public:

        /**
        * @brief Get the type of shader stage.
        */
        virtual Type GetType() const = 0;


    protected:

        /**
        * @brief Virtual destructor.
        */
        virtual ~Stage() = default;

    };


    /**
    * @brief Vertex shader stage class.
    */
    class MOLTEN_API VertexStage : public Stage
    {

    public:

        /**
        * @brief Get the type of shader stage. 
        */
        virtual Type GetType() const override;


    protected:

        VertexStage() = default;
        virtual ~VertexStage() = default;

    private:

        VertexStage(const VertexStage&) = delete;
        VertexStage(VertexStage&&) = delete;
            
    };


    /**
    * @brief Fragment shader stage class.
    */
    class MOLTEN_API FragmentStage : public Stage
    {

    public:

        /**
        * @brief Get the type of shader.
        */
        virtual Type GetType() const override;

    protected:

        FragmentStage() = default;
        virtual ~FragmentStage() = default;

    private:

        FragmentStage(const FragmentStage&) = delete;
        FragmentStage(FragmentStage&&) = delete;    

    };

}

#endif
