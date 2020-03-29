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

#ifndef CURSE_CORE_ECS_ECSSIGNATURE_HPP
#define CURSE_CORE_ECS_ECSSIGNATURE_HPP

#include "Curse/Ecs/Ecs.hpp"
#include "Curse/Utility/Bitfield.hpp"

namespace Curse
{

    namespace Ecs
    {

        using Signature = Curse::Bitfield<CURSE_ECS_MAX_COMPONENT_TYPES>;


        /**
        * @brief Construct a signature from multiple component types.
        */
        template<typename ... Components>
        Signature CreateSignature();


        /**
        * @brief Static declaration of signature footprint of multiple component types.
        */
        template<typename ... Components>
        struct ComponentSignature
        {

            static inline const Signature signature = CreateSignature<Components...>();

        };

    }

}

#include "Curse/Ecs/EcsSignature.inl"

#endif