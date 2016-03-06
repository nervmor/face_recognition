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

	void preprocessor_manager::add_preprocessor(boost::shared_ptr<preprocessor> sp_preprocessor)
	{
		util_log::logd(PREPROCESSOR_MANAGER_TAG, "add_preprocessor[%ws].", sp_preprocessor->name().c_str());
		m_vec_preprocessor.push_back(sp_preprocessor);
	}

	result preprocessor_manager::process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out)
	{
		result res = result_success;
		boost::shared_ptr<context> sp_ctx = context::create();
		boost::shared_ptr<picture> sp_pic_in_x = sp_pic_in;
		boost::shared_ptr<picture> sp_pic_out_x;
		for (std::vector<boost::shared_ptr<preprocessor> >::iterator it = m_vec_preprocessor.begin(); it != m_vec_preprocessor.end(); it++)
		{
			boost::shared_ptr<preprocessor> sp_preprocessor(*it);
			util_log::logd(PREPROCESSOR_MANAGER_TAG, "preprocessor [%ws] starting to process.", sp_preprocessor->name().c_str());
			res = sp_preprocessor->process(sp_pic_in_x, sp_ctx, sp_pic_out_x);
			if (res != result_success)
			{
				util_log::log(PREPROCESSOR_MANAGER_TAG, "preprocessor [%ws] process fail with result[%s].", sp_preprocessor->name().c_str(), result_string(res));
				break;
			}
			sp_pic_in_x = sp_pic_out_x;
		}
		if (res == result_success)
		{
			sp_pic_out = sp_pic_out_x;
			util_log::logd(PREPROCESSOR_MANAGER_TAG, "process success.");
		}
		return res;
	}

	unsigned int preprocessor_manager::get_preprocess_count()
	{
		return m_vec_preprocessor.size();
	}

}


