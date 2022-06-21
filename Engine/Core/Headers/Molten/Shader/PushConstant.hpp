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

#ifndef MOLTEN_CORE_SHADER_PUSHCONSTANT_HPP
#define MOLTEN_CORE_SHADER_PUSHCONSTANT_HPP

#include "Molten/Shader/Shader.hpp"
#include <map>

namespace Molten
{

    struct MOLTEN_CORE_API PushConstantLocation
    {

        static constexpr uint32_t UnknownId = std::numeric_limits<uint32_t>::max();
        static constexpr uint32_t UnknownLocation = std::numeric_limits<uint32_t>::max();

        PushConstantLocation();
        PushConstantLocation(
            const uint32_t id,
            const uint32_t location,
            const Shader::VariableDataType dataType);
        ~PushConstantLocation() = default;

        PushConstantLocation(const PushConstantLocation&) = default;
        PushConstantLocation(PushConstantLocation&&) = default;
        PushConstantLocation& operator =(const PushConstantLocation&) = default;
        PushConstantLocation& operator =(PushConstantLocation&&) = default;

        uint32_t id;
        uint32_t location;
        Shader::VariableDataType dataType;

    };

    using PushConstantLocations = std::map<uint32_t, PushConstantLocation>;

}

#endif
