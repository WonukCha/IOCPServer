#pragma once
#include "RingBuffer.hpp"
#include <mutex>

class RingbufferLock : public RingBuffer
{
public:
	void Init()
	{
		std::lock_guard<std::mutex> lock_guard(mCS);
		RingBuffer::Init();
	}
	bool IsEmpty()
	{
		std::lock_guard<std::mutex> lock_guard(mCS);
		return RingBuffer::IsEmpty();
	}
	bool IsFull()
	{
		std::lock_guard<std::mutex> lock_guard(mCS);
		return RingBuffer::IsFull();
	}
	size_t GetSize()
	{
		std::lock_guard<std::mutex> lock_guard(mCS);
		return RingBuffer::GetSize();
	}

	bool PutData(const void* data, size_t size)
	{
		std::lock_guard<std::mutex> lock_guard(mCS);
		return RingBuffer::PutData(data, size);
	}
	size_t GetData(void* data, size_t size, rbuf_opt_e clear = rbuf_opt_e::RBUF_CLEAR)
	{
		std::lock_guard<std::mutex> lock_guard(mCS);
		return RingBuffer::GetData(data, size, clear);
	}

	bool DeleteData(size_t size)
	{
		std::lock_guard<std::mutex> lock_guard(mCS);
		return RingBuffer::DeleteData(size);
	}

	void Flush(rbuf_opt_e clear)
	{
		std::lock_guard<std::mutex> lock_guard(mCS);
		return RingBuffer::Flush(clear);
	}

	void Clear()
	{
		std::lock_guard<std::mutex> lock_guard(mCS);
		return RingBuffer::Clear();
	}
	size_t GetMaxSize()
	{
		return RingBuffer::GetMaxSize();
	}
private:
	std::mutex mCS;
};
