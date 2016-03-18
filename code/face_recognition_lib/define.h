#pragma once

#define TRAIN_TASK_TAG						"train_task"
#define PICTURE_TAG							"picture"
#define PICTURE_HANDLER_TAG					"picture_handler"
#define CONTOURS_DETECTOR					"contours_detector"
#define CONTEXT_TAG							"context"
#define PREPROCESSOR_MANAGER_TAG			"preprocessor_manager"
#define CASCADE_DETECTOR_TAG				"cascade_detector"
#define MUTIPLE_CASCADE_DETECTOR_TAG		"mutiple_cascade_detector"
#define FACE_FEATURE_DETECTOR_TAG			"face_feature_detector"
#define MODEL_RECOGNIZER_TAG				"model_recognizer"
#define SESSION_TAG							"session"
#define TRAIN_TASK_CREATOR_TAG				"train_task_creator"
#define SCHEME_TAG							"scheme"


#define PICTURE_FILENAME_CONTEXT_KEY		L"filename"
namespace face_recognition{

	enum result
	{
		result_success,
		result_file_not_exist,
		result_dir_not_exist,
		result_file_invalid,
		result_context_key_not_found,
		result_picture_invalid,
		result_train_repeated,
		result_already_handled,
		result_no_face_detected,
		result_lib_fail,
		result_no_eyes_fetected,
	};

	const char* result_string(result res);
}