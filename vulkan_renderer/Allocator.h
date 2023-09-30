#pragma once

namespace dc::mem // disus core engine
{

	enum vkMemUsage_t
	{
		VULKAN_MEMORY_USAGE_UNKNOWN,
		VULKAN_MEMORY_USAGE_GPU_ONLY,
		VULKAN_MEMORY_USAGE_CPU_ONLY,
		VULKAN_MEMORY_USAGE_CPU_TO_GPU,
		VULKAN_MEMORY_USAGE_GPU_TO_CPU,
		VULKAN_MEMORY_USAGES,
	};

	enum vkAllocationType_t
	{
		VULKAN_ALLOCATION_TYPE_FREE,
		VULKAN_ALLOCATION_TYPE_BUFFER,
		VULKAN_ALLOCATION_TYPE_IMAGE,
		VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR,
		VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL,
		VULKAN_ALLOCATION_TYPES,
	};

	struct Context;

	u32 FindMemoryTypeIndex(Context* context, const u32 memoryTypeBits, const vkMemUsage_t usage);

	struct vkMemBlock;

	struct vkAllocation
	{
		vkMemBlock* block = nullptr;
		u32 id = 0;
		VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
		VkDeviceSize offset = 0;
		VkDeviceSize size = 0;
		byte* data = nullptr;
	};

	struct vkMemChunk
	{
		u32 id = 0;
		VkDeviceSize size = 0;
		VkDeviceSize offset = 0;
		vkMemChunk* prev = nullptr;
		vkMemChunk* next = nullptr;
		vkAllocationType_t type;
	};

	struct vkMemBlock
	{
		vkMemChunk* head = nullptr;
		u32 nextBlockId = 0;
		u32 memTypeIndex = 0;
		vkMemUsage_t usage = {};
		VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
		VkDeviceSize size = 0;
		VkDeviceSize allocated = 0;
		byte* data = nullptr;
	};

	namespace block
	{
		bool Init(vkMemBlock* block);
		void Term(vkMemBlock* block);
		bool IsHostVisible(vkMemBlock* block) { return block->usage != VULKAN_MEMORY_USAGE_GPU_ONLY; }
		bool Allocate(vkMemBlock* block, const u32 size, 
																		 const u32 align,
																		 const VkDeviceSize granularity,
																		 const vkAllocationType_t allocType,
																		 vkAllocation* allocation);
		void Free(vkMemBlock* block, vkAllocation* allocation);
	}

	typedef std::array< std::list< vkMemBlock * >, VK_MAX_MEMORY_TYPES > vkMemBlocks;

	struct vkAllocator
	{
		// int garbageIndex = 0;
		i32 deviceLocalMemoryBytes = 0;
		i32 hostVisibleMemoryBytes = 0;
		VkDeviceSize bufferImageGranularity = 0;

		vkMemBlocks blocks;
		// std::list<vkAllocation> garbage[MAX_FRAMES_IN_FLIGHT];
	};

	namespace allocator
	{
		void Init(vkAllocator* allocator);
		void Term(vkAllocator* allocator);
		void Allocate(vkAllocator* allocator, const u32 size,
																					const u32 align,
																					const VkDeviceSize granularity,
																					const vkMemUsage_t usage,
																					const vkAllocationType_t allocType);
		void Free(vkAllocator* allocator, const vkAllocation allocation);
	}

	extern vkAllocator vulkanAllocator;

}