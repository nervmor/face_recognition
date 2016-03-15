#pragma once


#include "picture.h"
#include "train_task.h"
#include <opencv2/objdetect.hpp>
#include <opencv2/face.hpp>
#include <opencv2/highgui.hpp>
#include "flandmark_detector.h"
#include "../frame/util_string.h"
#include "../frame/util_path.h"
#include "../frame/util_log.h"

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
		static result gray(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out)
		{
			Assert(sp_pic_in);
			cv::Mat data_gray;
			cv::cvtColor(sp_pic_in->data(), data_gray, CV_BGR2GRAY);
			sp_pic_out = picture::create(data_gray);
			return result_success;
		}
		static result equalization(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out)
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
		static result blur(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out)
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
		static result resize(boost::shared_ptr<picture> sp_pic_in, unsigned int width, unsigned int height, boost::shared_ptr<picture>& sp_pic_out)
		{
			Assert(sp_pic_in);
			cv::Size rsize = cv::Size(width, height);
			cv::Mat rdata;
			cv::resize(sp_pic_in->data(), rdata, rsize);
			sp_pic_out = picture::create(rdata);
			return result_success;
		}
		static result cut(boost::shared_ptr<picture> sp_pic_in, unsigned int x, unsigned int y, unsigned int width, unsigned int height, boost::shared_ptr<picture>& sp_pic_out)
		{
			Assert(sp_pic_in);
			cv::Rect rect(x, y, width, height);
			cv::Mat cut_data;
			sp_pic_in->data()(rect).copyTo(cut_data);
			sp_pic_out = picture::create(cut_data);
			return result_success;
		}
		static result edge(boost::shared_ptr<picture> sp_pic_in, int low_threshold, int high_threshold, boost::shared_ptr<picture>& sp_pic_out)
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
		static result threshold(boost::shared_ptr<picture> sp_pic_in, int low_threshold, int high_threshold, boost::shared_ptr<picture>& sp_pic_out)
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
		static result mask(boost::shared_ptr<picture> sp_pic_in, const pic_area& area, boost::shared_ptr<picture>& sp_pic_out)
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
	};

	class contours_detector
	{
	public:
		static result detect_contours(boost::shared_ptr<picture> sp_pic_in, std::vector<pic_area>& vec_pic_area)
		{
			if (sp_pic_in->channels() != 1)
			{
				util_log::log(CONTOURS_DETECTOR, "picture is not single to detect contours.");
				return result_picture_invalid;
			}
			std::vector<std::vector<cv::Point> > contours;
			std::vector<cv::Vec4i> hierarchy;
			cv::findContours(sp_pic_in->data(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

			for (std::vector<std::vector<cv::Point> >::iterator it = contours.begin(); it != contours.end(); it++)
			{
				std::vector<cv::Point>& vec_pt = *it;

				pic_area area;
				for (std::vector<cv::Point>::iterator itx = vec_pt.begin(); itx != vec_pt.end(); itx++)
				{
					cv::Point& pt = *itx;
					pic_point pic_pt(pt.x, pt.y);
					area.add_pt(pic_pt);
				}
				vec_pic_area.push_back(area);
			}
			return result_success;
		}
	};


	class cascade_detector
	{
	public:
		static result create(const std::wstring& str_cascade_file, boost::shared_ptr<cascade_detector>& sp_detector)
		{
			if (!util_path::file_exist(str_cascade_file))
			{
				util_log::log(CASCADE_DETECTOR_TAG, "cascade file[%ws] is not exist.", str_cascade_file.c_str());
				return result_file_not_exist;
			}

			boost::shared_ptr<cascade_detector> sp_detector_x(boost::make_shared<cascade_detector>());
			if (!sp_detector_x->m_cascade.load(util_string::w2a(str_cascade_file)))
			{
				util_log::log(CASCADE_DETECTOR_TAG, "cascade file[%ws] is not invalid.", str_cascade_file.c_str());
				return result_file_invalid;
			}
			sp_detector = sp_detector_x;
			util_log::logd(CASCADE_DETECTOR_TAG, "cascade detector create success by file[%ws]", str_cascade_file.c_str());
			return result_success;
		}
		void destroy()
		{
			if (!m_cascade.empty())
			{
				
			}
		}
		result detect(boost::shared_ptr<picture> sp_pic_in, std::vector<pic_rect>& vec_rect)
		{
			if (sp_pic_in->depth() != CV_8U)
			{
				util_log::log(CASCADE_DETECTOR_TAG, "picture is not CV_8U to detect cascade rect.");
				return result_picture_invalid;
			}
			std::vector<cv::Rect> faces;
			m_cascade.detectMultiScale(sp_pic_in->data(), faces,
				1.1, 2, 0
				//|CASCADE_FIND_BIGGEST_OBJECT
				//|CASCADE_DO_ROUGH_SEARCH
				| cv::CASCADE_SCALE_IMAGE);
			if (faces.empty())
			{
				util_log::logd(CASCADE_DETECTOR_TAG, "there is no cascade rect detected.");
			}
			else
			{
				std::string str_face_rect_info = "detected cascade rect -->";
				for (std::vector<cv::Rect>::iterator it = faces.begin(); it != faces.end(); it++)
				{
					cv::Rect& rect = *it;
					vec_rect.push_back(pic_rect(rect.x, rect.y, rect.width, rect.height));
					char buf[4096];
					sprintf(buf, "[%d, %d, %d, %d]", rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
					str_face_rect_info += buf;
				}
				util_log::logd(CASCADE_DETECTOR_TAG, str_face_rect_info.c_str());
			}
			return result_success;
		}
		result detect_largest(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<pic_rect>& sp_rect)
		{
			std::vector<pic_rect> vec_rect;
			result res = detect(sp_pic_in, vec_rect);
			if (res != result_success)
			{
				return res;
			}
			if (vec_rect.empty())
			{
				return result_success;
			}
			pic_rect& max_acreage_rect = *vec_rect.begin();
			int_64 max_acreage = max_acreage_rect.cacl_acreage();
			for (std::vector<pic_rect>::iterator it = vec_rect.begin(); it != vec_rect.end(); it++)
			{
				pic_rect& rect = *it;
				int_64 acreage = rect.cacl_acreage();
				if (acreage > max_acreage)
				{
					max_acreage = acreage;
					max_acreage_rect = *it;
				}
			}
			sp_rect = boost::make_shared<pic_rect>(max_acreage_rect);
			return result_success;
		}
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
		static result create(const std::wstring& str_flandmark_model_file, boost::shared_ptr<face_feature_detector>& sp_detector)
		{
			boost::shared_ptr<face_feature_detector>& sp_detector_x = boost::make_shared<face_feature_detector>();

			sp_detector_x->m_p_model = flandmark_init(util_string::w2a(str_flandmark_model_file).c_str());
			if (sp_detector_x->m_p_model == NULL)
			{
				util_log::log(FACE_FEATURE_DETECTOR_TAG, "flandmark_init fail.");
				return result_lib_fail;
			}
			sp_detector = sp_detector_x;
			return result_success;
		}
		void destroy()
		{
			if (m_p_model != NULL)
			{
				flandmark_free(m_p_model);
			}
		}
	
		result detect_feature(boost::shared_ptr<picture> sp_pic, const pic_rect& face_rect, boost::shared_ptr<face_feature>& sp_feature)
		{
			int *bbox = new int[4];
			bbox[0] = face_rect._x;
			bbox[1] = face_rect._y;
			bbox[2] = face_rect._x + face_rect._width;
			bbox[3] = face_rect._y + face_rect._height;
			double *landmarks = new double[2 * m_p_model->data.options.M];
			boost::shared_array<int> sp_bbox(bbox);
			boost::shared_array<double> sp_landmarks(landmarks);
			IplImage img = sp_pic->image();
			
			if (0 != flandmark_detect(&img, bbox, m_p_model, landmarks))
			{
				util_log::log(FACE_FEATURE_DETECTOR_TAG, "flandmark_detect get no face feature.");
				return result_no_face_feature_fetected;
			}
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
			sp_feature = boost::make_shared<face_feature>(landmarks);
			return result_success;
		}
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
		static const char* type_string(recognizer_type type)
		{
			static const char* s_type_string[] =
			{
				"type_eigen",
				"type_fisher",
				"type_lbph"
			};
			return s_type_string[(unsigned int)type];
		}
	public:
		static result create(recognizer_type type, boost::shared_ptr<model_recognizer>& sp_model)
		{
			cv::Ptr<cv::face::BasicFaceRecognizer> model;
			cv::Ptr<cv::face::LBPHFaceRecognizer> model_lbph;
			if (type == type_eigen)
			{
				model = cv::face::createEigenFaceRecognizer();
			}
			else if (type == type_fisher)
			{
				model = cv::face::createFisherFaceRecognizer();
			}
			else if (type == type_lbph)
			{
				model_lbph = cv::face::createLBPHFaceRecognizer();
			}
			else
			{
				Assert(false);
			}
			sp_model = boost::make_shared<model_recognizer>(type);
			if (type == type_lbph)
			{
				sp_model->m_model_lbph = model_lbph;
			}
			else
			{
				sp_model->m_model = model;
			}
			util_log::logd(MODEL_RECOGNIZER_TAG, "type[%s] create success.", type_string(type));
			return result_success;
		}
		static result create(recognizer_type type, const std::wstring str_model_file, boost::shared_ptr<model_recognizer>& sp_model)
		{
			if (!util_path::file_exist(str_model_file))
			{
				util_log::log(MODEL_RECOGNIZER_TAG, "model file[%ws] is not exist.", str_model_file.c_str());
				return result_file_not_exist;
			}
			cv::Ptr<cv::face::BasicFaceRecognizer> model;
			cv::Ptr<cv::face::LBPHFaceRecognizer> model_lbph;
			if (type == type_eigen)
			{
				model = cv::face::createEigenFaceRecognizer();
			}
			else if (type == type_fisher)
			{
				model = cv::face::createFisherFaceRecognizer();
			}
			else if (type == type_lbph)
			{
				model_lbph = cv::face::createLBPHFaceRecognizer();
			}
			else
			{
				Assert(false);
			}
			boost::shared_ptr<model_recognizer> sp_model_x = boost::make_shared<model_recognizer>(type);
			if (type == type_lbph)
			{
				model_lbph->load(util_string::w2a(str_model_file));
				sp_model_x->m_model_lbph = model_lbph;
			}
			else
			{
				model->load(util_string::w2a(str_model_file));
				sp_model_x->m_model = model;
			}
			
			util_log::logd(MODEL_RECOGNIZER_TAG, "type[%s] model_file[%ws] load success.", type_string(type), str_model_file.c_str());
			return result_success;
		}
		void train(boost::shared_ptr<train_task> sp_task)
		{
			std::vector<cv::Mat> images;
			std::vector<int> labels;
			std::map<int, std::wstring> map_label_str;
			sp_task->get(images, labels, map_label_str);
			if (images.empty())
			{
				util_log::log(MODEL_RECOGNIZER_TAG, "get no valid image to train.");
				return;
			}
			if (images.size() != labels.size())
			{
				util_log::log(MODEL_RECOGNIZER_TAG, "images and labels count is not the same. no pictures are train fail.");
				return;
			}
			if (m_type == type_lbph)
			{
				m_model_lbph->train(images, labels);
			}
			else
			{
				m_model->train(images, labels);
			}
			
			std::set<unsigned int> set_label;
			for (std::map<int, std::wstring>::iterator it = map_label_str.begin(); it != map_label_str.end(); it++)
			{
				unsigned int index = it->first;
				const std::wstring& str_label = it->second;
				if (m_type == type_lbph)
				{
					m_model_lbph->setLabelInfo(index, util_string::w2a(str_label));
				}
				else
				{
					m_model->setLabelInfo(index, util_string::w2a(str_label));
				}
				set_label.insert(index);
			}
			util_log::logd(MODEL_RECOGNIZER_TAG, "train picture count[%d], label count[%d].", images.size(), set_label.size());
		}
		void predict(boost::shared_ptr<picture> sp_pic, std::wstring& str_label, double& confidence)
		{
			int label = -1;
			double confidence_x = 1.0;
			if (m_type == type_lbph)
			{
				m_model_lbph->predict(sp_pic->data(), label, confidence_x);
			}
			else
			{
				m_model->predict(sp_pic->data(), label, confidence_x);
			}
			
			if (label == -1)
			{
				str_label.clear();
				confidence = 0.0;
			}
			else
			{
				cv::String str_label_info;
				if (m_type == type_lbph)
				{
					str_label_info = m_model_lbph->getLabelInfo(label);
				}
				else
				{
					str_label_info = m_model->getLabelInfo(label);
				}
				
				str_label = util_string::a2w(str_label_info.c_str());
				confidence = confidence_x;
			}
			util_log::logd(MODEL_RECOGNIZER_TAG, "predict picture label[%ws], confidence[%f].", str_label.c_str(), confidence);
		}
		void destroy()
		{

		}
		void save(const std::wstring& str_model_file)
		{
			if (m_type == type_lbph)
			{
				m_model_lbph->save(util_string::w2a(str_model_file));
			}
			else
			{
				m_model->save(util_string::w2a(str_model_file));
			}
			util_log::logd(MODEL_RECOGNIZER_TAG, "save model to file[%ws] success.", str_model_file.c_str());
		}
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