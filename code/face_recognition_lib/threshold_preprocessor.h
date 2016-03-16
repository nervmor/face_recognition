#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class thres_hold_preprocessor : public preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"thres_hold_preprocessor";
		}
		virtual result init()
		{
			return result_success;
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			bool threshold_handle_state = false;
			result res = sp_ctx->get_bool_value(THRESHOLD_HANDLE_STATE, threshold_handle_state);
			if (res == result_success)
			{
				if (threshold_handle_state)
				{
					util_log::log(THRESHOLD_PREPROCESSOR_TAG, "context threshold state have been set. gray repeatly.");
					return result_already_handled;
				}
			}
			res = picture_handler::threshold(sp_pic_in, m_low_threshold, m_high_threshold, sp_pic_out);
			if (res != result_success)
			{
				util_log::log(THRESHOLD_PREPROCESSOR_TAG, "threshold fail with result[%s].", result_string(res));
				return res;
			}

			sp_ctx->set_bool_value(THRESHOLD_HANDLE_STATE, true);
			util_log::logd(THRESHOLD_PREPROCESSOR_TAG, "threshold success and set context [%ws] to true.", GRAY_HANDLE_STATE);
			return result_success;
		}
	public:
		thres_hold_preprocessor(int low_threshold, int high_threshold)
			: m_low_threshold(low_threshold)
			, m_high_threshold(high_threshold)
		{}
	private:
		int m_low_threshold;
		int m_high_threshold;
	};
}