#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class edge_preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"edge_preprocessor";
		}
		virtual result init()
		{
			return result_success;
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			bool edge_handled_state = false;
			result res = sp_ctx->get_bool_value(EDGE_HANDLE_STATE, edge_handled_state);
			if (res == result_success)
			{
				if (edge_handled_state)
				{
					util_log::log(EDGE_PREPROCESSOR_TAG, "context edge state have been set. edge repeatly.");
					return result_already_handled;
				}
			}
			bool gray_handled_state = false;
			res = sp_ctx->get_bool_value(GRAY_HANDLE_STATE, gray_handled_state);
			if (res == result_success)
			{
				if (!gray_handled_state)
				{
					util_log::log(EDGE_PREPROCESSOR_TAG, "+-WARNING-+ context gray state have not been set. edge should be done after it.");
				}
				else
				{
					util_log::logd(EDGE_PREPROCESSOR_TAG, "context gray have been set. it will do edge now.");
				}
			}
			else
			{
				util_log::log(EDGE_PREPROCESSOR_TAG, "+-WARNING-+ context gray state get fail. the picture may not be gray before edge");
			}

			bool blur_handled_state = false;
			res = sp_ctx->get_bool_value(BLUR_HANDLE_STATE, blur_handled_state);
			if (res == result_success)
			{
				if (!blur_handled_state)
				{
					util_log::log(EDGE_PREPROCESSOR_TAG, "+-WARNING-+ context blur state have not been set. edge should be done after it.");
				}
				else
				{
					util_log::logd(EDGE_PREPROCESSOR_TAG, "context blur have been set. it will do edge now.");
				}
			}
			else
			{
				util_log::log(EDGE_PREPROCESSOR_TAG, "+-WARNING-+ context blur state get fail. the picture may not be blur before edge");
			}

			res = picture_handler::edge(sp_pic_in, m_low_threshold, m_high_threshold, sp_pic_out);
			if (res != result_success)
			{
				util_log::log(EDGE_PREPROCESSOR_TAG, "edge fail with result[%s].", result_string(res));
				return res;
			}
			sp_ctx->set_bool_value(EDGE_HANDLE_STATE, true);
			util_log::logd(EDGE_PREPROCESSOR_TAG, "edge success and set context [%ws] to true.", EDGE_HANDLE_STATE);
			return result_success;
		}
	public:
		edge_preprocessor(int low_threshold, int high_threshold)
			: m_low_threshold(low_threshold)
			, m_high_threshold(high_threshold)
		{}
	private:
		int m_low_threshold;
		int m_high_threshold;
	};
}