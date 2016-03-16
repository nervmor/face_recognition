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
		virtual result init()
		{
			return result_success;
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

			//直方图均衡化,左均衡右均衡,全均衡
			cv::Mat faceImg(sp_pic_in->data());
			
			int w = faceImg.cols;
			int h = faceImg.rows;
			cv::Mat wholeFace;
			cv::equalizeHist(faceImg, wholeFace);
			int midX = w / 2;
			cv::Mat leftSide = faceImg(cv::Rect(0, 0, midX, h));
			cv::Mat rightSide = faceImg(cv::Rect(midX, 0, w - midX, h));
			cv::equalizeHist(leftSide, leftSide);
			cv::equalizeHist(rightSide, rightSide);


			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					int v;
					if (x < w / 4)
					{
						v = leftSide.at<uchar>(y, x);
					}
					else if (x < w * 2 / 4)
					{
						int lv = leftSide.at<uchar>(y, x);
						int wv = wholeFace.at<uchar>(y, x);
						float f = (x - w * 1 / 4) / (float)(w / 4);
						v = cvRound((1.0f - f)*lv + (f)*wv);
					}
					else if (x < w * 3 / 4)
					{
						int rv = rightSide.at<uchar>(y, x - midX);
						int wv = wholeFace.at<uchar>(y, x);
						float f = (x - w * 2 / 4) / (float)(w / 4);
						v = cvRound((1.0f - f)*wv + (f)*rv);

					}
					else
					{
						v = rightSide.at<uchar>(y, x - midX);
					}
					faceImg.at<uchar>(y, x) = v;
				}
			}
			cv::Mat filtered = cv::Mat(faceImg.size(), CV_8U);
			cv::bilateralFilter(faceImg, filtered, 0, 20.0, 2.0);

			//椭圆形掩码
			cv::Mat mask = cv::Mat(faceImg.size(), CV_8UC1, cv::Scalar(255));


			cv::Point faceCenter = cv::Point(cvRound(faceImg.size().width*0.5), cvRound(faceImg.size().height*0.4));
			cv::Size size = cv::Size(cvRound(faceImg.size().width*0.5), cvRound(faceImg.size().height*0.8));
			ellipse(mask, faceCenter, size, 0, 0, 360, cv::Scalar(0), CV_FILLED);
			filtered.setTo(cv::Scalar(128), mask);

			sp_pic_out = picture::create(filtered);

			sp_ctx->set_bool_value(EUQALIZATION_HANDLE_STATE, true);
			util_log::logd(EQUALIZATION_PREPROCESSOR_TAG, "equalization success and set context [%ws] to true.", EUQALIZATION_HANDLE_STATE);
			return result_success;
		}
	};
}