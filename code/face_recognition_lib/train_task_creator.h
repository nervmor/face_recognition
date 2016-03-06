#pragma once
#include "train_task.h"
#include "../frame/util_path.h"
#include "../frame/util_log.h"

namespace face_recognition
{
	class train_task_creator
	{
	public:
		static result create_task(const std::wstring& str_dir, boost::shared_ptr<train_task>& sp_train_task)
		{
			std::vector<std::wstring> vec_dirs;
			if (!util_path::list_dirs(str_dir, vec_dirs))
			{
				return result_dir_not_exist;
			}
			std::map<std::wstring, std::vector<std::wstring> > map_label_vecfile;
			for (std::vector<std::wstring>::iterator it = vec_dirs.begin(); it != vec_dirs.end(); it++)
			{
				const std::wstring& str_dir = *it;
				std::vector<std::wstring> vec_files;
				if (!util_path::list_dir_files(str_dir, vec_files))
				{
					util_log::log(TRAIN_TASK_CREATOR_TAG, "list_dir_files dir[%ws] fail.", str_dir.c_str());
					continue;
				}
				std::wstring str_label = util_path::get_base_filename_without_ext(str_dir);
				std::vector<std::wstring> vec_pic_file;
				util_log::logd(TRAIN_TASK_CREATOR_TAG, "create label[%ws].", str_label.c_str());
				for (std::vector<std::wstring>::iterator itx = vec_files.begin(); itx != vec_files.end(); itx++)
				{
					const std::wstring& str_pic_file = *itx;
					vec_pic_file.push_back(str_pic_file);
					util_log::logd(TRAIN_TASK_CREATOR_TAG, "put picture[%ws] to label[%ws].", str_pic_file.c_str(), str_label.c_str());
				}
				map_label_vecfile.insert(std::make_pair(str_label, vec_pic_file));
			}
			return train_task::create(map_label_vecfile, sp_train_task);
		}
	};
}