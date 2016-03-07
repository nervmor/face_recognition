#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class face_align_preprocessor : public preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"face_align_preprocessor";
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			bool b_face_align_handled_state = false;
			result res = sp_ctx->get_bool_value(FACE_ALIGN_STATE, b_face_align_handled_state);
			if (res == result_success)
			{
				if (b_face_align_handled_state)
				{
					util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "context face_align state have been set. do repeatly.");
					return result_already_handled;
				}
			}
			boost::shared_ptr<face_feature_detector> sp_detector;
			res = face_feature_detector::create(m_str_flandmark_model_file, sp_detector);
			if (res != result_success)
			{
				util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "face_feature_detector create fail with result[%s]", result_string(res));
				return res;
			}
			boost::shared_ptr<pic_rect> sp_rect;
			res = sp_ctx->get_value(FACE_AREA_RECT, sp_rect);
			if (res != result_success)
			{
				util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "unable to detects face feature for geting [%ws] fail.", FACE_AREA_RECT);
				return res;
			}
			boost::shared_ptr<face_feature> sp_feature;
			res = sp_detector->detect_feature(sp_pic_in, *sp_rect.get(), sp_feature);
			if (res != result_success)
			{
				util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "detect_feature fail with result[%s]", result_string(res));
				return res;
			}
			cv::Point2f leftEye(cv::Point((sp_feature->_left_eye_right._x - sp_feature->_left_eye_left._x) / 2, (sp_feature->_left_eye_right._y - sp_feature->_left_eye_left._y) / 2));
			cv::Point2f rightEye(cv::Point((sp_feature->_right_eye_right._x - sp_feature->_right_eye_left._x) / 2, (sp_feature->_right_eye_right._y - sp_feature->_right_eye_left._y) / 2));

			// 获取两眼中心点 Get the center between the 2 eyes.
			cv::Point2f eyesCenter;
			eyesCenter.x = (leftEye.x + rightEye.x) * 0.5f;
			eyesCenter.y = (leftEye.y + rightEye.y) * 0.5f;
			// 获取两眼的角度Get the angle between the 2 eyes.
			double dy = (rightEye.y - leftEye.y);
			double dx = (rightEye.x - leftEye.x);
			double len = sqrt(dx*dx + dy*dy);
			// 将弧度转为角度Convert Radians to Degrees.
			double angle = atan2(dy, dx) * 180.0 / CV_PI;
			// Hand measurements shown that the left eye center should
			// ideally be roughly at (0.16, 0.14) of a scaled face image.
			const double DESIRED_LEFT_EYE_X = 0.16;
			const double DESIRED_RIGHT_EYE_X = (1.0f - 0.16);
			// Get the amount we need to scale the image to be the desired
			// fixed size we want.
			const int DESIRED_FACE_WIDTH = 70;
			const int DESIRED_FACE_HEIGHT = 70;
			double desiredLen = (DESIRED_RIGHT_EYE_X - 0.16);
			double scale = desiredLen * DESIRED_FACE_WIDTH / len;

			// Get the transformation matrix for the desired angle & size.
			cv::Mat rot_mat = getRotationMatrix2D(eyesCenter, angle, scale);
			// Shift the center of the eyes to be the desired center.
			const int DESIRED_LEFT_EYE_Y = 0.14;
			double ex = DESIRED_FACE_WIDTH * 0.5f - eyesCenter.x;
			double ey = DESIRED_FACE_HEIGHT * DESIRED_LEFT_EYE_Y - eyesCenter.y;
			rot_mat.at<double>(0, 2) += ex;
			rot_mat.at<double>(1, 2) += ey;
			// Transform the face image to the desired angle & size &
			// position! Also clear the transformed image background to a
			// default grey.
			cv::Mat warped = cv::Mat(DESIRED_FACE_HEIGHT, DESIRED_FACE_WIDTH, CV_8U, cv::Scalar(128));
			cv::warpAffine(sp_pic_in->data(), warped, rot_mat, warped.size());
			
			sp_pic_out = picture::create(warped);
			sp_ctx->set_bool_value(FACE_ALIGN_STATE, true);
			util_log::logd(FACE_ALIGN_PREPROCESSOR_TAG, "face_align success and set context [%ws] to true.", FACE_ALIGN_STATE);
			return result_success;
		}
	public:
		face_align_preprocessor(const std::wstring& str_flandmark_model_file)
			: m_str_flandmark_model_file(str_flandmark_model_file)
		{
			
		}
		~face_align_preprocessor()
		{}
	private:
		std::wstring m_str_flandmark_model_file;
	};
}