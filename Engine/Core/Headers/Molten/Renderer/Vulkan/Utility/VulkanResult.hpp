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

#ifndef MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANRESULT_HPP
#define MOLTEN_CORE_RENDERER_VULKAN_UTILITY_VULKANRESULT_HPP

#include "Molten/Core.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include "Molten/Renderer/Vulkan/VulkanHeaders.hpp"
#include <variant>
#include <string>

MOLTEN_UNSCOPED_ENUM_BEGIN

namespace Molten::Vulkan
{
//
//    template<typename TCustomResult = VkResult>
//    class Result;
//
//
//    /**Result type, returned by vulkan utility functionsand methods. */
//    template<typename TCustomResult>
//    class Result : private std::variant<VkResult, TCustomResult>
//    {
//
//    public:
//
//        /** Helper struct for getting type of return value from variant index. */
//        /**@{*/
//        template<size_t Index>
//        struct ResultValue;
//
//        template<>
//        struct ResultValue<0>
//        {
//            using Type = VkResult;
//        };
//
//        template<>
//        struct ResultValue<1>
//        {
//            using Type = TCustomResult;
//        };
//        /**@}*/
//
//
//        /** Constructor. */
//        Result();
//
//        /** Construct result from vulkan result. */
//        Result(const VkResult result);
//
//        /** Construct result from TCustomResult. */
//        Result(const TCustomResult& result);
//
//        /** Construct result from TCustomResult. */
//        Result(TCustomResult&& result);
//
//
//        /** Assign result from vulkan result. */
//        Result& operator =(const VkResult result);
//
//        /** Assign result from TCustomResult. */
//        Result& operator =(const TCustomResult& result);
//
//        /** Assign result from TCustomResult. */
//        Result& operator =(TCustomResult&& result);
//
//
//        /** Get value from result by index. 0 or 1 is accepted indicies. */
//        /**@{*/
//        template<size_t Index>
//        typename ResultValue<Index>::Type& Get();
//
//        template<size_t Index>
//        const typename ResultValue<Index>::Type& Get() const;
//        /**@}*/
//
//        /** Get type index. Returns 0 if underlying type is VkResult, else 1. */
//        size_t GetTypeIndex() const;
//
//        /** Check if underlying value is equal to any of the provided arguments. */
//        bool ExpectAny(VkResult vulkanResult, const TCustomResult& customResult);
//
//        /** Check if underlying value is equal to VkResult::VK_SUCCESS or provided argument. */
//        bool ExpectSuccessOr(const TCustomResult& customResult);
//
//    };
//
//
//    /** Vulkan result type, returned by vulkan utility functions and methods.
//     * This is a special case, where only one result should be available if TCustomResult is VkResult.
//     */
//    template<>
//    class Result<VkResult>
//    {
//
//    public:
//
//        /** Constructor. */
//        Result();
//
//        /** Construct result from vulkan result. */
//        Result(const VkResult result);
//
//        /** Get value from result by index. 0 is the only accepted index. */
//        /**@{*/
//        template<size_t Index>
//        VkResult& Get();
//
//        template<size_t Index>
//        const VkResult& Get() const;
//
//        VkResult& Get();
//        const VkResult& Get() const;
//        /**@}*/
//
//        /** Get type index. Always returns 0. */
//        size_t GetTypeIndex() const;
//
//        /** Check if underlying vulkan result is equal or not to VkResult::VK_SUCCESS. */
//        /**@{*/
//        operator bool() const;
//        bool operator!() const;
//        /**@}*/
//
//    private:
//
//        VkResult m_value;
//
//    };
//
//
    /** Mapped type info storage, used for storing template specialization result type info.  */
    struct MOLTEN_API ResultTypeInfoStorage
    {
        const std::string name = "";
        const std::string description = "";
    };

    /** Mapped type info, declared by ResultMapper. */
    struct MOLTEN_API ResultTypeInfo
    {
        ResultTypeInfo()
        {}
        ResultTypeInfo(const ResultTypeInfoStorage& storage)
        {}

        const std::string& name = emptyString;
        const std::string& description = emptyString;

        inline static const std::string emptyString = ""; ///< Helper values, use this as default values.
        inline static const ResultTypeInfoStorage emptyStorage = { "", "" };
    };
//
//
//    /** Result mapping type trait, providing an interface for defining printable names and reason text of custom result. 
//     *  Declare your own template specialization for your own custom return values.
//     */
//    template<typename TResultType>
//    struct ResultMapper
//    {
//        inline static const auto defaultStorage = ResultTypeInfo{ ResultTypeInfo::emptyStorage };
//
//        inline static const auto GetInfo = [](const TResultType& result) -> const ResultTypeInfo { return defaultStorage; };
//    };
//
//    template<>
//    struct ResultMapper<VkResult>
//    {
//        inline static const auto success = ResultTypeInfoStorage{ "VK_SUCCESS", "Success." };
//        inline static const auto notReady = ResultTypeInfoStorage{ "VK_NOT_READY", "A fence or query has not yet completed." };
//        inline static const auto timeout = ResultTypeInfoStorage{ "VK_TIMEOUT", "A wait operation has not completed in the specified time." };
//        inline static const auto eventSet = ResultTypeInfoStorage{ "VK_EVENT_SET", "An event is signaled." };
//        inline static const auto eventReset = ResultTypeInfoStorage{ "VK_EVENT_RESET", "An event is unsignaled." };
//        inline static const auto incomplete = ResultTypeInfoStorage{ "VK_INCOMPLETE", "A return array was too small for the result." };
//        inline static const auto suboptimalKhr = ResultTypeInfoStorage{ "VK_SUBOPTIMAL_KHR", "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully." };
//        inline static const auto threadIdleKhr = ResultTypeInfoStorage{ "VK_THREAD_IDLE_KHR", "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call." };
//        inline static const auto threadDoneKhr = ResultTypeInfoStorage{ "VK_THREAD_DONE_KHR", "A deferred operation is not complete but there is no work remaining to assign to additional threads." };
//        inline static const auto operationDeferredKhr = ResultTypeInfoStorage{ "VK_OPERATION_DEFERRED_KHR", "A deferred operation was requested and at least some of the work was deferred." };
//        inline static const auto operationNotDeferredKhr = ResultTypeInfoStorage{ "VK_OPERATION_NOT_DEFERRED_KHR", "A deferred operation was requested and no operations were deferred." };
//        inline static const auto pipelineCompileRequiredExt = ResultTypeInfoStorage{ "VK_PIPELINE_COMPILE_REQUIRED_EXT", "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed." };
//        inline static const auto errorOutOfHostMemory = ResultTypeInfoStorage{ "VK_ERROR_OUT_OF_HOST_MEMORY", "A host memory allocation has failed." };
//        inline static const auto errorOutOfDeviceMemory = ResultTypeInfoStorage{ "VK_ERROR_OUT_OF_DEVICE_MEMORY", "A device memory allocation has failed." };
//        inline static const auto errorInitializationFailed = ResultTypeInfoStorage{ "VK_ERROR_INITIALIZATION_FAILED", "Initialization of an object could not be completed for implementation-specific reasons." };
//        inline static const auto errorDeviceLost = ResultTypeInfoStorage{ "VK_ERROR_DEVICE_LOST", "The logical or physical device has been lost." };
//        inline static const auto errorMemoryMapFailed = ResultTypeInfoStorage{ "VK_ERROR_MEMORY_MAP_FAILED", "Mapping of a memory object has failed." };
//        inline static const auto errorLayerNotPresent = ResultTypeInfoStorage{ "VK_ERROR_LAYER_NOT_PRESENT", "A requested layer is not present or could not be loaded." };
//        inline static const auto errorExtensionNotPresent = ResultTypeInfoStorage{ "VK_ERROR_EXTENSION_NOT_PRESENT", "A requested extension is not supported." };
//        inline static const auto errorFeatureNotPresent = ResultTypeInfoStorage{ "VK_ERROR_FEATURE_NOT_PRESENT", "A requested feature is not supported." };
//        inline static const auto errorIncompatibleDriver = ResultTypeInfoStorage{ "VK_ERROR_INCOMPATIBLE_DRIVER", "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons." };
//        inline static const auto errorTooManyObjects = ResultTypeInfoStorage{ "VK_ERROR_TOO_MANY_OBJECTS", "Too many objects of the type have already been created." };
//        inline static const auto errorFormatNotSupported = ResultTypeInfoStorage{ "VK_ERROR_FORMAT_NOT_SUPPORTED", "A requested format is not supported on this device." };
//        inline static const auto errorFragmentedPool = ResultTypeInfoStorage{ "VK_ERROR_FRAGMENTED_POOL", "A pool allocation has failed due to fragmentation of the pool’s memory." };
//        inline static const auto errorSurfaceLostKhr = ResultTypeInfoStorage{ "VK_ERROR_SURFACE_LOST_KHR", "A surface is no longer available." };
//        inline static const auto errorNativeWindowInUseKhr = ResultTypeInfoStorage{ "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR", "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again." };
//        inline static const auto errorOutOfDateKhr = ResultTypeInfoStorage{ "VK_ERROR_OUT_OF_DATE_KHR", "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail." };
//        inline static const auto errorIncompatibleDisplayKhr = ResultTypeInfoStorage{ "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR", "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image." };
//        inline static const auto errorInvalidShaderNv = ResultTypeInfoStorage{ "VK_ERROR_INVALID_SHADER_NV", "One or more shaders failed to compile or link." };
//        inline static const auto errorOutOfPoolMemory = ResultTypeInfoStorage{ "VK_ERROR_OUT_OF_POOL_MEMORY", "A pool memory allocation has failed." };
//        inline static const auto errorInvalidExternalHandle = ResultTypeInfoStorage{ "VK_ERROR_INVALID_EXTERNAL_HANDLE", "An external handle is not a valid handle of the specified type." };
//        inline static const auto errorFragmentation = ResultTypeInfoStorage{ "VK_ERROR_FRAGMENTATION", "A descriptor pool creation has failed due to fragmentation." };
//        inline static const auto errorInvalidDeviceAddressExt = ResultTypeInfoStorage{ "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT", "A buffer creation failed because the requested address is not available." };
//        inline static const auto errorFullScreenExclusiveModeLostExt = ResultTypeInfoStorage{ "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT", "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access." };
//        inline static const auto errorUnknown = ResultTypeInfoStorage{ "VK_ERROR_UNKNOWN", "An unknown error has occurred." };
//
//        inline static const auto GetInfo = [](const VkResult& result) -> const ResultTypeInfo
//        {
//            switch (result)
//            {
//            case VkResult::VK_SUCCESS: return success;
//            case VkResult::VK_NOT_READY: return notReady;
//            case VkResult::VK_TIMEOUT: return timeout;
//            case VkResult::VK_EVENT_SET: return eventSet;
//            case VkResult::VK_EVENT_RESET: return eventReset;
//            case VkResult::VK_INCOMPLETE: return incomplete;
//            case VkResult::VK_SUBOPTIMAL_KHR: return suboptimalKhr;
//            case VkResult::VK_THREAD_IDLE_KHR: return threadIdleKhr;
//            case VkResult::VK_THREAD_DONE_KHR: return threadDoneKhr;
//            case VkResult::VK_OPERATION_DEFERRED_KHR: return operationDeferredKhr;
//            case VkResult::VK_OPERATION_NOT_DEFERRED_KHR: return operationNotDeferredKhr;
//            case VkResult::VK_PIPELINE_COMPILE_REQUIRED_EXT: return pipelineCompileRequiredExt;
//            case VkResult::VK_ERROR_OUT_OF_HOST_MEMORY: return errorOutOfHostMemory;
//            case VkResult::VK_ERROR_OUT_OF_DEVICE_MEMORY: return errorOutOfDeviceMemory;
//            case VkResult::VK_ERROR_INITIALIZATION_FAILED: return errorInitializationFailed;
//            case VkResult::VK_ERROR_DEVICE_LOST: return errorDeviceLost;
//            case VkResult::VK_ERROR_MEMORY_MAP_FAILED: return errorMemoryMapFailed;
//            case VkResult::VK_ERROR_LAYER_NOT_PRESENT: return errorLayerNotPresent;
//            case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT: return errorExtensionNotPresent;
//            case VkResult::VK_ERROR_FEATURE_NOT_PRESENT: return errorFeatureNotPresent;
//            case VkResult::VK_ERROR_INCOMPATIBLE_DRIVER: return errorIncompatibleDriver;
//            case VkResult::VK_ERROR_TOO_MANY_OBJECTS: return errorTooManyObjects;
//            case VkResult::VK_ERROR_FORMAT_NOT_SUPPORTED: return errorFormatNotSupported;
//            case VkResult::VK_ERROR_FRAGMENTED_POOL: return errorFragmentedPool;
//            case VkResult::VK_ERROR_SURFACE_LOST_KHR: return errorSurfaceLostKhr;
//            case VkResult::VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return errorNativeWindowInUseKhr;
//            case VkResult::VK_ERROR_OUT_OF_DATE_KHR: return errorOutOfDateKhr;
//            case VkResult::VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return errorIncompatibleDisplayKhr;
//            case VkResult::VK_ERROR_INVALID_SHADER_NV: return errorInvalidShaderNv;
//            case VkResult::VK_ERROR_OUT_OF_POOL_MEMORY: return errorOutOfPoolMemory;
//            case VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE: return errorInvalidExternalHandle;
//            case VkResult::VK_ERROR_FRAGMENTATION: return errorFragmentation;
//            case VkResult::VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: return errorInvalidDeviceAddressExt;
//            case VkResult::VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return errorFullScreenExclusiveModeLostExt;
//            case VkResult::VK_ERROR_UNKNOWN: return errorUnknown;
//            default: return errorUnknown;
//            }
//        };
//
//    };
//
}

//#include "Molten/Renderer/Vulkan/Utility/VulkanResult.inl"

MOLTEN_UNSCOPED_ENUM_END

#endif

#endif