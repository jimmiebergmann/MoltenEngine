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


#include "Molten/Renderer/Vulkan/VulkanResultOld.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)

#include <utility>

MOLTEN_UNSCOPED_ENUM_BEGIN

static const std::pair<std::string, std::string> g_text_empty = { "", "" };

#define MOLTEN_VULKAN_TEXT_QUOTE(x) #x
#define MOLTEN_VULKAN_TEXT_DEFINITION(result, description) \
    static const std::pair<std::string, std::string> g_text_VK_##result = { MOLTEN_VULKAN_TEXT_QUOTE(VK_##result), description }

MOLTEN_VULKAN_TEXT_DEFINITION(SUCCESS, "Success.");
MOLTEN_VULKAN_TEXT_DEFINITION(NOT_READY, "A fence or query has not yet completed.");
MOLTEN_VULKAN_TEXT_DEFINITION(TIMEOUT, "A wait operation has not completed in the specified time.");
MOLTEN_VULKAN_TEXT_DEFINITION(EVENT_SET, "An event is signaled.");
MOLTEN_VULKAN_TEXT_DEFINITION(EVENT_RESET, "An event is unsignaled.");
MOLTEN_VULKAN_TEXT_DEFINITION(INCOMPLETE, "A return array was too small for the result.");
MOLTEN_VULKAN_TEXT_DEFINITION(SUBOPTIMAL_KHR, "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.");
MOLTEN_VULKAN_TEXT_DEFINITION(THREAD_IDLE_KHR, "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call.");
MOLTEN_VULKAN_TEXT_DEFINITION(THREAD_DONE_KHR, "A deferred operation is not complete but there is no work remaining to assign to additional threads.");
MOLTEN_VULKAN_TEXT_DEFINITION(OPERATION_DEFERRED_KHR, "A deferred operation was requested and at least some of the work was deferred.");
MOLTEN_VULKAN_TEXT_DEFINITION(OPERATION_NOT_DEFERRED_KHR, "A deferred operation was requested and no operations were deferred.");
MOLTEN_VULKAN_TEXT_DEFINITION(PIPELINE_COMPILE_REQUIRED_EXT, "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_OUT_OF_HOST_MEMORY, "A host memory allocation has failed.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_OUT_OF_DEVICE_MEMORY, "A device memory allocation has failed.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_INITIALIZATION_FAILED, "Initialization of an object could not be completed for implementation-specific reasons.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_DEVICE_LOST, "The logical or physical device has been lost.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_MEMORY_MAP_FAILED, "Mapping of a memory object has failed.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_LAYER_NOT_PRESENT, "A requested layer is not present or could not be loaded.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_EXTENSION_NOT_PRESENT, "A requested extension is not supported.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_FEATURE_NOT_PRESENT, "A requested feature is not supported.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_INCOMPATIBLE_DRIVER, "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_TOO_MANY_OBJECTS, "Too many objects of the type have already been created.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_FORMAT_NOT_SUPPORTED, "A requested format is not supported on this device.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_FRAGMENTED_POOL, "A pool allocation has failed due to fragmentation of the pool’s memory.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_SURFACE_LOST_KHR, "A surface is no longer available.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_NATIVE_WINDOW_IN_USE_KHR, "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_OUT_OF_DATE_KHR, "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_INCOMPATIBLE_DISPLAY_KHR, "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_INVALID_SHADER_NV, "One or more shaders failed to compile or link.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_OUT_OF_POOL_MEMORY, "A pool memory allocation has failed.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_INVALID_EXTERNAL_HANDLE, "An external handle is not a valid handle of the specified type.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_FRAGMENTATION, "A descriptor pool creation has failed due to fragmentation.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_INVALID_DEVICE_ADDRESS_EXT, "A buffer creation failed because the requested address is not available.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT, "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access.");
MOLTEN_VULKAN_TEXT_DEFINITION(ERROR_UNKNOWN, "An unknown error has occurred.");

#define MOLTEN_VULKAN_RESULT_CASE_TYPE(result) g_text_VK_##result
#define MOLTEN_VULKAN_RESULT_CASE_first(result) case VkResult::VK_ ##result : return MOLTEN_VULKAN_RESULT_CASE_TYPE(result).first
#define MOLTEN_VULKAN_RESULT_CASE_second(result) case VkResult::VK_ ##result : return MOLTEN_VULKAN_RESULT_CASE_TYPE(result).second

#define MOLTEN_VULKAN_RESULT_CASES(type) \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (SUCCESS); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (NOT_READY); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (TIMEOUT); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (EVENT_SET); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (EVENT_RESET); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (INCOMPLETE); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (SUBOPTIMAL_KHR); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (THREAD_IDLE_KHR); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (THREAD_DONE_KHR); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (OPERATION_DEFERRED_KHR); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (OPERATION_NOT_DEFERRED_KHR); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (PIPELINE_COMPILE_REQUIRED_EXT); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_OUT_OF_HOST_MEMORY); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_OUT_OF_DEVICE_MEMORY); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_INITIALIZATION_FAILED); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_DEVICE_LOST); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_MEMORY_MAP_FAILED); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_LAYER_NOT_PRESENT); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_EXTENSION_NOT_PRESENT); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_FEATURE_NOT_PRESENT); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_INCOMPATIBLE_DRIVER); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_TOO_MANY_OBJECTS); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_FORMAT_NOT_SUPPORTED); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_FRAGMENTED_POOL); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_SURFACE_LOST_KHR); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_NATIVE_WINDOW_IN_USE_KHR); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_OUT_OF_DATE_KHR); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_INCOMPATIBLE_DISPLAY_KHR); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_INVALID_SHADER_NV); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_OUT_OF_POOL_MEMORY); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_INVALID_EXTERNAL_HANDLE); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_FRAGMENTATION); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_INVALID_DEVICE_ADDRESS_EXT); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT); \
    MOLTEN_VULKAN_RESULT_CASE_ ##type (ERROR_UNKNOWN);

namespace Molten
{

    // Helper implementations.
    static const std::string& GetName(VkResult result)
    {
        switch (result)
        {
            MOLTEN_VULKAN_RESULT_CASES(first);
            default: return g_text_VK_ERROR_UNKNOWN.first;
        }
    }

    static const std::string& GetDecription(VkResult result)
    {
        switch (result)
        {
            MOLTEN_VULKAN_RESULT_CASES(second);
            default: return g_text_VK_ERROR_UNKNOWN.second;
        }
    }


    // Vulkan result implementations.
    VulkanResultOld::VulkanResultOld() :
        name(g_text_empty.first),
        description(g_text_empty.second)
    {}

    VulkanResultOld::VulkanResultOld(VkResult result) :
        name(GetName(result)),
        description(GetDecription(result))
    {}

}

MOLTEN_UNSCOPED_ENUM_END

#endif