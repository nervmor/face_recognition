#pragma once

#include "preprocessor_manager.h"
#include "train_task.h"
#include "misc.h"

namespace face_recognition
{
	class session : public boost::noncopyable
	{
	public:
		static result create(boost::shared_ptr<preprocessor_manager> sp_preprocessor_mgr, model_recognizer::recognizer_type model_type, boost::shared_ptr<session>& sp_session);
		static result create(boost::shared_ptr<preprocessor_manager> sp_preprocessor_mgr, model_recognizer::recognizer_type model_type, const std::wstring& str_file, boost::shared_ptr<session>& sp_session);
		void destroy();
	public:
		result train(boost::shared_ptr<train_task> sp_task);
		result predict(boost::shared_ptr<picture> sp_pic, const std::wstring& str_pic_filename, std::wstring& str_label, double& confidence);
		result save(const std::wstring& file);
	private:
		enum state
		{
			state_wait_for_train,
			state_wait_for_predict,
		};
	private:
		boost::shared_ptr<preprocessor_manager> m_sp_preprocessor_mgr;
		boost::shared_ptr<model_recognizer> m_sp_model_recognizer;
		state m_state;
	public:
		session();
		~session();
	};
}