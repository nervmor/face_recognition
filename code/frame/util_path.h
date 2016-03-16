#pragma once

#include "util_string.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp> 
#include <boost/filesystem/operations.hpp>  
#include <boost/format.hpp>
#include "include.h"
class util_path
{
public:
	static std::wstring get_image_dir()
	{
#ifdef _WIN32
#include <windows.h>
		WCHAR szImagePath[4096];
		GetModuleFileNameW(NULL, szImagePath, 4096);
		std::wstring str_image_dir = szImagePath;
		return get_dir(str_image_dir);
#else
		std::wstring str_image_dir = boost::filesystem::initial_path<boost::filesystem::wpath>().wstring();
		if (str_image_dir[str_image_dir.size() - 1] != L'/')
		{
			str_image_dir += L"/";
		}
		return str_image_dir;
#endif // _WIN32
	}
	static std::wstring get_dir(const std::wstring& str_file_name)
	{
		boost::filesystem::wpath path(str_file_name);
		std::wstring str_dir = path.parent_path().wstring();
		if (str_dir[str_dir.size() - 1] != L'/')
		{
			str_dir += L"/";
		}
		return str_dir;
	}
	static std::wstring get_base_filename_without_ext(const std::wstring& str_file_name)
	{
		boost::filesystem::wpath path(str_file_name);
		return path.stem().wstring();
	}
	static bool make_sure_dir_exist(const std::wstring& str_dir)
	{
		std::wstring str_dir_tmp = str_dir;
		ENSUREf(!str_dir_tmp.empty());
		if (str_dir_tmp[str_dir_tmp.size() - 1] == L'/')
		{
			str_dir_tmp = str_dir_tmp.substr(0, str_dir_tmp.size() - 1);
		}
		boost::filesystem::wpath path_dir(str_dir_tmp);
		if (!boost::filesystem::exists(path_dir))
		{
			ENSUREf(boost::filesystem::create_directories(path_dir));
		}
		return true;
	}
	static bool delete_file(const std::wstring& str_file)
	{
		boost::filesystem::wpath path_file(str_file);
		ENSUREf(boost::filesystem::remove(path_file));
		return true;
	}
	static bool file_exist(const std::wstring& str_file)
	{
		boost::filesystem::wpath path_dir(str_file);
		ENSUREf(boost::filesystem::exists(path_dir));
		return true;
	}
	static bool list_dir_files(const std::wstring& str_dir, std::vector<std::wstring>& vec_file_list)
	{
		boost::filesystem::wpath path_dir(str_dir);
		ENSUREf(boost::filesystem::is_directory(path_dir));
		ENSUREf(boost::filesystem::exists(path_dir));
		boost::filesystem::directory_iterator it_end;
		
		for (boost::filesystem::directory_iterator it(path_dir); it != it_end; it++)
		{
			if (boost::filesystem::is_directory(it->status()))
			{
				continue;
			}
			vec_file_list.push_back(it->path().wstring());
		}
		return true;
	}
	static bool list_dirs(const std::wstring& str_dir, std::vector<std::wstring>& vec_dir_list)
	{
		boost::filesystem::wpath path_dir(str_dir);
		ENSUREf(boost::filesystem::is_directory(path_dir));
		ENSUREf(boost::filesystem::exists(path_dir));
		boost::filesystem::directory_iterator it_end;

		for (boost::filesystem::directory_iterator it(path_dir); it != it_end; it++)
		{
			if (!boost::filesystem::is_directory(it->status()))
			{
				continue;
			}
			vec_dir_list.push_back(it->path().wstring());
		}
		return true;
	}
};