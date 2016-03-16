#pragma once
#include "preprocessor.h"
#include "preprocess_common.h"
#include "misc.h"

namespace face_recognition
{
	class size_align_preprocessor : public preprocessor
	{
	public:
		virtual std::wstring name()
		{
			return L"size_align_preprocessor";
		}
		virtual result init()
		{
			return result_success;
		}
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out)
		{
			bool size_align_state = false;
			result res = sp_ctx->get_bool_value(SIZE_ALIGN_STATE, size_align_state);
			if (res == result_success)
			{
				if (size_align_state)
				{
					util_log::log(SIZE_ALIGN_PREPROCESSOR_TAG, "context size align state have been set. size align repeatly.");
					return result_already_handled;
				}
			}

			bool need_cut = false;
			int cut_x, cut_y, cut_width, cut_height;
			if (sp_pic_in->width() > sp_pic_in->height())
			{
				cut_x = (sp_pic_in->width() - sp_pic_in->height()) / 2;
				cut_y = 0;
				cut_width = sp_pic_in->height();
				cut_height = sp_pic_in->height();
				need_cut = true;
				util_log::logd(SIZE_ALIGN_PREPROCESSOR_TAG, "the picture's width[%d] > height[%d]. cut it to square with height length.", sp_pic_in->width(), sp_pic_in->height());
			}
			else if (sp_pic_in->width() < sp_pic_in->height())
			{
				cut_x = 0;
				cut_y = (sp_pic_in->height() - sp_pic_in->width()) / 2;
				cut_width = sp_pic_in->width();
				cut_height = sp_pic_in->width();
				need_cut = true;
				util_log::logd(SIZE_ALIGN_PREPROCESSOR_TAG, "the picture's width[%d] < height[%d]. cut it to square with width length.", sp_pic_in->width(), sp_pic_in->height());
			}
			else
			{
				util_log::logd(SIZE_ALIGN_PREPROCESSOR_TAG, "the picture's width[%d] = height[%d]. don't need to cut.", sp_pic_in->width(), sp_pic_in->height());
			}

			boost::shared_ptr<picture> sp_cutted_pic;
			if (need_cut)
			{
				res = picture_handler::cut(sp_pic_in, cut_x, cut_y, cut_width, cut_height, sp_cutted_pic);
				if (res != result_success)
				{
					util_log::log(SIZE_ALIGN_PREPROCESSOR_TAG, "cut picture to square fail with result[%s].", result_string(res));
					return res;
				}
			}
			else
			{
				sp_cutted_pic = sp_pic_in;
			}
			Assert(sp_cutted_pic->width() == sp_cutted_pic->height());
			if (sp_cutted_pic->width() != m_length)
			{
				util_log::logd(SIZE_ALIGN_PREPROCESSOR_TAG, "the cutted square picture size is length[%d]. it will be resized to length[%d] by the scale.", sp_pic_in->width(), m_length);
				res = picture_handler::resize(sp_cutted_pic, m_length, m_length, sp_pic_out);
				if (res != result_success)
				{
					util_log::log(SIZE_ALIGN_PREPROCESSOR_TAG, "cutted square picture scale to define length fail with result[%s].", result_string(res));
					return res;
				}
			}
			else
			{
				sp_pic_out = sp_cutted_pic;
			}
			sp_ctx->set_bool_value(SIZE_ALIGN_STATE, true);
			util_log::logd(SIZE_ALIGN_PREPROCESSOR_TAG, "size align success and set context [%ws] to true.", SIZE_ALIGN_STATE);
			return result_success;
		}
	public:
		size_align_preprocessor(unsigned int length)
			: m_length(length)
		{}
	private:
		unsigned int m_length;
	};
}