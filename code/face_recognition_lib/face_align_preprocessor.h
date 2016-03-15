#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class face_align_preprocessor : public preprocessor
	{
	public:
		result get_eyes_postion(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<pic_rect> sp_rect, boost::shared_ptr<pic_point>& sp_left_eye, boost::shared_ptr<pic_point>& sp_right_eye)
		{
			boost::shared_ptr<face_feature_detector> sp_detector;
			result res = face_feature_detector::create(m_str_flandmark_model_file, sp_detector);
			if (res != result_success)
			{
				util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "face_feature_detector create fail with result[%s]", result_string(res));
				return res;
			}
			boost::shared_ptr<face_feature> sp_feature;
			res = sp_detector->detect_feature(sp_pic_in, *sp_rect.get(), sp_feature);
			if (res != result_success)
			{
				util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "detect_feature fail with result[%s]", result_string(res));
				return res;
			}
			cv::Rect face_rect;
			face_rect.x = sp_rect->_x;
			face_rect.y = sp_rect->_y;
			face_rect.width = sp_rect->_width;
			face_rect.height = sp_rect->_height;

			cv::Mat face = cv::Mat(sp_pic_in->data(), face_rect);
			const float EYE_SX = 0.16f;//x
			const float EYE_SY = 0.26f;//y
			const float EYE_SW = 0.30f;//width
			const float EYE_SH = 0.28f;//height

			int leftX = cvRound(face.cols * EYE_SX);
			int topY = cvRound(face.rows * EYE_SY);
			int widthX = cvRound(face.cols * EYE_SW);
			int heightY = cvRound(face.rows * EYE_SH);
			int rightX = cvRound(face.cols * (1.0 - EYE_SX - EYE_SW));  // 右眼的开始区域

			cv::Mat topLeftOfFace = face(cv::Rect(leftX, topY, widthX, heightY));
			cv::Mat topRightOfFace = face(cv::Rect(rightX, topY, widthX, heightY));
			cv::Rect leftEyeRect, rightEyeRect;

			boost::shared_ptr<picture> sp_face_left_eye = picture::create(topLeftOfFace);
			boost::shared_ptr<picture> sp_face_right_eye = picture::create(topRightOfFace);
			boost::shared_ptr<cascade_detector> sp_cascade_detector;
			res = cascade_detector::create(m_str_eyes_cascade_file, sp_cascade_detector);
			if (res != result_success)
			{
				return res;
			}
			std::vector<pic_rect> vec_left_eye_rect;
			res = sp_cascade_detector->detect(sp_face_left_eye, vec_left_eye_rect, 0, 0, 1000, 1000);
			if (res != result_success)
			{
				return res;
			}
			if (vec_left_eye_rect.empty())
			{
				return result_already_handled;
			}

			std::vector<pic_rect> vec_right_eye_rect;
			res = sp_cascade_detector->detect(sp_face_right_eye, vec_right_eye_rect, 0, 0, 1000, 1000);
			if (res != result_success)
			{
				return res;
			}
			if (vec_right_eye_rect.empty())
			{
				return result_already_handled;
			}

			leftEyeRect.x = leftX + vec_left_eye_rect[0]._x + sp_rect->_x;
			leftEyeRect.y = topY + vec_left_eye_rect[0]._y + sp_rect->_y;
			leftEyeRect.width = vec_left_eye_rect[0]._width;
			leftEyeRect.height = vec_left_eye_rect[0]._height;
			pic_point cascade_pic_left_eye(leftEyeRect.x + leftEyeRect.width / 2, leftEyeRect.y + leftEyeRect.height / 2);

			rightEyeRect.x = rightX + vec_right_eye_rect[0]._x + sp_rect->_x;
			rightEyeRect.y = topY + vec_right_eye_rect[0]._y + sp_rect->_y;
			rightEyeRect.width = vec_right_eye_rect[0]._width;
			rightEyeRect.height = vec_right_eye_rect[0]._height;
			pic_point cascade_pic_right_eye(rightEyeRect.x + rightEyeRect.width / 2, rightEyeRect.y + rightEyeRect.height / 2);

			cv::circle(sp_pic_in->data(), cv::Point(cascade_pic_left_eye._x, cascade_pic_left_eye._y), 3, CV_RGB(0, 255, 0), CV_FILLED);
			cv::circle(sp_pic_in->data(), cv::Point(cascade_pic_right_eye._x, cascade_pic_right_eye._y), 3, CV_RGB(0, 255, 0), CV_FILLED);
			cv::imshow("cascade", face);
			cv::waitKey();

			pic_point feature_pic_left_eye((sp_feature->_left_eye_left._x + sp_feature->_left_eye_right._x) / 2, (sp_feature->_left_eye_left._y + sp_feature->_left_eye_right._y) / 2);
			pic_point feature_pic_right_eye((sp_feature->_right_eye_left._x + sp_feature->_right_eye_right._x) / 2, (sp_feature->_right_eye_left._y + sp_feature->_right_eye_right._y) / 2);
			cv::circle(sp_pic_in->data(), cv::Point(feature_pic_left_eye._x, feature_pic_left_eye._y), 3, CV_RGB(255, 255, 0), CV_FILLED);
			cv::circle(sp_pic_in->data(), cv::Point(feature_pic_right_eye._x, feature_pic_right_eye._y), 3, CV_RGB(255, 255, 0), CV_FILLED);
			cv::imshow("flandmark", face);
			cv::waitKey();

			util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "flandmark eyes [%d,%d] [%d,%d] --- cascade eyes [%d,%d][%d,%d]", 
				feature_pic_left_eye._x, feature_pic_left_eye._y, feature_pic_right_eye._x, feature_pic_right_eye._y,
				cascade_pic_left_eye._x, cascade_pic_left_eye._y, cascade_pic_right_eye._x, cascade_pic_right_eye._y);
			return result_dir_not_exist;
		}
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
			boost::shared_ptr<pic_rect> sp_rect;
			res = sp_ctx->get_value(FACE_AREA_RECT, sp_rect);
			if (res != result_success)
			{
				util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "unable to detects face feature for geting [%ws] fail.", FACE_AREA_RECT);
				return res;
			}

			boost::shared_ptr<pic_point> sp_left_eye;
			boost::shared_ptr<pic_point> sp_right_eye;
			res = get_eyes_postion(sp_pic_in, sp_rect, sp_left_eye, sp_right_eye);
			if (res != result_success)
			{
				util_log::log(FACE_ALIGN_PREPROCESSOR_TAG, "get_eyes_postion fail with result[%s].", result_string(res));
				return res;
			}
			cv::Point2f leftEye(sp_left_eye->_x, sp_left_eye->_y);
			cv::Point2f rightEye(sp_right_eye->_x, sp_right_eye->_y);

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
			const int DESIRED_FACE_WIDTH = m_align_size;
			const int DESIRED_FACE_HEIGHT = m_align_size;
			double desiredLen = (DESIRED_RIGHT_EYE_X - 0.16);
			double scale = desiredLen * DESIRED_FACE_WIDTH / len;

			// Get the transformation matrix for the desired angle & size.
			cv::Mat rot_mat = getRotationMatrix2D(eyesCenter, angle, scale);
			// Shift the center of the eyes to be the desired center.
			const double DESIRED_LEFT_EYE_Y = 0.16;
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
		face_align_preprocessor(const std::wstring& str_eyes_cascade_file,
								const std::wstring& str_eyes_cascade_file2, 
								const std::wstring& str_flandmark_model_file, unsigned int align_size)
			: m_str_eyes_cascade_file(str_eyes_cascade_file)
			, m_str_eyes_cascade_file2(str_eyes_cascade_file2)
			, m_str_flandmark_model_file(str_flandmark_model_file)
			, m_align_size(align_size)
		{
			
		}
		~face_align_preprocessor()
		{}
	private:
		std::wstring m_str_eyes_cascade_file;
		std::wstring m_str_eyes_cascade_file2;
		std::wstring m_str_flandmark_model_file;
		unsigned int m_align_size;
	};
}