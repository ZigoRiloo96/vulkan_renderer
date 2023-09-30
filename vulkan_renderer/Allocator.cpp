
#include "PCH.h"

namespace dc::mem // disus core engine
{

u32 FindMemoryTypeIndex(VkPhysicalDeviceMemoryProperties& physicalMemoryProperties, const u32 memoryTypeBits, const vkMemUsage_t usage)
{
	VkMemoryPropertyFlags required = 0;
	VkMemoryPropertyFlags preferred = 0;

	switch ( usage ) {
	case VULKAN_MEMORY_USAGE_GPU_ONLY:
		preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case VULKAN_MEMORY_USAGE_CPU_ONLY:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	case VULKAN_MEMORY_USAGE_CPU_TO_GPU:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case VULKAN_MEMORY_USAGE_GPU_TO_CPU:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	default:
		// FatalError( "idVulkanAllocator::AllocateFromPools: Unknown memory usage." );
	}

	for ( u32 i = 0; i < physicalMemoryProperties.memoryTypeCount; ++i ) 
	{
		if ( ( ( memoryTypeBits >> i ) & 1 ) == 0 ) {
			continue;
		}
		
		const VkMemoryPropertyFlags properties = physicalMemoryProperties.memoryTypes[ i ].propertyFlags;
		if ( ( properties & required ) != required ) {
			continue;
		}

		if ( ( properties & preferred ) != preferred ) {
			continue;
		}

		return i;
	}

	for ( u32 i = 0; i < physicalMemoryProperties.memoryTypeCount; ++i ) {
		if ( ( ( memoryTypeBits >> i ) & 1 ) == 0 ) {
			continue;
		}
			
		const VkMemoryPropertyFlags properties = physicalMemoryProperties.memoryTypes[ i ].propertyFlags;
		if ( ( properties & required ) != required ) {
			continue;
		}

		return i;
	}

	return UINT32_MAX;
}


namespace block
{

bool
Init(vkMemBlock* block)
{
  return false;
}

void
Term(vkMemBlock* block)
{
}

bool
Allocate(vkMemBlock* block, const u32 size,
                            const u32 align,
                            const VkDeviceSize granularity,
                            const vkAllocationType_t allocType,
                            vkAllocation* allocation)
{
  return false;
}

void
Free(vkMemBlock* block, vkAllocation* allocation)
{
}

}

namespace allocator
{

void Init(vkAllocator* allocator)
{

}

void Term(vkAllocator* allocator)
{}

void Allocate(vkAllocator* allocator, const u32 size,
																			const u32 align,
																			const VkDeviceSize granularity,
																			const vkMemUsage_t usage,
																			const vkAllocationType_t allocType)
{}

void Free(vkAllocator* allocator, const vkAllocation allocation)
{}

}


}