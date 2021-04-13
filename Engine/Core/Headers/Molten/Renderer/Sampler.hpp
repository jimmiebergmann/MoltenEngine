/*
* MIT License
*
* Copyright (c) 2021 Jimmie Bergmann
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

#ifndef MOLTEN_CORE_RENDERER_SAMPLER_HPP
#define MOLTEN_CORE_RENDERER_SAMPLER_HPP

#include "Molten/Math/Vector.hpp"

namespace Molten
{

    /** Enumerator of sampler wrap modes. */
    enum class SamplerWrapMode : uint8_t
    {
        Repeat,
        RepeatMirror,
        Clamp
    };

    /** Enumerator of sampler filtering types. */
    enum class SamplerFilter : uint8_t
    {
        Nearest,
        Linear
    };

    /** Sampler resource object. */
    template<size_t VDimensions>
    class Sampler
    {

        static_assert(VDimensions >= 1 && VDimensions <= 3, "Sampler must be of dimension 1-3.");

    public:

        Sampler() = default;
        virtual ~Sampler() = default;

        /* Deleted copy and move operations. */
        /**@{*/
        Sampler(const Sampler&) = delete;
        Sampler(Sampler&&) = delete;
        Sampler& operator =(const Sampler&) = delete;
        Sampler& operator =(Sampler&&) = delete;
        /**@}*/

    };

    using Sampler1D = Sampler<1>;
    using Sampler2D = Sampler<2>;
    using Sampler3D = Sampler<3>;


    /**
    * @brief Descriptor of sampler.
    */
    template<size_t VDimensions>
    struct SamplerDescriptor
    {

        static_assert(VDimensions >= 1 && VDimensions <= 3, "Sampler descriptor must be of dimension 1-3.");

        using SamplerWrapModes = Vector<VDimensions, SamplerWrapMode>;

        SamplerDescriptor();
        SamplerDescriptor(
            const SamplerWrapModes wrapModes,
            const SamplerFilter magFilter,
            const SamplerFilter minFilter,
            const uint32_t maxAnisotropy);
        ~SamplerDescriptor() = default;

        SamplerDescriptor(const SamplerDescriptor&) = default;
        SamplerDescriptor(SamplerDescriptor&&) = default;
        SamplerDescriptor& operator =(const SamplerDescriptor&) = default;
        SamplerDescriptor& operator =(SamplerDescriptor&&) = default;

        SamplerWrapModes wrapModes;
        SamplerFilter magFilter;
        SamplerFilter minFilter;
        uint32_t maxAnisotropy;

    };

    using SamplerDescriptor1D = SamplerDescriptor<1>;
    using SamplerDescriptor2D = SamplerDescriptor<2>;
    using SamplerDescriptor3D = SamplerDescriptor<3>;

}

#include "Molten/Renderer/Sampler.inl"

#endif
