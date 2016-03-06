#pragma once

#include "include.h"
#include <string.h>
class util_buffer : public boost::noncopyable
{
public:
	bool create(size_t size, const unsigned char* p_data = NULL)
	{
		boost::shared_array<unsigned char> sp_data( new unsigned char[size]);
		m_sp_data = sp_data;
		if (p_data != NULL)
		{
			memcpy(m_sp_data.get(), p_data, size);
			m_pos = size;
		}
		else
		{
			m_pos = 0;
		}
		m_size = size;
		return true;
	}
	template <typename T>
	bool create(size_t size, const T* p_data)
	{
		return create(size, reinterpret_cast<const unsigned char*>(p_data));
	}
	void destroy()
	{
		m_sp_data.reset();
		m_size = 0;
		m_pos = 0;
	}
	size_t size()
	{
		return m_size;
	}
	const unsigned char* data()
	{
		return m_sp_data.get();
	}
	template <typename T>
	const T* data()
	{
		return reinterpret_cast<const T*>(data());
	}
	bool empty()
	{
		ENSUREf(m_size == 0);
		return true;
	}
	void write(const unsigned char* data, size_t size, size_t pos = -1)
	{
		unsigned char* p_data = m_sp_data.get();
		
		if (pos != -1)
		{
			p_data += pos;
			Assert(size <= m_size - pos);
		}
		else
		{
			p_data += m_pos;
			Assert(size <= m_size - m_pos);
			m_pos += size;
		}
		memcpy(p_data, data, size);
	}
	void write(boost::shared_ptr<util_buffer> sp_buffer, size_t pos = -1)
	{
		write(sp_buffer->data(), sp_buffer->size());
	}
	unsigned char* _data()
	{
		return m_sp_data.get();
	}
private:
	boost::shared_array<unsigned char> m_sp_data;
	size_t m_size;
	size_t m_pos;
public:
	util_buffer()
		: m_size(0)
		, m_pos(0)
	{

	}
	~util_buffer()
	{
		destroy();
	}
};



