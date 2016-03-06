#pragma once

#include "../frame/include.h"
#include "../frame/util_path.h"
#include "../frame/util_string.h"
#include "picture.h"
#include "../frame/util_log.h"
namespace face_recognition
{
	class train_task : public boost::noncopyable
	{
	public:
		static result create(const std::map<std::wstring, std::vector<std::wstring> >& map_label_vecfile, boost::shared_ptr<train_task>& sp_task)
		{
			result ret = result_file_not_exist;
			boost::shared_ptr<train_task> sp_task_x(boost::make_shared<train_task>());
			for (std::map<std::wstring, std::vector<std::wstring> >::const_iterator it = map_label_vecfile.begin(); it != map_label_vecfile.end(); it++)
			{
				const std::wstring& str_label = it->first;
				const std::vector<std::wstring>& vec_pic_file = it->second;

				for (std::vector<std::wstring>::const_iterator itx = vec_pic_file.begin(); itx != vec_pic_file.end(); itx++)
				{
					const std::wstring& str_pic_file = *itx;
					
					result res = sp_task_x->add(str_label, str_pic_file);
					if (res == result_success)
					{
						ret = result_success;
						util_log::log(TRAIN_TASK_TAG, "add picture[%ws] success.", str_pic_file.c_str());
					}
					else
					{
						util_log::log(TRAIN_TASK_TAG, "add picture[%ws] fail with result[%s] in create.", str_pic_file.c_str(), result_string(res));
					}
				}
			}
			if (ret == result_success)
			{
				sp_task = sp_task_x;
			}
			return ret;
		}
		void destroy()
		{
			
		}
		result add(const std::wstring& str_label, const std::wstring& str_pic_file)
		{
			boost::shared_ptr<picture> sp_picture;
			result res = picture::load(str_pic_file, sp_picture);
			if (res != result_success)
			{
				util_log::log(TRAIN_TASK_TAG, "load picture[%ws] fail with result[%s] in add.", str_pic_file.c_str(), result_string(res));
				return res;
			}
			std::map<std::wstring, std::vector<unsigned int> >::iterator it = m_map_label_vecpicindex.find(str_label);
			if (it == m_map_label_vecpicindex.end())
			{
				std::vector<unsigned int> vec_pic_index;
				m_vec_picture.push_back(sp_picture);
				vec_pic_index.push_back(m_vec_picture.size() - 1);
				m_map_label_vecpicindex.insert(std::make_pair(str_label, vec_pic_index));
			}
			else
			{
				std::vector<unsigned int>& vec_pic_index = it->second;
				m_vec_picture.push_back(sp_picture);
				vec_pic_index.push_back(m_vec_picture.size() - 1);
			}
			return result_success;
		}
		void get(std::vector<cv::Mat>& images, std::vector<int>& labels, std::map<int, std::wstring>& map_label_str)
		{
			int label_index = 0;
			for (std::map<std::wstring, std::vector<unsigned int> >::iterator it = m_map_label_vecpicindex.begin(); it != m_map_label_vecpicindex.end(); it++)
			{
				const std::wstring& str_label = it->first;
				std::vector<unsigned int>& vec_pic_index = it->second;
				
				bool valid_pic = false;
				for (std::vector<unsigned int>::iterator itx = vec_pic_index.begin(); itx != vec_pic_index.end(); itx++)
				{
					unsigned int pic_index = *itx;
					if (is_valid(pic_index))
					{
						valid_pic = true;
						break;
					}
				}
				if (!valid_pic)
				{
					continue;
				}

				map_label_str.insert(std::make_pair(++label_index, str_label));

				for (std::vector<unsigned int>::iterator itx = vec_pic_index.begin(); itx != vec_pic_index.end(); itx++)
				{
					unsigned int index = *itx;
					if (!is_valid(index))
					{
						continue;
					}
					boost::shared_ptr<picture> sp_pic = m_vec_picture[index];
					images.push_back(sp_pic->data());
					labels.push_back(label_index);
				}
			}
		}
		unsigned int get_picture_count()
		{
			return m_vec_picture.size();
		}
		boost::shared_ptr<picture> get_picture(unsigned int index)
		{
			Assert(index < m_vec_picture.size());
			return m_vec_picture[index];
		}
		void update_picture(unsigned int index, boost::shared_ptr<picture> sp_pic)
		{
			Assert(index < m_vec_picture.size());
			m_vec_picture[index]->destroy();
			m_vec_picture[index] = sp_pic;
		}
		bool is_valid(unsigned int i)
		{
			std::set<unsigned int>::iterator it = m_vec_invalid_index.find(i);
			return it == m_vec_invalid_index.end();
		}
		void invalid_picture(unsigned int index)
		{
			m_vec_invalid_index.insert(index);
		}
	private:
		std::map<std::wstring, std::vector<unsigned int> > m_map_label_vecpicindex;
		std::vector<boost::shared_ptr<picture>> m_vec_picture;
		std::set<unsigned int> m_vec_invalid_index;
	public:
		~train_task()
		{
			destroy();
		}
	};
}