#pragma once

#define mb *1024*1024
#define kb *1024

//std::pmr::monotonic_buffer_resource gloabal_pool{ std::data(global_buffer), buffer_size };

namespace dc // disus core engine
{

	class Allocator
	{
	public:

		Allocator(uint32_t size);

		~Allocator();

		template <typename T>
		inline T* Alloc(uint32_t size)
		{
			allocated += size;
			assert(allocated <= size);
			return static_cast<T*>(pool.allocate(size));
		}

		template <typename T>
		inline T* Realloc(uint32_t size)
		{
		}

		std::pmr::monotonic_buffer_resource& operator()()
		{
			return pool;
		}

		uint32_t Size() const
		{
			return size;
		}

	private:

		void recreate_pool(uint32_t size);

	private:
		std::pmr::monotonic_buffer_resource pool;
		uint32_t size;
		uint32_t allocated;
	};

}