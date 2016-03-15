#include "scheme.h"
#include "gray_preprocessor.h"
#include "equalization_preprocessor.h"
#include "blur_preprocessor.h"
#include "threshold_preprocessor.h"
#include "edge_preprocessor.h"
#include "face_area_detect_preprocessor.h"
#include "face_contour_mask_preprocessor.h"
#include "face_align_preprocessor.h"
#include "size_align_preprocessor.h"
#include "session.h"
#include "train_task_creator.h"

namespace face_recognition
{
	result scheme::train(const std::wstring& str_train_pic_dir,
		const std::wstring& str_face_cascade_file, const std::wstring& str_eye_cascade_file,
		const std::wstring& str_eye_cascade_file2, const std::wstring& str_flandmark_model_file,
		unsigned int size_align_length,
		int low_threshold, int high_threshold)
	{
		boost::shared_ptr<preprocessor> sp_gray_preprocessor = preprocessor_factroy::create_preprocessor<gray_preprocessor>();
		boost::shared_ptr<preprocessor> sp_equalization_preprocessor = preprocessor_factroy::create_preprocessor<equalization_preprocessor>();
		boost::shared_ptr<preprocessor> sp_face_area_detect_preprocessor = preprocessor_factroy::create_preprocessor<face_area_detect_preprocessor>(str_face_cascade_file);
		boost::shared_ptr<preprocessor> sp_face_align_preprocessor = preprocessor_factroy::create_preprocessor<face_align_preprocessor>(str_eye_cascade_file, str_eye_cascade_file2, str_flandmark_model_file, size_align_length);
		boost::shared_ptr<preprocessor> sp_face_contour_mask_preprocessor = preprocessor_factroy::create_preprocessor<face_contour_mask_preprocessor>(low_threshold, high_threshold);

		boost::shared_ptr<preprocessor_manager> sp_mgr(preprocessor_manager::create());

		sp_mgr->add_preprocessor(sp_gray_preprocessor);
		sp_mgr->add_preprocessor(sp_face_area_detect_preprocessor);
		sp_mgr->add_preprocessor(sp_face_align_preprocessor);
		sp_mgr->add_preprocessor(sp_equalization_preprocessor);
		//sp_mgr->add_preprocessor(sp_face_contour_mask_preprocessor);

		boost::shared_ptr<session> sp_session;
		result res = session::create(sp_mgr, model_recognizer::type_lbph, sp_session);
		if (res != result_success)
		{
			util_log::log(SCHEME_TAG, "create session fail with result[%s]", result_string(res));
			return res;
		}
		boost::shared_ptr<train_task> sp_train_task;
		res = train_task_creator::create_task(str_train_pic_dir, sp_train_task);
		if (res != result_success)
		{
			util_log::log(SCHEME_TAG, "create train task fail with result[%s]", result_string(res));
			return res;
		}
		res = sp_session->train(sp_train_task);
		if (res != result_success)
		{
			util_log::log(SCHEME_TAG, "train fail with result[%s]", result_string(res));
			return res;
		}
		m_sp_session = sp_session;
		return result_success;
	}

	result scheme::predict(const std::wstring& str_pic, std::wstring& str_label, double& confidence)
	{
		boost::shared_ptr<picture> sp_pic;
		result res = picture::load(str_pic, sp_pic);
		if (res != result_success)
		{
			util_log::log(SCHEME_TAG, "in predict load picture[%ws] fail with result[%s]", str_pic.c_str(), result_string(res));
			return res;
		}
		res = m_sp_session->predict(sp_pic, str_label, confidence);
		if (res != result_success)
		{
			util_log::log(SCHEME_TAG, "predict fail with result[%s]", result_string(res));
			return res;
		}
		return result_success;
	}
}