#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class face_contour_mask_preprocessor : public preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"face_contour_mask_preprocessor";
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			bool face_contour_mask_state_state = false;
			result res = sp_ctx->get_bool_value(FACE_COUNTOUR_MASK_STATE, face_contour_mask_state_state);
			if (res == result_success)
			{
				if (face_contour_mask_state_state)
				{
					util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "context face_contour_mask state state have been set. do it repeatly.");
					return result_already_handled;
				}
			}

			bool gray_handled_state = false;
			res = sp_ctx->get_bool_value(GRAY_HANDLE_STATE, gray_handled_state);
			if (res == result_success)
			{
				if (!gray_handled_state)
				{
					util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "+-WARNING-+ context gray state have not been set. face_contour_mask should be done after it.");
				}
				else
				{
					util_log::logd(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "context gray have been set.");
				}
			}
			else
			{
				util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "+-WARNING-+ context gray state get fail. the picture may not be gray before face_contour_mask");
			}

			bool equalization_handled_state = false;
			res = sp_ctx->get_bool_value(EUQALIZATION_HANDLE_STATE, equalization_handled_state);
			if (res == result_success)
			{
				if (!equalization_handled_state)
				{
					util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "+-WARNING-+ context equalization state have not been set. face_contour_mask should be done after it.");
				}
				else
				{
					util_log::logd(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "context equalization state have been set.");
				}
			}
			else
			{
				util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "+-WARNING-+ context equalization state get fail. the picture may not be equalization before face_contour_mask");
			}

			boost::shared_ptr<picture> sp_pic_x;
			bool threshold_handled_state = false;
			res = sp_ctx->get_bool_value(THRESHOLD_HANDLE_STATE, threshold_handled_state);
			if (threshold_handled_state)
			{
				util_log::logd(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "context threshold state have been set.");
				sp_pic_x = sp_pic_in;
			}
			else
			{
				util_log::logd(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "context threshold state have not been set. threshold the picture before find contour.");
				res = picture_handler::threshold(sp_pic_in, m_low_threshold, m_high_threshold, sp_pic_x);
				if (res != result_success)
				{
					util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "before find contour do threshod fail with result[%s].", result_string(res));
					return res;
				}
			}
			std::vector<pic_area> vec_pic_area;
			res = contours_detector::detect_contours(sp_pic_x, vec_pic_area);
			if (res != result_success)
			{
				util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "detect_contours fail with result[%s].", result_string(res));
				return res;
			}

			if (vec_pic_area.empty())
			{
				util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "detect_contours get no area.");
				return result_success;
			}

			pic_area& max_acreage_area = *vec_pic_area.begin();
			double max_acreage = max_acreage_area.area_size();
			for (std::vector<pic_area>::iterator it = vec_pic_area.begin(); it != vec_pic_area.end(); it++)
			{
				pic_area& area = *it;
				double acreage = area.area_size();
				if (acreage > max_acreage)
				{
					max_acreage_area = *it;
					max_acreage = acreage;
				}
			}
			util_log::logd(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "detect_contours get area count[%d], the biggest area arecage[%f]", vec_pic_area.size(), max_acreage);

			int_64 pic_arecage = sp_pic_in->width() * sp_pic_in->height();
			if (max_acreage < pic_arecage / 2)
			{
				util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "detect_contours get area arecage[%f] < picture arecage[%lld]/2. give up masking operator.", max_acreage, pic_arecage);
				return result_success;
			}

			res = picture_handler::mask(sp_pic_in, max_acreage_area, sp_pic_out);
			if (res != result_success)
			{
				util_log::log(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "do mask fail with result[%s].", result_string(res));
				return res;
			}
	
			sp_ctx->set_bool_value(FACE_COUNTOUR_MASK_STATE, true);
			util_log::logd(FACE_COUNTOUR_MASK_PREPROCESSOR_TAG, "face_contour_mask success and set context [%ws] to true.", FACE_COUNTOUR_MASK_STATE);
			
			return result_success;
		}
	public:
		face_contour_mask_preprocessor(int low_threshold, int high_threshold)
			: m_low_threshold(low_threshold)
			, m_high_threshold(high_threshold)
		{}
	private:
		int m_low_threshold;
		int m_high_threshold;
	};
}