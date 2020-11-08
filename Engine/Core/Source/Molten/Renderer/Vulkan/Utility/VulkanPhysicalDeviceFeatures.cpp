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

#include "Molten/Renderer/Vulkan/Utility/VulkanPhysicalDeviceFeatures.hpp"

#if defined(MOLTEN_ENABLE_VULKAN)
#include <map>

MOLTEN_UNSCOPED_ENUM_BEGIN

#define CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME_QUOTE(x) #x
#define CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(feature) { &(g_lookupFeatures.##feature), CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME_QUOTE(##feature) }

namespace Molten::Vulkan
{

    static const std::string g_emptyString = "";
    static const VkPhysicalDeviceFeatures g_lookupFeatures = {};
    static const std::map<const VkBool32*, std::string> g_featureNames =
    {
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(robustBufferAccess),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(fullDrawIndexUint32),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(imageCubeArray),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(independentBlend),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(geometryShader),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(tessellationShader),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sampleRateShading),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(dualSrcBlend),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(logicOp),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(multiDrawIndirect),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(drawIndirectFirstInstance),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(depthClamp),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(depthBiasClamp),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(fillModeNonSolid),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(depthBounds),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(wideLines),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(largePoints),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(alphaToOne),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(multiViewport),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(samplerAnisotropy),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(textureCompressionETC2),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(textureCompressionASTC_LDR),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(textureCompressionBC),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(occlusionQueryPrecise),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(pipelineStatisticsQuery),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(vertexPipelineStoresAndAtomics),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(fragmentStoresAndAtomics),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderTessellationAndGeometryPointSize),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderImageGatherExtended),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageExtendedFormats),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageMultisample),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageReadWithoutFormat),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageWriteWithoutFormat),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderUniformBufferArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderSampledImageArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageBufferArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderStorageImageArrayDynamicIndexing),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderClipDistance),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderCullDistance),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderFloat64),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderInt64),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderInt16),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderResourceResidency),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(shaderResourceMinLod),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseBinding),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyBuffer),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyImage2D),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyImage3D),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency2Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency4Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency8Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidency16Samples),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(sparseResidencyAliased),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(variableMultisampleRate),
        CREATE_PHYSICAL_DEVICE_FEATURE_WITH_NAME(inheritedQueries)
    };

    // Physical device feature with name implementations.
    PhysicalDeviceFeatureWithName::PhysicalDeviceFeatureWithName() :
        pointer(nullptr),
        name(g_emptyString)
    { }

    PhysicalDeviceFeatureWithName::PhysicalDeviceFeatureWithName(
        PhysicalDeviceFeaturePointer pointer,
        const std::string& name
    ) :
        pointer(pointer),
        name(name)
    {}


    // Static implementations.
    void EnablePhysicalDeviceFeatures(
        VkPhysicalDeviceFeatures& finalFeatures,
        const PhysicalDeviceFeaturePointers enableFeatures)
    {
        for (auto& enableFeature : enableFeatures)
        {
            finalFeatures.*enableFeature = VK_TRUE;
        }
    }

    void EnableOptionalPhysicalDeviceFeatures(
        VkPhysicalDeviceFeatures& finalFeatures,
        const VkPhysicalDeviceFeatures& availableFeatures,
        const PhysicalDeviceFeaturePointers optionalFeatures)
    {
        for (auto& optionalFeature : optionalFeatures)
        {
            VkBool32 available = availableFeatures.*optionalFeature;
            if (available)
            {
                finalFeatures.*optionalFeature = VK_TRUE;
            }
        }
    }

    bool CheckRequiredPhysicalDeviceFeatures(
        PhysicalDeviceFeaturesWithName& missingFeatures,
        const VkPhysicalDeviceFeatures& availableFeatures,
        const PhysicalDeviceFeaturePointers requiredFeatures)
    {
        missingFeatures.clear();

        for (auto& requiredFeature : requiredFeatures)
        {
            VkBool32 available = availableFeatures.*requiredFeature;
            if (!available)
            {
                auto key = &(g_lookupFeatures.*requiredFeature);
                auto it = g_featureNames.find(key);
                if (it != g_featureNames.end())
                {
                    missingFeatures.push_back(PhysicalDeviceFeatureWithName{ requiredFeature, it->second });
                }
                else
                {
                    missingFeatures.push_back(PhysicalDeviceFeatureWithName{ nullptr, g_emptyString });
                }
            }
        }

        return missingFeatures.size() == 0;
    }

}

MOLTEN_UNSCOPED_ENUM_END

#endif