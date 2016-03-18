#include "../face_recognition_lib/scheme.h"
#include "../frame/util_path.h"

void main()
{
	std::wstring str_image_dir = util_path::get_image_dir();
	std::wstring str_train_picture_dir = str_image_dir + L"..\\..\\..\\data\\bug_picture\\";
	//std::wstring str_train_picture_dir = str_image_dir + L"..\\..\\..\\data\\train_picture\\";
	std::vector<std::wstring> vec_face_haar_file;
	vec_face_haar_file.push_back(str_image_dir + L"..\\..\\..\\data\\haarcascades\\haarcascade_frontalface_alt.xml");
	vec_face_haar_file.push_back(str_image_dir + L"..\\..\\..\\data\\haarcascades\\haarcascade_frontalface_alt2.xml");
	vec_face_haar_file.push_back(str_image_dir + L"..\\..\\..\\data\\haarcascades\\haarcascade_frontalface_default.xml");
	vec_face_haar_file.push_back(str_image_dir + L"..\\..\\..\\data\\haarcascades\\haarcascade_frontalface_alt_tree.xml");
	
	std::vector<std::wstring> vec_left_eye_haar_file;
	vec_left_eye_haar_file.push_back(str_image_dir + L"..\\..\\..\\data\\haarcascades\\haarcascade_eye.xml");
	vec_left_eye_haar_file.push_back(str_image_dir + L"..\\..\\..\\data\\haarcascades\\haarcascade_lefteye_2splits.xml");

	std::vector<std::wstring> vec_right_eye_haar_file;
	vec_right_eye_haar_file.push_back(str_image_dir + L"..\\..\\..\\data\\haarcascades\\haarcascade_eye.xml");
	vec_right_eye_haar_file.push_back(str_image_dir + L"..\\..\\..\\data\\haarcascades\\haarcascade_righteye_2splits.xml");

	std::wstring str_flandmark_model_file = str_image_dir + L"..\\..\\..\\data\\flandmark_data\\flandmark_model.dat";

	face_recognition::scheme sc;
	face_recognition::result res = sc.train(str_train_picture_dir, vec_face_haar_file, vec_left_eye_haar_file, vec_right_eye_haar_file, str_flandmark_model_file, 100);

	if (res != face_recognition::result_success)
	{
		printf("train fail.");
		getchar();
		return;
	}

	std::vector<std::wstring> vec_predict_file;
	std::wstring str_predict_picture_dir = str_image_dir + L"..\\..\\..\\data\\predict_picture\\";
	util_path::list_dir_files(str_predict_picture_dir, vec_predict_file);
	for (std::vector<std::wstring>::iterator it = vec_predict_file.begin(); it != vec_predict_file.end(); it++)
	{
		const std::wstring& str_pic_file = *it;
		std::wstring str_label;
		double k = 0.0;
		res = sc.predict(str_pic_file, str_label, k);
		if (res == face_recognition::result_success)
		{
			printf("[%ws]--->[%ws]<---[%f]\n", str_pic_file.c_str(), str_label.c_str(), k);
		}
		else
		{
			printf("[%ws] predict fail.\n", str_pic_file.c_str());
		}
	}
	getchar();
}