#pragma once

#include "../frame/include.h"
#include "define.h"
#include <boost/any.hpp>
#include "../frame/util_log.h"

namespace face_recognition
{
	class context : public boost::noncopyable
	{
	public:
		static boost::shared_ptr<context> create()
		{
			boost::shared_ptr<context> sp_context(boost::make_shared<context>());
			return sp_context;
		}
		void destroy()
		{
		}
	public:
		template <class value_type>
		void set_value(const std::wstring& str_key, boost::shared_ptr<value_type> sp_value)
		{
			std::map <std::wstring, boost::any>::iterator it = m_map_key_value.find(str_key);
			if (it == m_map_key_value.end())
			{
				boost::any value(sp_value);
				m_map_key_value.insert(std::make_pair(str_key, value));
				util_log::logd(CONTEXT_TAG, "add key[%ws] value.", str_key.c_str());
			}
			else
			{
				boost::any& value = it->second;
				value = boost::any(sp_value);
				util_log::logd(CONTEXT_TAG, "update key[%ws] value.", str_key.c_str());
			}
		}
		template <class value_type>
		result get_value(const std::wstring& str_key, boost::shared_ptr<value_type>& sp_value)
		{
			std::map <std::wstring, boost::any>::iterator it = m_map_key_value.find(str_key);
			if (it == m_map_key_value.end())
			{
				util_log::logd(CONTEXT_TAG, "key[%ws] is not found.", str_key.c_str());
				return result_context_key_not_found;
			}
			boost::any& value = it->second;
			sp_value = boost::any_cast<boost::shared_ptr<value_type>>(value);
			return result_success;
		}

		void set_bool_value(const std::wstring& str_key, bool value)
		{
			boost::shared_ptr<bool> sp_value(boost::make_shared<bool>(value));
			set_value(str_key, sp_value);
		}
		result get_bool_value(const std::wstring& str_key, bool& value)
		{
			boost::shared_ptr<bool> sp_value;
			result res = get_value(str_key, sp_value);
			if (res != result_success)
			{
				return res;
			}
			value = *sp_value.get();
			return result_success;
		}
		void set_int_value(const std::wstring& str_key, int value)
		{
			boost::shared_ptr<int> sp_value(boost::make_shared<int>(value));
			set_value(str_key, sp_value);
		}
		result get_int_value(const std::wstring& str_key, int& value)
		{
			boost::shared_ptr<int> sp_value;
			result res = get_value(str_key, sp_value);
			if (res != result_success)
			{
				return res;
			}
			value = *sp_value.get();
			return result_success;
		}
		void set_string_value(const std::wstring& str_key, const std::wstring& str_value)
		{
			boost::shared_ptr<std::wstring> sp_value(boost::make_shared<std::wstring>(str_value));
			set_value(str_key, sp_value);
		}
		result get_string_value(const std::wstring& str_key, std::wstring& str_value)
		{
			boost::shared_ptr<std::wstring> sp_value;
			result res = get_value(str_key, sp_value);
			if (res != result_success)
			{
				return res;
			}
			str_value = *sp_value.get();
			return result_success;
		}
	private:
		std::map <std::wstring, boost::any> m_map_key_value;
	public:
		~context()
		{
			destroy();
		}
	};
}