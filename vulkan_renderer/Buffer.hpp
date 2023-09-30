//#pragma once
//
//const uint32_t default_buffer_alloc_size = 1 mb;
//
//class Buffer
//{
//public:
//	Buffer(uint32_t alloc_size = default_buffer_alloc_size) : allocator(alloc_size), data(allocator.Alloc<byte>(alloc_size))
//	{}
//
//	Buffer(const Buffer& other)
//		: allocator(other.allocator.Size()), data(allocator.Alloc<byte>(other.allocator.Size()))
//	{
//		memcpy(data, other.data, other.allocator.Size());
//		size = other.size;
//	}
//
//	template<typename T>
//	Buffer(const T& data, uint32_t alloc_size = default_buffer_alloc_size)
//		: allocator(alloc_size), data(allocator.Alloc<byte>(alloc_size))
//	{
//		Add(data);
//	}
//
//	~Buffer()
//	{}
//
//	template<typename T>
//	T* AsPtr()
//	{
//		return (T*)data;
//	}
//
//	template<typename T>
//	T As()
//	{
//		return *(T*)data;
//	}
//
//	inline uint32_t Size() const { return size; }
//
//	template<typename T>
//	void Add(const T& data)
//	{
//		Add(&data, sizeof(data));
//	}
//
//	template<typename T>
//	void Add(const T* data, uint32_t size)
//	{
//		assert(this->size + size <= allocator.Size());
//
//		memcpy(this->data + this->size / sizeof(byte), data, size);
//		this->size += size;
//	}
//
//private:
//	dc::Allocator allocator;
//
//	byte* data;
//
//	uint32_t size = 0;
//
//public:
//
//	static void BufferTest()
//	{
//		struct stats
//		{
//			uint32_t Str;
//			uint32_t Int;
//			uint32_t Wil;
//			uint32_t Dex;
//		} current[3] = 
//		{
//			{1,2,3,4},
//			{5,6,7,8},
//			{9,10,11,12}
//		};
//
//		std::function show_stats_array(
//		[](Buffer b){
//
//			stats* current_stats = b.AsPtr<stats>();
//
//			for (size_t i = 0; i < b.Size() / sizeof(stats); i++)
//			{
//				std::cout << "Stats[" << i << "]: " 
//					<< current_stats[i].Str << ' '
//					<< current_stats[i].Int << ' '
//					<< current_stats[i].Wil << ' '
//					<< current_stats[i].Dex << '\n';
//			}
//
//			std::cout << '\n';
//		});
//
//		Buffer b3;
//		b3.Add(current[0]);
//		b3.Add(current[1]);
//		b3.Add(current[2]);
//
//		show_stats_array(b3);
//
//		Buffer b2(current, sizeof(current) + sizeof(current[2]));
//		b2.Add(current[2]);
//
//		show_stats_array(b2);
//
//		Buffer b1(b2);
//
//		show_stats_array(b1);
//	}
//};
