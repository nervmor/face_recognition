#include "preprocessor_manager.h"
#include "../frame/util_log.h"
#include "context.h"

namespace face_recognition
{
	boost::shared_ptr<preprocessor_manager> preprocessor_manager::create()
	{
		boost::shared_ptr<preprocessor_manager> sp_manager(boost::make_shared<preprocessor_manager>());
		return sp_manager;
	}

	void preprocessor_manager::destroy()
	{
	}

	result preprocessor_manager::add_preprocessor(boost::shared_ptr<preprocessor> sp_preprocessor)
	{
		result res = sp_preprocessor->init();
		if (res != result_success)
		{
			util_log::log(PREPROCESSOR_MANAGER_TAG, "prepreprocessor[%ws] init fail with result[%s]..", sp_preprocessor->name().c_str(), result_string(res));
			return res;
		}
		util_log::logd(PREPROCESSOR_MANAGER_TAG, "add_preprocessor[%ws].", sp_preprocessor->name().c_str());
		m_vec_preprocessor.push_back(sp_preprocessor);
		return result_success;
	}

	result preprocessor_manager::process(boost::shared_ptr<picture> sp_pic_in, const std::wstring& str_pic_filename, boost::shared_ptr<picture>& sp_pic_out)
	{
		result res = result_success;
		boost::shared_ptr<context> sp_ctx = context::create();
		sp_ctx->set_string_value(PICTURE_FILENAME_CONTEXT_KEY, str_pic_filename);
		boost::shared_ptr<picture> sp_pic_in_x = sp_pic_in;
		boost::shared_ptr<picture> sp_pic_out_x;
		for (std::vector<boost::shared_ptr<preprocessor> >::iterator it = m_vec_preprocessor.begin(); it != m_vec_preprocessor.end(); it++)
		{
			boost::shared_ptr<preprocessor> sp_preprocessor(*it);
			util_log::logd(PREPROCESSOR_MANAGER_TAG, "start preprocess[%ws] picture[%ws]", sp_preprocessor->name().c_str(), str_pic_filename.c_str());
			res = sp_preprocessor->process(sp_pic_in_x, sp_ctx, sp_pic_out_x);
			if (res != result_success)
			{
				util_log::log(PREPROCESSOR_MANAGER_TAG, "preprocessor [%ws] process picture [%ws] fail with result[%s].", sp_preprocessor->name().c_str(), str_pic_filename.c_str(), result_string(res));
				break;
			}
			sp_pic_in_x = sp_pic_out_x;
		}
		if (res == result_success)
		{
			sp_pic_out = sp_pic_out_x;
			util_log::logd(PREPROCESSOR_MANAGER_TAG, "preprocess picture[%ws] success", str_pic_filename.c_str());
		}
		return res;
	}

	unsigned int preprocessor_manager::get_preprocess_count()
	{
		return m_vec_preprocessor.size();
	}

}


