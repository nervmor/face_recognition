#pragma once
#include "../frame/include.h"
#include "../face_recognition_lib/define.h"

namespace face_recognition
{
	class session;
	class scheme
	{
	public:
		result train(const std::wstring& str_train_pic_dir,
			const std::wstring& str_face_cascade_file, 
			const std::wstring& str_eye_cascade_file,
			const std::wstring& str_eye_cascade_file2,
			const std::wstring& str_flandmark_model_file,
			unsigned int min_face_area_width, unsigned int min_face_area_height, unsigned int max_face_area_width, unsigned int max_face_area_height,
			unsigned int size_align_length,
			int low_threshold, int high_threshold);
		result predict(const std::wstring& str_pic, std::wstring& str_label, double& confidence);
	private:
		boost::shared_ptr<session> m_sp_session;
	};
}