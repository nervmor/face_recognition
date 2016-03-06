#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class equalization_preprocessor : public preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"equalization_preprocessor";
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			bool equalization_handled_state = false;
			result res = sp_ctx->get_bool_value(EUQALIZATION_HANDLE_STATE, equalization_handled_state);
			if (res == result_success)
			{
				if (equalization_handled_state)
				{
					util_log::log(EQUALIZATION_PREPROCESSOR_TAG, "context equalization state have been set. equalization repeatly.");
					return result_already_handled;
				}
			}
			bool gray_handled_state = false;
			res = sp_ctx->get_bool_value(GRAY_HANDLE_STATE, gray_handled_state);
			if (res == result_success)
			{
				if (!gray_handled_state)
				{
					util_log::log(EQUALIZATION_PREPROCESSOR_TAG, "+-WARNING-+ context gray state have not been set. equalization should be done after it.");
				}
				else
				{
					util_log::logd(EQUALIZATION_PREPROCESSOR_TAG, "context gray have been set. it will do equalization now.");
				}
			}
			else
			{
				util_log::log(EQUALIZATION_PREPROCESSOR_TAG, "+-WARNING-+ context gray state get fail. the picture may not be gray before equalization");
			}
			res = picture_handler::equalization(sp_pic_in, sp_pic_out);
			if (res != result_success)
			{
				util_log::log(EQUALIZATION_PREPROCESSOR_TAG, "equalization fail with result[%s].", result_string(res));
				return res;
			}
			sp_ctx->set_bool_value(EUQALIZATION_HANDLE_STATE, true);
			util_log::logd(EQUALIZATION_PREPROCESSOR_TAG, "equalization success and set context [%ws] to true.", EUQALIZATION_HANDLE_STATE);
			return result_success;
		}
	};
}