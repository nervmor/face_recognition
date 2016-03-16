#pragma once


#define GRAY_HANDLE_STATE							L"gray_handled_state"
#define EUQALIZATION_HANDLE_STATE					L"equalization_handled_state"
#define	BLUR_HANDLE_STATE							L"blur_handle_state"
#define THRESHOLD_HANDLE_STATE						L"threshold_handle_state"
#define EDGE_HANDLE_STATE							L"edge_handle_state"
#define FACE_ALIGN_STATE							L"face_align_state"
#define SIZE_ALIGN_STATE							L"size_align_state"
#define FACE_COUNTOUR_MASK_STATE					L"face_contour_mask_state"



#define GRAY_PREPROCESSOR_TAG						"gray_preprocessor"
#define EQUALIZATION_PREPROCESSOR_TAG				"equalization_preprocessor"
#define BLUR_PREPROCESSOR_TAG						"blur_preprocessor"
#define THRESHOLD_PREPROCESSOR_TAG					"blur_preprocessor"
#define EDGE_PREPROCESSOR_TAG						"edge_preprocessor"
#define FACE_ARRA_DETECT_PREPROCESSOR_TAG			"face_area_detect_preprocessor"
#define EYES_POINT_DETECT_PREPROCESSOR_TAG			"eyes_point_detect_preprocessor"
#define FACE_ALIGN_PREPROCESSOR_TAG					"face_align_preprocessor"
#define SIZE_ALIGN_PREPROCESSOR_TAG					"size_align_preprocessor"
#define FACE_COUNTOUR_MASK_PREPROCESSOR_TAG			"face_contour_mask_preprocessor"




// boost::shared_ptr<pic_rect>
#define FACE_AREA_RECT								L"face_area_rect"
// boost::shared_ptr<pic_point>
#define LEFT_EYE_POINT								L"left_eye_point"
// boost::shared_ptr<pic_point>
#define RIGHT_EYE_POINT								L"right_eye_point"