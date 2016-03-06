#pragma once


#include "util_buffer.h"

class util_cache : public boost::noncopyable
{
public:
	void put(boost::shared_ptr<util_buffer> sp_buffer)
	{
		m_list_cache.push_back(sp_buffer);
	}
	void clear()
	{
		m_list_cache.clear();
	}
	bool empty()
	{
		return m_list_cache.empty();
	}
	boost::shared_ptr<util_buffer> extract()
	{
		return _peek(true);
	}
	boost::shared_ptr<util_buffer> peek()
	{
		return _peek(false);
	}
private:
	boost::shared_ptr<util_buffer> _peek(bool b_clear)
	{
		size_t cache_size = 0;
		for (std::list<boost::shared_ptr<util_buffer> >::iterator it = m_list_cache.begin(); it != m_list_cache.end(); it++)
		{
			boost::shared_ptr<util_buffer> sp_buffer = *it;
			cache_size += sp_buffer->size();
		}
		boost::shared_ptr<util_buffer> sp_cache;
		if (cache_size != 0)
		{
			sp_cache = boost::shared_ptr<util_buffer>(new util_buffer);

			Assert(sp_cache->create(cache_size));

			for (std::list<boost::shared_ptr<util_buffer> >::iterator it = m_list_cache.begin(); it != m_list_cache.end(); it++)
			{
				boost::shared_ptr<util_buffer> sp_buffer = *it;
				sp_cache->write(sp_buffer);
			}
			if (b_clear)
			{
				clear();
			}
		}
		return sp_cache;
	}
private:
	std::list<boost::shared_ptr<util_buffer> > m_list_cache;
};