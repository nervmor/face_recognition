#pragma once

#include "../frame/include.h"
#include "define.h"
#include <opencv2/imgproc.hpp>

namespace face_recognition
{
	class picture : public boost::noncopyable
	{
	public:
		static result load(const std::wstring str_file, boost::shared_ptr<picture>& sp_pic);
		static boost::shared_ptr<picture> create(cv::Mat& data);
		boost::shared_ptr<picture> clone();
		void destroy();
		unsigned int width();
		unsigned int height();
		unsigned int channels();
		unsigned int depth();
		cv::Mat& data();
		IplImage image();
		void show(int delay);
	private:
		cv::Mat m_data;
	public:
		~picture();
	};
}