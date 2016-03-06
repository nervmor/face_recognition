#pragma once

#include "picture.h"
#include "context.h"

namespace face_recognition
{
	class preprocessor_factroy;

	class preprocessor : public boost::noncopyable
	{
	public:
		virtual ~preprocessor(){}
		virtual std::wstring name() = 0;
		virtual result process(boost::shared_ptr<picture> sp_pic_in, boost::shared_ptr<context> sp_ctx, boost::shared_ptr<picture>& sp_pic_out) = 0;
	public:
		friend class preprocessor_factroy;
	};

	class preprocessor_factroy
	{
	public:
		template <class PreprocessorType>
		static boost::shared_ptr<preprocessor> create_preprocessor()
		{
			boost::shared_ptr<preprocessor> sp_preprocessor(boost::static_pointer_cast<preprocessor>(boost::make_shared<PreprocessorType>()));
			return sp_preprocessor;
		}
		template <class PreprocessorType, class Param1Type>
		static boost::shared_ptr<preprocessor> create_preprocessor(Param1Type param1)
		{
			boost::shared_ptr<preprocessor> sp_preprocessor(boost::static_pointer_cast<preprocessor>(boost::make_shared<PreprocessorType>(param1)));
			return sp_preprocessor;
		}
		template <class PreprocessorType, class Param1Type, class Param2Type>
		static boost::shared_ptr<preprocessor> create_preprocessor(Param1Type param1, Param2Type param2)
		{
			boost::shared_ptr<preprocessor> sp_preprocessor(boost::static_pointer_cast<preprocessor>(boost::make_shared<PreprocessorType>(param1, param2)));
			return sp_preprocessor;
		}
		template <class PreprocessorType, class Param1Type, class Param2Type, class Param3Type>
		static boost::shared_ptr<preprocessor> create_preprocessor(Param1Type param1, Param2Type param2, Param3Type param3)
		{
			boost::shared_ptr<preprocessor> sp_preprocessor(boost::static_pointer_cast<preprocessor>(boost::make_shared<PreprocessorType>(param1, param2, param3)));
			return sp_preprocessor;
		}
		template <class PreprocessorType, class Param1Type, class Param2Type, class Param3Type, class Param4Type>
		static boost::shared_ptr<preprocessor> create_preprocessor(Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4)
		{
			boost::shared_ptr<preprocessor> sp_preprocessor(boost::static_pointer_cast<preprocessor>(boost::make_shared<PreprocessorType>(param1, param2, param3, param4)));
			return sp_preprocessor;
		}
		template <class PreprocessorType, class Param1Type, class Param2Type, class Param3Type, class Param4Type, class Param5Type>
		static boost::shared_ptr<preprocessor> create_preprocessor(Param1Type param1, Param2Type param2, Param3Type param3, Param4Type param4, Param5Type param5)
		{
			boost::shared_ptr<preprocessor> sp_preprocessor(boost::static_pointer_cast<preprocessor>(boost::make_shared<PreprocessorType>(param1, param2, param3, param4, param5)));
			return sp_preprocessor;
		}
	};
}