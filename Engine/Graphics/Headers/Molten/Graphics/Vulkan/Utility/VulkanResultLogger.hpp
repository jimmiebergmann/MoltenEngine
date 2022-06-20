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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANRESULTLOGGER_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANRESULTLOGGER_HPP

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/Utility/VulkanResult.hpp"
#include "Molten/Logger.hpp"

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{

    class Logger
    {

    public:

        /** Write result and optional message. */
        /**@{*/
        static void Write(
            Molten::Logger* logger,
            Molten::Logger::Severity severity,
            const VkResult result,
            const std::string& message = "");

        template<typename TResultType, template<typename> typename TResult>
        static void Write(
            Molten::Logger* logger,
            Molten::Logger::Severity severity,
            const TResult<TResultType>& result,
            const std::string& message = "");

        template<typename TResultType, template<typename, typename> typename TResult>
        static void Write(
            Molten::Logger* logger,
            Molten::Logger::Severity severity,
            TResult<VkResult, TResultType>& result,
            const std::string& message = "");
        /**@}*/

        /** Write result and optional message as info. */
        /**@{*/
        static void WriteInfo(
            Molten::Logger* logger,
            const VkResult result,
            const std::string& message = "");

        template<typename TResultType, template<typename> typename TResult>
        static void WriteInfo(
            Molten::Logger* logger,
            const TResult<TResultType>& result,
            const std::string& message = "");

        template<typename TResultType, template<typename, typename> typename TResult>
        static void WriteInfo(
            Molten::Logger* logger,
            TResult<VkResult, TResultType>& result,
            const std::string& message = "");
        /**@}*/

        /** Write result and optional message as error. */
        /**@{*/
        static void WriteDebug(
            Molten::Logger* logger,
            const VkResult result,
            const std::string& message = "");

        template<typename TResultType, template<typename> typename TResult>
        static void WriteDebug(
            Molten::Logger* logger,
            const TResult<TResultType>& result,
            const std::string& message = "");

        template<typename TResultType, template<typename, typename> typename TResult>
        static void WriteDebug(
            Molten::Logger* logger,
            TResult<VkResult, TResultType>& result,
            const std::string& message = "");
        /**@}*/

        /** Write result and optional message as warning. */
        /**@{*/
        static void WriteWarning(
            Molten::Logger* logger,
            const VkResult result,
            const std::string& message = "");

        template<typename TResultType, template<typename> typename TResult>
        static void WriteWarning(
            Molten::Logger* logger,
            const TResult<TResultType>& result,
            const std::string& message = "");

        template<typename TResultType, template<typename, typename> typename TResult>
        static void WriteWarning(
            Molten::Logger* logger,
            TResult<VkResult, TResultType>& result,
            const std::string& message = "");
        /**@}*/

        /** Write result and optional message as error. */
        /**@{*/
        static void WriteError(
            Molten::Logger* logger,
            const VkResult result,
            const std::string& message = "");

        template<typename TResultType, template<typename> typename TResult>
        static void WriteError(
            Molten::Logger* logger,
            const TResult<TResultType>& result,
            const std::string& message = "");

        template<typename TResultType, template<typename, typename> typename TResult>
        static void WriteError(
            Molten::Logger* logger,
            TResult<VkResult, TResultType>& result,
            const std::string& message = "");
        /**@}*/

    private:

        static std::string CreateMessage(
            const ResultTypeInfo& typeInfo,
            const std::string& message);

    };

}

#include "Molten/Renderer/Vulkan/Utility/VulkanResultLogger.inl"

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif