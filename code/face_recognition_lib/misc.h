#pragma once


#include "picture.h"
#include "train_task.h"
#include <opencv2/objdetect.hpp>
#include <opencv2/face.hpp>
#include <opencv2/highgui.hpp>
#include "flandmark_detector.h"


namespace face_recognition
{
#if defined _WIN32
	typedef __int64 int_64;
	typedef unsigned __int64 uint_64;
#else
	typedef int64_t int_64;
	typedef uint64_t uint_64;
#endif // _WIN32

	struct pic_point
	{
		int _x;
		int _y;

		pic_point(int x, int y)
			: _x(x)
			, _y(y)
		{}
		pic_point(const pic_point& pt)
			: _x(pt._x)
			, _y(pt._y)
		{}
		pic_point operator =(const pic_point& pt)
		{
			_x = pt._x;
			_y = pt._y;
			return *this;
		}
	};
	struct pic_rect
	{
		unsigned int _x;
		unsigned int _y;
		unsigned int _width;
		unsigned int _height;

		pic_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
			: _x(x)
			, _y(y)
			, _width(width)
			, _height(height)
		{}
		int_64 cacl_acreage()
		{
			int_64 width = _width;
			int_64 height = _height;
			return width * height;
		}
	};
	struct pic_area
	{
		std::vector<pic_point> _vec_pt;

		void add_pt(const pic_point& pt)
		{
			_vec_pt.push_back(pt);
		}
		double area_size()
		{
			std::vector<cv::Point> contour;
			for (std::vector<pic_point>::iterator it = _vec_pt.begin(); it != _vec_pt.end(); it++)
			{
				pic_point& pic_pt = *it;
				contour.push_back(cv::Point(pic_pt._x, pic_pt._y));
			}
			return cv::contourArea(contour);
		}
	};
	struct face_feature
	{
		pic_point _left_eye_left;
		pic_point _left_eye_right;
		pic_point _right_eye_left;
		pic_point _right_eye_right;
		pic_point _left_nose;
		pic_point _right_nose;
		pic_point _left_mouth;
		pic_point _right_mouth;


		face_feature(double landmarks[])
			: _left_eye_left(static_cast<int>(landmarks[5 * 2]), static_cast<int>(landmarks[5 * 2 + 1])),
			_left_eye_right(static_cast<int>(landmarks[1 * 2]), static_cast<int>(landmarks[1 * 2 + 1])),
			_right_eye_left(static_cast<int>(landmarks[2 * 2]), static_cast<int>(landmarks[2 * 2 + 1])),
			_right_eye_right(static_cast<int>(landmarks[6 * 2]), static_cast<int>(landmarks[6 * 2 + 1])),
			_left_nose(static_cast<int>(landmarks[0 * 2]), static_cast<int>(landmarks[0 * 2 + 1])),
			_right_nose(static_cast<int>(landmarks[7 * 2]), static_cast<int>(landmarks[7 * 2 + 1])),
			_left_mouth(static_cast<int>(landmarks[3 * 2]), static_cast<int>(landmarks[3 * 2 + 1])),
			_right_mouth(static_cast<int>(landmarks[4 * 2]), static_cast<int>(landmarks[4 * 2 + 1]))
		{
			/*
			*    5   1    2   6
			*
			*
			*          0/7
			*
			*
			*       3       4
			*
			*/
		}
	};
	class picture_handler
	{
	public:
		static result gray(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out);
		static result equalization(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out);
		static result blur(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out);
		static result resize(boost::shared_ptr<picture> sp_pic_in, unsigned int width, unsigned int height, boost::shared_ptr<picture>& sp_pic_out);
		static result cut(boost::shared_ptr<picture> sp_pic_in, unsigned int x, unsigned int y, unsigned int width, unsigned int height, boost::shared_ptr<picture>& sp_pic_out);
		static result edge(boost::shared_ptr<picture> sp_pic_in, int low_threshold, int high_threshold, boost::shared_ptr<picture>& sp_pic_out);
		static result threshold(boost::shared_ptr<picture> sp_pic_in, int low_threshold, int high_threshold, boost::shared_ptr<picture>& sp_pic_out);
		static result mask(boost::shared_ptr<picture> sp_pic_in, const pic_area& area, boost::shared_ptr<picture>& sp_pic_out);
	};

	class contours_detector
	{
	public:
		static result detect_contours(boost::shared_ptr<picture> sp_pic_in, std::vector<pic_area>& vec_pic_area);
	};


	class cascade_detector
	{
	public:
		static result create(const std::wstring& str_cascade_file, boost::shared_ptr<cascade_detector>& sp_detector);
		void destroy();
		result detect(boost::shared_ptr<picture> sp_pic_in, std::vector<pic_rect>& vec_rect);
		result detect_largest(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<pic_rect>& sp_rect);
	private:
		cv::CascadeClassifier m_cascade;
	public:
		~cascade_detector()
		{
			destroy();
		}
	};

	class face_feature_detector
	{
	public:
		static result create(const std::wstring& str_flandmark_model_file, boost::shared_ptr<face_feature_detector>& sp_detector);
		void destroy();
		result detect_feature(boost::shared_ptr<picture> sp_pic, const pic_rect& face_rect, boost::shared_ptr<face_feature>& sp_feature);
	public:
		face_feature_detector()
			: m_p_model(NULL)
		{}
		~face_feature_detector()
		{
			destroy();
		}
	private:
		FLANDMARK_Model* m_p_model;
	};

	class model_recognizer
	{
	public:
		enum recognizer_type
		{
			type_eigen,
			type_fisher,
			type_lbph
		};
		static const char* type_string(recognizer_type type);
	public:
		static result create(recognizer_type type, boost::shared_ptr<model_recognizer>& sp_model);
		static result create(recognizer_type type, const std::wstring str_model_file, boost::shared_ptr<model_recognizer>& sp_model);
		void train(boost::shared_ptr<train_task> sp_task);
		void predict(boost::shared_ptr<picture> sp_pic, std::wstring& str_label, double& confidence);
		void destroy();
		void save(const std::wstring& str_model_file);
	private:
		recognizer_type m_type;
		cv::Ptr<cv::face::BasicFaceRecognizer> m_model;
		cv::Ptr<cv::face::LBPHFaceRecognizer> m_model_lbph;
	public:
		model_recognizer(recognizer_type type)
			: m_type(type)
		{}
		~model_recognizer()
		{
			destroy();
		}
	};

}