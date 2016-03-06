#pragma once

#include "preprocessor.h"

namespace face_recognition
{
	class preprocessor_manager : public boost::noncopyable
	{
	public:
		static boost::shared_ptr<preprocessor_manager> create();
		void destroy();
		void add_preprocessor(boost::shared_ptr<preprocessor> sp_preprocessor);
		result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<picture>& sp_pic_out);
		unsigned int get_preprocess_count();
	private:
		std::vector<boost::shared_ptr<preprocessor> > m_vec_preprocessor;
	};
}