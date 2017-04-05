//
// Created by sen on 2017/3/25.
//

#ifndef OHMYTUNNEL_LOGGER_H
#define OHMYTUNNEL_LOGGER_H

#include <stdio.h>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <sstream>

enum LOG_LV { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_ERROR, LOG_LAST };
const char * const LOG_LV_TO_STR[] = { "TRACE", "DEBUG", "INFO", "ERROR"};

inline std::string createCommon() 
{
	std::string str;
	std::string strTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
	int pos = strTime.find('T');
	strTime.replace(pos, 1, std::string("-"));
	strTime.replace(pos + 3, 0, std::string(":"));
	strTime.replace(pos + 6, 0, std::string(":"));
	std::stringstream ss;
	ss << boost::this_thread::get_id();
	return std::string("[") + strTime + "][" + ss.str() + "]";
}

LOG_LV &getLogLv();
void initLog(LOG_LV lv);

#define SF_LOG(lv, fmt, ...) do {\
	if(lv >= getLogLv()) \
		printf("%s[%s:%d][%s]" fmt "\n", createCommon().c_str(), __FUNCTION__, __LINE__, LOG_LV_TO_STR[lv], ##__VA_ARGS__); \
} while(false)


#endif //OHMYTUNNEL_LOGGER_H
