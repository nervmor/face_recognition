#include "picture.h"
#include <opencv2/imgcodecs.hpp>
#include "../frame/util_string.h"
#include "../frame/util_path.h"
#include "../frame/util_log.h"

namespace face_recognition
{
	face_recognition::result face_recognition::picture::load(const std::wstring str_file, boost::shared_ptr<picture>& sp_pic)
	{
		if (!util_path::file_exist(str_file))
		{
			util_log::log(PICTURE_TAG, "picture file[%ws] is not exist.", str_file.c_str());
			return result_file_not_exist;
		}
		cv::Mat pic = cv::imread(util_string::w2a(str_file).c_str());
		if (pic.data == NULL)
		{
			util_log::log(PICTURE_TAG, "imread fail. picture file[%ws] is invalid.", str_file.c_str());
			return result_file_invalid;
		}
		sp_pic = boost::make_shared<picture>();
		sp_pic->m_data = pic;
		util_log::logd(PICTURE_TAG, "load picture file[%ws] success.", str_file.c_str());
		return result_success;
	}

	boost::shared_ptr<picture> picture::create(cv::Mat& data)
	{
		boost::shared_ptr<picture> sp_pic = boost::make_shared<picture>();
		sp_pic->m_data = data;
		return sp_pic;
	}

	cv::Mat& picture::data()
	{
		return m_data;
	}

	IplImage picture::image()
	{
		return IplImage(m_data);
	}

	unsigned int picture::channels()
	{
		return m_data.channels();
	}

	unsigned int picture::width()
	{
		return m_data.cols;
	}

	unsigned int picture::height()
	{
		return m_data.rows;
	}

	unsigned int picture::depth()
	{
		return m_data.depth();
	}

	void picture::destroy()
	{
		if (!m_data.data)
		{
			
		}
	}

	picture::~picture()
	{
		destroy();
	}

	

}