#include "session.h"
#include "../frame/util_log.h"
#include "../frame/util_path.h"

namespace face_recognition
{
	result session::create(boost::shared_ptr<preprocessor_manager> sp_preprocessor_mgr, model_recognizer::recognizer_type model_type, boost::shared_ptr<session>& sp_session)
	{
		sp_session = boost::make_shared<session>();
		sp_session->m_sp_preprocessor_mgr = sp_preprocessor_mgr;
		model_recognizer::create(model_type, sp_session->m_sp_model_recognizer);
		util_log::logd(SESSION_TAG, "create session success with preprocessor count[%d]", sp_preprocessor_mgr->get_preprocess_count());
		return result_success;
	}

	face_recognition::result session::create(boost::shared_ptr<preprocessor_manager> sp_preprocessor_mgr, model_recognizer::recognizer_type model_type, const std::wstring& str_file, boost::shared_ptr<session>& sp_session)
	{
		if (!util_path::file_exist(str_file))
		{
			util_log::log(SESSION_TAG, "file[%ws] is not exist. create session fail.", str_file.c_str());
			return result_file_not_exist;
		}
		boost::shared_ptr<session> sp_session_x(boost::make_shared<session>());
		sp_session_x->m_sp_preprocessor_mgr = sp_preprocessor_mgr;
		result res = model_recognizer::create(model_type, str_file, sp_session_x->m_sp_model_recognizer);
		if (res != result_success)
		{
			util_log::log(SESSION_TAG, "create session fail for model_recognizer create fail with result[%s]", result_string(res));
			return res;
		}
		sp_session = sp_session_x;
		util_log::logd(SESSION_TAG, "create session by file success with preprocessor count[%d]", sp_preprocessor_mgr->get_preprocess_count());
		return result_success;
	}

	void session::destroy()
	{

	}

	result session::train(boost::shared_ptr<train_task> sp_task)
	{
		Assert(m_state == state_wait_for_train);
		if (m_state != state_wait_for_train)
		{
			util_log::log(SESSION_TAG, "session have already been trained.");
			return result_train_repeated;
		}
		unsigned int cnt = sp_task->get_picture_count();
		for (unsigned int i = 0; i != cnt; i++)
		{
			if (!sp_task->is_valid(i))
			{
				util_log::log(SESSION_TAG, "picture is invalid. ignore to train it.");
				continue;
			}
			boost::shared_ptr<picture> sp_pic = sp_task->get_picture(i);
			Assert(sp_pic);
			boost::shared_ptr<picture> sp_processed_pic;
			result res = m_sp_preprocessor_mgr->process(sp_pic, sp_processed_pic);
			if (res != result_success)
			{
				util_log::log(SESSION_TAG, "train picture fail. preprocess manager process it fail with result[%s]", result_string(res));
				sp_task->invalid_picture(i);
 				cv::imshow("train preprocess fail", sp_pic->data());
 				cv::waitKey();
			}
			else
			{
				util_log::logd(SESSION_TAG, "train preprcoessor manager process picture success.");
				sp_task->update_picture(i, sp_processed_pic);
//  				cv::imshow("train preprocess success", sp_processed_pic->data());
//  				cv::waitKey();
			}
		}
		m_sp_model_recognizer->train(sp_task);
		m_state = state_wait_for_predict;
		util_log::logd(SESSION_TAG, "train picture count[%d] completed.", sp_task->get_picture_count());
		return result_success;
	}

	result session::predict(boost::shared_ptr<picture> sp_pic, std::wstring& str_label, double& confidence)
	{
		Assert(m_state == state_wait_for_predict);
		if (m_state != state_wait_for_predict)
		{
			util_log::log(SESSION_TAG, "session have not been trained yet. do predict after training.");
			return result_train_repeated;
		}
		boost::shared_ptr<picture> sp_prcoessed_pic;
		result res = m_sp_preprocessor_mgr->process(sp_pic, sp_prcoessed_pic);
		if (res != result_success)
		{
			cv::imshow("predict preprocess fail", sp_pic->data());
			cv::waitKey();
			util_log::log(SESSION_TAG, "preprocess picture fail with result[%s] before predict.", result_string(res));
			return res;
		}
 		cv::imshow("predict preprocess success", sp_prcoessed_pic->data());
 		cv::waitKey();
		m_sp_model_recognizer->predict(sp_prcoessed_pic, str_label, confidence);
		return result_success;
	}

	result session::save(const std::wstring& file)
	{
		Assert(m_state == state_wait_for_predict);
		if (util_path::file_exist(file))
		{
			util_log::log(SESSION_TAG, "file[%ws] is already exist. it will recover it.", file.c_str());
		}
		m_sp_model_recognizer->save(file);
		util_log::log(SESSION_TAG, "save file[%ws] success", file.c_str());
		return result_success;
	}

	session::~session()
	{
		destroy();
	}

	session::session()
		: m_state(state_wait_for_train)
	{

	}

}


