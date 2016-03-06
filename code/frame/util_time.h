#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>   
#define BOOST_DATE_TIME_SOURCE   



class util_time
{
public:
	static std::string get_current_time_string()
	{
		std::string str_time = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
		int pos = str_time.find('T');
		str_time.replace(pos,1,std::string("_"));  
		str_time.replace(pos + 3,0,std::string("_"));  
		str_time.replace(pos + 6,0,std::string("_"));  
		return str_time;
	}
};