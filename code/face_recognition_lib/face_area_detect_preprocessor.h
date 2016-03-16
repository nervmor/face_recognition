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
		virtual result init()
		{
			boost::shared_ptr<cascade_detector> sp_face_detector;
			result res = cascade_detector::create(m_str_cascade_file, sp_face_detector);
			if (res != result_success)
			{
				util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "cascade_detector create fail with result[%s].", result_string(res));
				return res;
			}
			m_sp_face_detector = sp_face_detector;
			return result_success;
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			Assert(m_sp_face_detector);
			bool gray_handled_state = false;
			result res = sp_ctx->get_bool_value(GRAY_HANDLE_STATE, gray_handled_state);
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
			
			std::vector<pic_rect> vec_face_rect;
			res = m_sp_face_detector->detect(sp_pic_in, vec_face_rect);
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
			
			return result_success;
		}
	public:
		face_area_detect_preprocessor(const std::wstring& str_cascade_file)
					: m_str_cascade_file(str_cascade_file)
		{}
	private:
		std::wstring m_str_cascade_file;
		boost::shared_ptr<cascade_detector> m_sp_face_detector;
	};
}