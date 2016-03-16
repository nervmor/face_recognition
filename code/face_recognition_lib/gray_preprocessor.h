#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class gray_preprocessor : public preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"gray_preprocessor";
		}
		virtual result init()
		{
			return result_success;
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			bool gray_handle_state = false;
			result res = sp_ctx->get_bool_value(GRAY_HANDLE_STATE, gray_handle_state);
			if (res == result_success)
			{
				if (gray_handle_state)
				{
					util_log::log(GRAY_PREPROCESSOR_TAG, "context gray state have been set. gray repeatly.");
					return result_already_handled;
				}
			}
			res = picture_handler::gray(sp_pic_in, sp_pic_out);
			if (res != result_success)
			{
				util_log::log(GRAY_PREPROCESSOR_TAG, "gray fail with result[%s].", result_string(res));
				return res;
			}
			
			sp_ctx->set_bool_value(GRAY_HANDLE_STATE, true);
			util_log::logd(GRAY_PREPROCESSOR_TAG, "gray success and set context [%ws] to true.", GRAY_HANDLE_STATE);
			return result_success;
		}
	};
}