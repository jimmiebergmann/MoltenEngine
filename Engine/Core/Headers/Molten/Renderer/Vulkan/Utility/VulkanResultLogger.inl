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

namespace Molten::Vulkan
{

    template<typename TResultType, template<typename> typename TResult>
    void Logger::Write(
        Molten::Logger* logger,
        Molten::Logger::Severity severity,
        const TResult<TResultType>& result,
        const std::string& message)
    {
        if (!logger)
        {
            return;
        }

        auto typeInfo = ResultMapper<TResultType>::GetInfo(result.Get<0>());
        auto msg = CreateMessage(typeInfo, message);
        logger->Write(severity, msg);
    }

    template<typename TResultType, template<typename, typename> typename TResult>
    void Logger::Write(
        Molten::Logger* logger,
        Molten::Logger::Severity severity,
        TResult<VkResult, TResultType>& result,
        const std::string& message)
    {
        if (!logger)
        {
            return;
        }

        if (result.index() == 0)
        {
            auto typeInfo = ResultMapper<VkResult>::GetInfo(result.Get<0>());
            auto msg = CreateMessage(typeInfo, message);
            logger->Write(severity, msg);
        }
        else
        {
            auto typeInfo = ResultMapper<TResultType>::GetInfo(result.Get<1>());
            auto msg = CreateMessage(typeInfo, message);
            logger->Write(severity, msg);
        }
        
    }

    template<typename TResultType, template<typename> typename TResult>
    void Logger::WriteInfo(Molten::Logger* logger, const TResult<TResultType>& result, const std::string& message)
    {
        Write(logger, Molten::Logger::Severity::Info, result, message);
    }
    template<typename TResultType, template<typename, typename> typename TResult>
    void Logger::WriteInfo(Molten::Logger* logger, TResult<VkResult, TResultType>& result, const std::string& message)
    {
        Write(logger, Molten::Logger::Severity::Info, result, message);
    }

    template<typename TResultType, template<typename> typename TResult>
    void Logger::WriteDebug(Molten::Logger* logger, const TResult<TResultType>& result, const std::string& message)
    {
        Write(logger, Molten::Logger::Severity::Debug, result, message);
    }
    template<typename TResultType, template<typename, typename> typename TResult>
    void Logger::WriteDebug(Molten::Logger* logger, TResult<VkResult, TResultType>& result, const std::string& message)
    {
        Write(logger, Molten::Logger::Severity::Debug, result, message);
    }

    template<typename TResultType, template<typename> typename TResult>
    void Logger::WriteWarning(Molten::Logger* logger, const TResult<TResultType>& result, const std::string& message)
    {
        Write(logger, Molten::Logger::Severity::Warning, result, message);
    }
    template<typename TResultType, template<typename, typename> typename TResult>
    void Logger::WriteWarning(Molten::Logger* logger, TResult<VkResult, TResultType>& result, const std::string& message)
    {
        Write(logger, Molten::Logger::Severity::Warning, result, message);
    }

    template<typename TResultType, template<typename> typename TResult>
    void Logger::WriteError(Molten::Logger* logger, const TResult<TResultType>& result, const std::string& message)
    {
        Write(logger, Molten::Logger::Severity::Error, result, message);
    }
    template<typename TResultType, template<typename, typename> typename TResult>
    void Logger::WriteError(Molten::Logger* logger, TResult<VkResult, TResultType>& result, const std::string& message)
    {
        Write(logger, Molten::Logger::Severity::Error, result, message);
    }

    inline std::string Logger::CreateMessage(const ResultTypeInfo& typeInfo, const std::string& message)
    {
        auto str = message;
        if (typeInfo.name.size())
        {
            str += " (" + typeInfo.name + ")";
        }
        if (typeInfo.description.size())
        {
            str += ": " + typeInfo.description + "";
        }
        return str;
    }

}