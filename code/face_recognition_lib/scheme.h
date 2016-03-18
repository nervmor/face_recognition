#pragma once
#include "../frame/include.h"
#include "../face_recognition_lib/define.h"

namespace face_recognition
{
	class session;
	class scheme : public boost::noncopyable
	{
	public:
		result train(const std::wstring& str_train_pic_dir,
			const std::vector<std::wstring>& vec_face_cascade_file, 
			const std::vector<std::wstring>& vec_left_eye_cascade_file,
			const std::vector<std::wstring>& vec_right_eye_cascade_file,
			const std::wstring& str_flandmark_model_file,
			unsigned int size_align_length);
		result predict(const std::wstring& str_pic, std::wstring& str_label, double& confidence);
	private:
		boost::shared_ptr<session> m_sp_session;
	};
}