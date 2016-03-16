#pragma once

#include "preprocessor.h"

namespace face_recognition
{
	class preprocessor_manager : public boost::noncopyable
	{
	public:
		static boost::shared_ptr<preprocessor_manager> create();
		void destroy();
		result add_preprocessor(boost::shared_ptr<preprocessor> sp_preprocessor);
		result process(boost::shared_ptr<picture> sp_pic_in, const std::wstring& str_pic_filename, boost::shared_ptr<picture>& sp_pic_out);
		unsigned int get_preprocess_count();
	private:
		std::vector<boost::shared_ptr<preprocessor> > m_vec_preprocessor;
	};
}