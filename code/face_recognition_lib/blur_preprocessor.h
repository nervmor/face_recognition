#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class blur_preprocessor : public preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"blur_preprocessor";
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			bool blur_handled_state = false;
			result res = sp_ctx->get_bool_value(BLUR_HANDLE_STATE, blur_handled_state);
			if (res == result_success)
			{
				if (blur_handled_state)
				{
					util_log::log(BLUR_PREPROCESSOR_TAG, "context blur state have been set. equalization repeatly.");
					return result_already_handled;
				}
			}
			
			res = picture_handler::blur(sp_pic_in, sp_pic_out);
			if (res != result_success)
			{
				util_log::log(BLUR_PREPROCESSOR_TAG, "blur fail with result[%s].", result_string(res));
				return res;
			}
			sp_ctx->set_bool_value(BLUR_HANDLE_STATE, true);
			util_log::logd(BLUR_PREPROCESSOR_TAG, "blur success and set context [%ws] to true.", BLUR_HANDLE_STATE);
			return result_success;
		}
	};
}