
#include "PCH.h"

const uint32_t g_buffer_size = 256 mb;
static byte g_buffer[g_buffer_size];
static uint32_t g_allocated = 0;

namespace dc
{
	Allocator::Allocator(uint32_t size)
		:pool(std::data(g_buffer), size), size(size), allocated(0)
	{
		g_allocated += size;
	}

	Allocator::~Allocator()
	{
		g_allocated -= size;
	}

	void Allocator::recreate_pool(uint32_t size)
	{
		pool.release();

		g_allocated -= this->size;
		g_allocated += size;
	}
}