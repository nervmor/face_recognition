#include "misc.h"
#include "../frame/util_log.h"

namespace face_recognition
{
	result picture_handler::gray(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out)
	{
		Assert(sp_pic_in);
		cv::Mat data_gray;
		cv::cvtColor(sp_pic_in->data(), data_gray, CV_BGR2GRAY);
		sp_pic_out = picture::create(data_gray);
		return result_success;
	}
	result picture_handler::equalization(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out)
	{
		Assert(sp_pic_in);
		if (!(sp_pic_in->channels() == 1 && (sp_pic_in->depth() == CV_8U || sp_pic_in->depth() == CV_8S)))
		{
			util_log::log(PICTURE_HANDLER_TAG, "picture is not single channel 8-bit to equalizeHist.");
			return result_picture_invalid;
		}
		cv::Mat data_equalize;
		cv::equalizeHist(sp_pic_in->data(), data_equalize);
		sp_pic_out = picture::create(data_equalize);
		return result_success;
	}
	result picture_handler::blur(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out)
	{
		Assert(sp_pic_in);
		if (sp_pic_in->depth() != CV_8U &&
			sp_pic_in->depth() != CV_16U &&
			sp_pic_in->depth() != CV_16S &&
			sp_pic_in->depth() != CV_32F &&
			sp_pic_in->depth() != CV_64F)
		{
			util_log::log(PICTURE_HANDLER_TAG, "picture is not invalid to blur.");
			return result_picture_invalid;
		}
		cv::Mat data_blur;
		cv::blur(sp_pic_in->data(), data_blur, cv::Size(3, 3));
		sp_pic_out = picture::create(data_blur);
		return result_success;
	}
	result picture_handler::resize(boost::shared_ptr<picture> sp_pic_in, unsigned int width, unsigned int height, boost::shared_ptr<picture>& sp_pic_out)
	{
		Assert(sp_pic_in);
		cv::Size rsize = cv::Size(width, height);
		cv::Mat rdata;
		cv::resize(sp_pic_in->data(), rdata, rsize);
		sp_pic_out = picture::create(rdata);
		return result_success;
	}
	result picture_handler::cut(boost::shared_ptr<picture> sp_pic_in, unsigned int x, unsigned int y, unsigned int width, unsigned int height, boost::shared_ptr<picture>& sp_pic_out)
	{
		Assert(sp_pic_in);
		cv::Rect rect(x, y, width, height);
		cv::Mat cut_data;
		sp_pic_in->data()(rect).copyTo(cut_data);
		sp_pic_out = picture::create(cut_data);
		return result_success;
	}
	result picture_handler::edge(boost::shared_ptr<picture> sp_pic_in, int low_threshold, int high_threshold, boost::shared_ptr<picture>& sp_pic_out)
	{
		Assert(sp_pic_in);
		if (!(sp_pic_in->depth() == CV_8U || sp_pic_in->depth() == CV_8S))
		{
			util_log::log(PICTURE_HANDLER_TAG, "picture is not 8-bit to edge Canny.");
			return result_picture_invalid;
		}
		cv::Mat data_edge;
		cv::Canny(sp_pic_in->data(), data_edge, low_threshold, high_threshold, 3);
		cv::Mat data_out;
		data_out = cv::Scalar::all(0);
		sp_pic_in->data().copyTo(data_out, data_edge);
		sp_pic_out = picture::create(data_out);
		return result_success;
	}
	result picture_handler::threshold(boost::shared_ptr<picture> sp_pic_in, int low_threshold, int high_threshold, boost::shared_ptr<picture>& sp_pic_out)
	{
		Assert(sp_pic_in);
		if (sp_pic_in->channels() != 1 || !(sp_pic_in->depth() == CV_8U || sp_pic_in->depth() == CV_8S || sp_pic_in->depth() == CV_32F || sp_pic_in->depth() == CV_32S))
		{
			util_log::log(PICTURE_HANDLER_TAG, "picture is not single channel 8-bit or 32-bit to threshold.");
			return result_picture_invalid;
		}
		cv::Mat data_threshold;
		data_threshold.create(sp_pic_in->data().size(), sp_pic_in->data().type());
		cv::threshold(sp_pic_in->data(), data_threshold, low_threshold, high_threshold, cv::THRESH_BINARY);
		sp_pic_out = picture::create(data_threshold);
		return result_success;
	}
	result picture_handler::mask(boost::shared_ptr<picture> sp_pic_in, const pic_area& area, boost::shared_ptr<picture>& sp_pic_out)
	{
		Assert(sp_pic_in);
		cv::Mat data_mask;
		data_mask.create(sp_pic_in->data().size(), CV_8UC1);
		data_mask.setTo(cv::Scalar(255));
		for (unsigned int index = 0; index != area._vec_pt.size() - 1; index++)
		{
			cv::line(data_mask, cv::Point(area._vec_pt[index]._x, area._vec_pt[index]._y), cv::Point(area._vec_pt[index + 1]._x, area._vec_pt[index + 1]._y), cv::Scalar::all(0), 2, 8, 0);
		}
		cv::Mat dst_data;
		floodFill(data_mask, cv::Point(area._vec_pt[area._vec_pt.size() - 1]._x, area._vec_pt[area._vec_pt.size() - 1]._y), cv::Scalar(0));
		sp_pic_in->data().copyTo(dst_data, data_mask);
		sp_pic_out = picture::create(dst_data);
		return result_success;
	}
}
