#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class face_area_detect_preprocessor : public preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"face_area_detect_preprocessor";
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			bool face_area_detect_handled_state = false;
			result res = sp_ctx->get_bool_value(FACE_AREA_DETECT_STATE, face_area_detect_handled_state);
			if (res == result_success)
			{
				if (face_area_detect_handled_state)
				{
					util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "context face_area_detect state have been set. face_area_detect repeatly.");
					return result_already_handled;
				}
			}

			bool gray_handled_state = false;
			res = sp_ctx->get_bool_value(GRAY_HANDLE_STATE, gray_handled_state);
			if (res == result_success)
			{
				if (!gray_handled_state)
				{
					util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "+-WARNING-+ context gray state have not been set. face_area_detect should be done after it.");
				}
				else
				{
					util_log::logd(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "context gray have been set.");
				}
			}
			else
			{
				util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "+-WARNING-+ context gray state get fail. the picture may not be gray before face_area_detect");
			}

			bool equalization_handled_state = false;
			res = sp_ctx->get_bool_value(EUQALIZATION_HANDLE_STATE, equalization_handled_state);
			if (res == result_success)
			{
				if (!equalization_handled_state)
				{
					util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "+-WARNING-+ context equalization state have not been set. face_area_detect should be done after it.");
				}
				else
				{
					util_log::logd(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "context equalization state have been set.");
				}
			}
			else
			{
				util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "+-WARNING-+ context equalization state get fail. the picture may not be equalization before face_area_detect");
			}
			
			boost::shared_ptr<cascade_detector> sp_face_detector;
			res = cascade_detector::create(m_str_cascade_file, sp_face_detector);
			if (res != result_success)
			{
				util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "cascade_detector create fail with result[%s].", result_string(res));
				return res;
			}
			std::vector<pic_rect> vec_face_rect;
			res = sp_face_detector->detect_face(sp_pic_in, vec_face_rect, m_min_width, m_min_height, m_max_width, m_max_height);
			if (res != result_success)
			{
				util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "detect_face fail with result[%s].", result_string(res));
				return res;
			}

			if (vec_face_rect.empty())
			{
				util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "there are no faces detected in the picture.");
				return result_no_face_detected;
			}

			pic_rect& max_acreage_rect = *vec_face_rect.begin();
			int_64 max_acreage = max_acreage_rect.cacl_acreage();
			for (std::vector<pic_rect>::iterator it = vec_face_rect.begin(); it != vec_face_rect.end(); it++)
			{
				pic_rect& rect = *it;
				int_64 acreage = rect.cacl_acreage();
				if (acreage > max_acreage)
				{
					max_acreage = acreage;
					max_acreage_rect = *it;
				}
			}
			util_log::logd(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "detect face area count[%d], then get the biggest area[%lld] postion[%d,%d,%d,%d]to cut", 
				vec_face_rect.size(), max_acreage, max_acreage_rect._x, max_acreage_rect._y, max_acreage_rect._x + max_acreage_rect._width, max_acreage_rect._y + max_acreage_rect._height);

			boost::shared_ptr<pic_rect> sp_rect(boost::make_shared<pic_rect>(max_acreage_rect._x, max_acreage_rect._y, max_acreage_rect._width, max_acreage_rect._height));
			sp_ctx->set_value(FACE_AREA_RECT, sp_rect);

// 			res = picture_handler::cut(sp_pic_in, max_acreage_rect._x, max_acreage_rect._y, max_acreage_rect._width, max_acreage_rect._height, sp_pic_out);
// 			if (res != result_success)
// 			{
// 				util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "cut picture by max face area fail with result[%s].", result_string(res));
// 				return res;
// 			}
			
			sp_ctx->set_bool_value(FACE_AREA_DETECT_STATE, true);
			util_log::logd(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "detect_face_area success and set context [%ws] to true.", FACE_AREA_DETECT_STATE);
			
			return result_success;
		}
	public:
		face_area_detect_preprocessor(const std::wstring& str_cascade_file, 
									  unsigned int min_width, 
									  unsigned int min_height,
									  unsigned int max_width,
									  unsigned int max_height)
					: m_str_cascade_file(str_cascade_file)
					, m_min_width(min_width)
					, m_min_height(min_height)
					, m_max_width(max_width)
					, m_max_height(max_height)
		{}
	private:
		std::wstring m_str_cascade_file;
		unsigned int m_min_width;
		unsigned int m_min_height;
		unsigned int m_max_width;
		unsigned int m_max_height;
	};
}