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

#ifndef MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERPUSHCONSTANT_HPP
#define MOLTEN_CORE_RENDERER_SHADER_VISUAL_VISUALSHADERPUSHCONSTANT_HPP

#include "Molten/Renderer/Shader/Visual/VisualShaderStructure.hpp"
#include <set>

namespace Molten::Shader::Visual
{


    /** 
     * Meta data related to push constant interface members.
     * The only available meta data for push constant members is IDs.
     */
    class MOLTEN_API PushConstantMetaData
    {

    public:

        explicit PushConstantMetaData(const uint32_t id);

        uint32_t GetId() const;

    private:

        /** Copy and move operations are not allowed. */
        /**@{*/
        PushConstantMetaData(const PushConstantMetaData& copy) = delete;
        PushConstantMetaData(const PushConstantMetaData&& move) = delete;
        PushConstantMetaData& operator =(const PushConstantMetaData& copy) = delete;
        PushConstantMetaData& operator =(const PushConstantMetaData&& move) = delete;
        /**@*/

        const uint32_t m_id;

    };


    /** Push constant interface type. */
    using PushConstantInterface = Structure<InputVariable, PushConstantMetaData>;

}

#endif