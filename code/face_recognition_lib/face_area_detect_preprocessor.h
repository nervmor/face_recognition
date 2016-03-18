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
			boost::shared_ptr<mutiple_cascade_detector> sp_face_detector;
			result res = mutiple_cascade_detector::create(m_vec_cascade_file, sp_face_detector);
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
			
			boost::shared_ptr<pic_rect> sp_face_rect;
			m_sp_face_detector->detect_evaluat(sp_pic_in, sp_face_rect);
			if (!sp_face_rect)
			{
				util_log::log(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "there are no faces detected in the picture.");
				return result_no_face_detected;
			}
			util_log::logd(FACE_ARRA_DETECT_PREPROCESSOR_TAG, "detect face area [%d,%d,%d,%d]", 
				sp_face_rect->_x, sp_face_rect->_y, sp_face_rect->_x + sp_face_rect->_width, sp_face_rect->_y + sp_face_rect->_height);

			sp_ctx->set_value(FACE_AREA_RECT, sp_face_rect);
			return result_success;
		}
	public:
		face_area_detect_preprocessor(const std::vector<std::wstring>& vec_cascade_file)
			: m_vec_cascade_file(vec_cascade_file)
		{}
	private:
		std::vector<std::wstring> m_vec_cascade_file;
		boost::shared_ptr<mutiple_cascade_detector> m_sp_face_detector;
	};
}