#include "misc.h"
#include "../frame/util_string.h"
#include "../frame/util_path.h"
#include "../frame/util_log.h"

namespace face_recognition
{
	result contours_detector::detect_contours(boost::shared_ptr<picture> sp_pic_in, std::vector<pic_area>& vec_pic_area)
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


	result cascade_detector::create(const std::wstring& str_cascade_file, boost::shared_ptr<cascade_detector>& sp_detector)
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
	void cascade_detector::destroy()
	{
		if (!m_cascade.empty())
		{

		}
	}
	result cascade_detector::detect(boost::shared_ptr<picture> sp_pic_in, std::vector<pic_rect>& vec_rect)
	{
		if (sp_pic_in->depth() != CV_8U)
		{
			util_log::log(CASCADE_DETECTOR_TAG, "picture is not CV_8U to detect cascade rect.");
			return result_picture_invalid;
		}
		std::vector<cv::Rect> detected_rect;
		m_cascade.detectMultiScale(sp_pic_in->data(), detected_rect,
			1.1, 2, 0
			//|CASCADE_FIND_BIGGEST_OBJECT
			//|CASCADE_DO_ROUGH_SEARCH
			| cv::CASCADE_SCALE_IMAGE);
		if (detected_rect.empty())
		{
			util_log::logd(CASCADE_DETECTOR_TAG, "there is no cascade rect detected.");
		}
		else
		{
			std::string str_face_rect_info = "detected cascade rect -->";
			for (std::vector<cv::Rect>::iterator it = detected_rect.begin(); it != detected_rect.end(); it++)
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
	result cascade_detector::detect_largest(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<pic_rect>& sp_rect)
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




	result face_feature_detector::create(const std::wstring& str_flandmark_model_file, boost::shared_ptr<face_feature_detector>& sp_detector)
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
	void face_feature_detector::destroy()
	{
		if (m_p_model != NULL)
		{
			flandmark_free(m_p_model);
		}
	}

	result face_feature_detector::detect_feature(boost::shared_ptr<picture> sp_pic, const pic_rect& face_rect, boost::shared_ptr<face_feature>& sp_feature)
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
			return result_no_eyes_fetected;
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


	const char* model_recognizer::type_string(recognizer_type type)
	{
		static const char* s_type_string[] =
		{
			"type_eigen",
			"type_fisher",
			"type_lbph"
		};
		return s_type_string[(unsigned int)type];
	}

	result model_recognizer::create(recognizer_type type, boost::shared_ptr<model_recognizer>& sp_model)
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
	result model_recognizer::create(recognizer_type type, const std::wstring str_model_file, boost::shared_ptr<model_recognizer>& sp_model)
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
	void model_recognizer::train(boost::shared_ptr<train_task> sp_task)
	{
		std::vector<cv::Mat> images;
		std::vector<int> labels;
		std::vector<std::wstring> filenames;
		std::map<int, std::wstring> map_label_str;
		sp_task->get(images, labels, filenames, map_label_str);
		if (images.empty())
		{
			util_log::log(MODEL_RECOGNIZER_TAG, "get no valid image to train.");
			return;
		}
		if (images.size() != labels.size())
		{
			util_log::log(MODEL_RECOGNIZER_TAG, "images and labels count is not the same. can't train.");
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
	void model_recognizer::predict(boost::shared_ptr<picture> sp_pic, std::wstring& str_label, double& confidence)
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
	void model_recognizer::destroy()
	{

	}
	void model_recognizer::save(const std::wstring& str_model_file)
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

	result mutiple_cascade_detector::create(const std::vector<std::wstring>& vec_cascade_file, boost::shared_ptr<mutiple_cascade_detector>& sp_detector)
	{
		if (vec_cascade_file.empty())
		{
			return result_success;
		}
		result res = result_success;
		std::vector<boost::shared_ptr<cascade_detector> > vec_sp_detector;
		for (std::vector<std::wstring>::const_iterator it = vec_cascade_file.begin(); it != vec_cascade_file.end(); it++)
		{
			const std::wstring& str_cascade_file = *it;
			boost::shared_ptr<cascade_detector> sp_cascade_detector;
			res = cascade_detector::create(str_cascade_file, sp_cascade_detector);
			if (res != result_success)
			{
				util_log::log(MUTIPLE_CASCADE_DETECTOR_TAG, "create cascade detecotr by file[%ws] fail with result[%s]", str_cascade_file.c_str(), result_string(res));
				continue;
			}
			vec_sp_detector.emplace_back(sp_cascade_detector);
		}
		if (vec_sp_detector.empty())
		{
			util_log::log(MUTIPLE_CASCADE_DETECTOR_TAG, "threre is no cascade detector created.");
			return res;
		}
		sp_detector = boost::make_shared<mutiple_cascade_detector>();
		sp_detector->m_vec_sp_detector = vec_sp_detector;
		return result_success;
	}

	void mutiple_cascade_detector::destroy()
	{

	}

	void mutiple_cascade_detector::detect(boost::shared_ptr<picture> sp_pic_in, std::vector<std::pair<result, std::vector<pic_rect> > >& vec_res_vec_detected_rect)
	{
		for (std::vector<boost::shared_ptr<cascade_detector> >::iterator it = m_vec_sp_detector.begin(); it != m_vec_sp_detector.end(); it++)
		{
			boost::shared_ptr<cascade_detector> sp_detector(*it);

			std::vector<pic_rect> vec_detected_rect;
			result res = sp_detector->detect(sp_pic_in, vec_detected_rect);
			if (res != result_success)
			{
				util_log::log(MUTIPLE_CASCADE_DETECTOR_TAG, "cascade detector detect fail with result[%s]", result_string(res));
			}
			if (!vec_detected_rect.empty())
			{
				vec_res_vec_detected_rect.push_back(std::make_pair(res, vec_detected_rect));
			}
		}
	}

	void mutiple_cascade_detector::detect_evaluat(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<pic_rect>& sp_rect)
	{
		std::vector<std::pair<result, std::vector<pic_rect> > > vec_res_vec_detected_rect;
		detect(sp_pic_in, vec_res_vec_detected_rect);
		
		std::vector<pic_rect> vec_all_rect;
		int64 total_acreage = 0;
		for (std::vector<std::pair<result, std::vector<pic_rect> > >::iterator it = vec_res_vec_detected_rect.begin(); it != vec_res_vec_detected_rect.end(); it++)
		{
			std::pair<result, std::vector<pic_rect> >& pari_res_vec_rect = *it;
			result res = pari_res_vec_rect.first;
			std::vector<pic_rect>& vec_rect = pari_res_vec_rect.second;
			if (res == result_success)
			{
				for (std::vector<pic_rect>::iterator itx = vec_rect.begin(); itx != vec_rect.end(); itx++)
				{
					pic_rect& detected_rect = *itx;
					total_acreage += detected_rect.cacl_acreage();
				}
				vec_all_rect.insert(vec_all_rect.end(), vec_rect.begin(), vec_rect.end());
			}
		}
		if (vec_all_rect.empty())
		{
			return;
		}

		boost::shared_ptr<pic_rect> sp_max_common_rect;
		float max_common_acreage_percent = 0;
		std::map<unsigned int, std::map<unsigned int, float> > map_statistics_common_acreage_percent;
		for (unsigned int i = 0; i != vec_all_rect.size(); i++)
		{
			pic_rect& rect = vec_all_rect[i];
			std::map<unsigned int, float> map_common_acreage_percent;
			for (unsigned int k = 0; k != vec_all_rect.size(); k++)
			{
				if (k == i)
				{
					continue;
				}
				pic_rect& rect_x = vec_all_rect[k];
				boost::shared_ptr<pic_rect> sp_common_rect;
				if (!get_common_rect(rect, rect_x, sp_common_rect))
				{
					continue;
				}
				int64 common_acreage = sp_common_rect->cacl_acreage();
				if (common_acreage == 0)
				{
					continue;
				}
				float percent1 = common_acreage;
				percent1 = percent1 / rect.cacl_acreage();
				float percent2 = common_acreage;
				percent2 = percent2 / rect_x.cacl_acreage();
				float average_percent = (percent1 + percent2) / 2;
				if (average_percent > max_common_acreage_percent)
				{
					sp_max_common_rect = sp_common_rect;
					max_common_acreage_percent = average_percent;
				}
				map_common_acreage_percent.insert(std::make_pair(k, average_percent));
			}
			if (!map_common_acreage_percent.empty())
			{
				map_statistics_common_acreage_percent.insert(std::make_pair(i, map_common_acreage_percent));
			}
		}
		sp_rect = sp_max_common_rect;
	}
}