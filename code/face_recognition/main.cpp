#include "../face_recognition_lib/scheme.h"
#include "../frame/util_path.h"


void main()
{
	face_recognition::scheme sc;
	sc.train(L"S:\\local\\face_recognition\\code\\data\\train_picture\\", 
		L"S:\\local\\face_recognition\\code\\data\\haarcascades\\haarcascade_frontalface_alt.xml",
		1, 1, 2000, 2000, 100, 98, 255);


	std::vector<std::wstring> vec_predict_file;
	util_path::list_dir_files(L"S:\\local\\face_recognition\\code\\data\\predict_picture\\", vec_predict_file);
	for (std::vector<std::wstring>::iterator it = vec_predict_file.begin(); it != vec_predict_file.end(); it++)
	{
		const std::wstring& str_pic_file = *it;
		std::wstring str_label;
		double k;
		sc.predict(str_pic_file, str_label, k);
		printf("[%ws]--->[%ws]<---[%f]\n", str_pic_file.c_str(), str_label.c_str(), k);
	}
	

	getchar();
}