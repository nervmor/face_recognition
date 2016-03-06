#pragma once

#include <boost/noncopyable.hpp>

template<class T>
class singleton : public boost::noncopyable
{
public:
	static T* instance()
	{
		static T* s_instance = NULL;
		if (s_instance == NULL)
		{
			if (s_instance == NULL)
			{
				s_instance = new T();
			}
		}
		return s_instance;
	}
public:
	virtual ~singleton(){}
protected:
	singleton(){}
};

#ifdef _WIN32
#define SINLETON_CLASS_DECLARE(class_name)	\
	friend class singleton<##class_name>;
#else
#define SINLETON_CLASS_DECLARE(class_name)	\
	friend class singleton<class_name>;
#endif // _WIN32
