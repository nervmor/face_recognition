#include "define.h"

namespace face_recognition
{
	const char* result_string(result res)
	{
		static const char* s_result_string[] = 
		{
			"result_success",
			"result_file_not_exist",
			"result_dir_not_exist",
			"result_file_invalid",
			"result_context_key_not_found",
			"result_picture_invalid",
			"result_train_repeated",
			"result_already_handled",
			"result_no_face_detected"
		};
		return s_result_string[(unsigned int)res];
	}
}
