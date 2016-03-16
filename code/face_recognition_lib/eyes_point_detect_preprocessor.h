#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class eyes_point_detect_preprocessor : public preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"eyes_point_detect_preprocessor";
		}
		virtual result init()
		{
			boost::shared_ptr<face_feature_detector> sp_detector;
			result res = face_feature_detector::create(m_str_flandmark_model_file, sp_detector);
			if (res != result_success)
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "face_feature_detector create fail with result[%s]", result_string(res));
				return res;
			}
			m_sp_feature_detector = sp_detector;

			boost::shared_ptr<cascade_detector> sp_cascade_detector;
			res = cascade_detector::create(m_str_eyes_cascade_file, sp_cascade_detector);
			if (res != result_success)
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "eye cascade_detector create fail with result[%s]", result_string(res));
				return res;
			}
			m_sp_eye_cascade_detector = sp_cascade_detector;
			return result_success;
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			Assert(m_sp_feature_detector);
			bool face_area_detect_handled_state = false;

			bool gray_handled_state = false;
			result res = sp_ctx->get_bool_value(GRAY_HANDLE_STATE, gray_handled_state);
			if (res == result_success)
			{
				if (!gray_handled_state)
				{
					util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "+-WARNING-+ context gray state have not been set. face_area_detect should be done after it.");
				}
				else
				{
					util_log::logd(EYES_POINT_DETECT_PREPROCESSOR_TAG, "context gray have been set.");
				}
			}
			else
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "+-WARNING-+ context gray state get fail. the picture may not be gray before face_area_detect");
			}

			boost::shared_ptr<pic_rect> sp_rect;
			res = sp_ctx->get_value(FACE_AREA_RECT, sp_rect);
			if (res != result_success)
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "unable to detects eyes point for geting context [%ws] fail.", FACE_AREA_RECT);
				return res;
			}
			boost::shared_ptr<pic_point> sp_left_eye;
			boost::shared_ptr<pic_point> sp_right_eye;
			res = get_eyes_postion(sp_pic_in, sp_rect, sp_left_eye, sp_right_eye);
			if (res != result_success)
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "get_eyes_postion fail with result [%s]", result_string(res));
				return res;
			}
			sp_ctx->set_value(LEFT_EYE_POINT, sp_left_eye);
			sp_ctx->set_value(RIGHT_EYE_POINT, sp_right_eye);

			util_log::logd(EYES_POINT_DETECT_PREPROCESSOR_TAG, "two eyes point get success. set it [%d,%d][%d,%d] to context[%ws][%ws]", 
				sp_left_eye->_x, sp_left_eye->_y, sp_right_eye->_x, sp_right_eye->_y, LEFT_EYE_POINT, RIGHT_EYE_POINT);
			return result_success;
		}
		result get_eyes_postion(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<pic_rect> sp_face_rect, boost::shared_ptr<pic_point>& sp_left_eye, boost::shared_ptr<pic_point>& sp_right_eye)
		{
			boost::shared_ptr<face_feature> sp_feature;
			result res = m_sp_feature_detector->detect_feature(sp_pic_in, *sp_face_rect.get(), sp_feature);
			if (res != result_success)
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "detect_feature fail with result[%s]", result_string(res));
				return res;
			}

			pic_point feature_pic_left_eye((sp_feature->_left_eye_left._x + sp_feature->_left_eye_right._x) / 2, (sp_feature->_left_eye_left._y + sp_feature->_left_eye_right._y) / 2);
			pic_point feature_pic_right_eye((sp_feature->_right_eye_left._x + sp_feature->_right_eye_right._x) / 2, (sp_feature->_right_eye_left._y + sp_feature->_right_eye_right._y) / 2);

			cv::Rect face_rect;
			face_rect.x = sp_face_rect->_x;
			face_rect.y = sp_face_rect->_y;
			face_rect.width = sp_face_rect->_width;
			face_rect.height = sp_face_rect->_height;

			cv::Mat face = cv::Mat(sp_pic_in->data(), face_rect);
			const float EYE_SX = 0.16f;//x
			const float EYE_SY = 0.26f;//y
			const float EYE_SW = 0.30f;//width
			const float EYE_SH = 0.28f;//height

			int leftX = cvRound(face.cols * EYE_SX);
			int topY = cvRound(face.rows * EYE_SY);
			int widthX = cvRound(face.cols * EYE_SW);
			int heightY = cvRound(face.rows * EYE_SH);
			int rightX = cvRound(face.cols * (1.0 - EYE_SX - EYE_SW));

			cv::Mat topLeftOfFace = face(cv::Rect(leftX, topY, widthX, heightY));
			cv::Mat topRightOfFace = face(cv::Rect(rightX, topY, widthX, heightY));

			boost::shared_ptr<picture> sp_face_left_eye = picture::create(topLeftOfFace);
			boost::shared_ptr<picture> sp_face_right_eye = picture::create(topRightOfFace);
	
			boost::shared_ptr<pic_rect> sp_left_eye_rect;
			res = m_sp_eye_cascade_detector->detect_largest(sp_face_left_eye, sp_left_eye_rect);
			if (res != result_success)
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "eye cascade_detector detect_largest fail with result[%s]", result_string(res));
				return res;
			}
			if (!sp_left_eye_rect)
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "eye cascade_detector detect no left eye");
				return result_no_eyes_fetected;
			}

			boost::shared_ptr<pic_rect> sp_right_eye_rect;
			res = m_sp_eye_cascade_detector->detect_largest(sp_face_right_eye, sp_right_eye_rect);
			if (res != result_success)
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "eye cascade_detector detect_largest fail with result[%s]", result_string(res));
				return res;
			}
			if (!sp_right_eye_rect)
			{
				util_log::log(EYES_POINT_DETECT_PREPROCESSOR_TAG, "eye cascade_detector detect no right eye");
				return result_no_eyes_fetected;
			}

			pic_point cascade_pic_left_eye(leftX + sp_left_eye_rect->_x + sp_face_rect->_x + sp_left_eye_rect->_width / 2, topY + sp_left_eye_rect->_y + sp_face_rect->_y + sp_left_eye_rect->_height / 2);
			pic_point cascade_pic_right_eye(rightX + sp_right_eye_rect->_x + sp_face_rect->_x + sp_right_eye_rect->_width / 2, topY + sp_right_eye_rect->_y + sp_face_rect->_y + sp_right_eye_rect->_height / 2);

			unsigned int left_eye_x_delta = cascade_pic_left_eye._x > feature_pic_left_eye._x ? cascade_pic_left_eye._x - feature_pic_left_eye._x : feature_pic_left_eye._x - cascade_pic_left_eye._x;
			unsigned int left_eye_y_delta = cascade_pic_left_eye._y > feature_pic_left_eye._y ? cascade_pic_left_eye._y - feature_pic_left_eye._y : feature_pic_left_eye._y - cascade_pic_left_eye._y;
			unsigned int right_eye_x_delta = cascade_pic_right_eye._x > feature_pic_right_eye._x ? cascade_pic_right_eye._x - feature_pic_right_eye._x : feature_pic_right_eye._x - cascade_pic_right_eye._x;
			unsigned int right_eye_y_delta = cascade_pic_right_eye._y > feature_pic_right_eye._y ? cascade_pic_right_eye._y - feature_pic_right_eye._y : feature_pic_right_eye._y - cascade_pic_right_eye._y;

			unsigned int x_range_delta = cvRound(sp_face_rect->_width * EYE_SW / 6);
			unsigned int y_range_delta = cvRound(sp_face_rect->_height * EYE_SH / 6);

			util_log::logd(FACE_ALIGN_PREPROCESSOR_TAG, "flandmark eyes [%d,%d] [%d,%d] --- cascade eyes [%d,%d][%d,%d] --->delta [%d,%d][%d,%d]<---standard delta[%d,%d]",
				feature_pic_left_eye._x, feature_pic_left_eye._y, feature_pic_right_eye._x, feature_pic_right_eye._y,
				cascade_pic_left_eye._x, cascade_pic_left_eye._y, cascade_pic_right_eye._x, cascade_pic_right_eye._y,
				left_eye_x_delta, left_eye_y_delta, right_eye_x_delta, right_eye_y_delta,
				x_range_delta, y_range_delta);

			if (left_eye_x_delta > x_range_delta ||
				right_eye_x_delta > x_range_delta ||
				left_eye_y_delta > y_range_delta ||
				right_eye_y_delta > y_range_delta)
			{
				util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "cascade and flandmark delta left[%d,%d]--right[%d,%d] don't within the standard delta[%d,%d]", 
					left_eye_x_delta, left_eye_y_delta, right_eye_x_delta, right_eye_y_delta, x_range_delta, y_range_delta);
				return result_no_eyes_fetected;
			}
 			sp_left_eye = boost::make_shared<pic_point>((cascade_pic_left_eye._x + feature_pic_left_eye._x) / 2, (cascade_pic_left_eye._y + feature_pic_left_eye._y) / 2);
			sp_right_eye = boost::make_shared<pic_point>((cascade_pic_right_eye._x + feature_pic_right_eye._x) / 2, (cascade_pic_right_eye._y + feature_pic_right_eye._y) / 2);
			return result_success;
		}
	public:
		eyes_point_detect_preprocessor(const std::wstring& str_eyes_cascade_file,
			const std::wstring& str_flandmark_model_file)
			: m_str_eyes_cascade_file(str_eyes_cascade_file)
			, m_str_flandmark_model_file(str_flandmark_model_file)
		{

		}
		~eyes_point_detect_preprocessor()
		{}
	private:
		std::wstring m_str_eyes_cascade_file;
		std::wstring m_str_flandmark_model_file;
		boost::shared_ptr<face_feature_detector> m_sp_feature_detector;
		boost::shared_ptr<cascade_detector> m_sp_eye_cascade_detector;
	};
}