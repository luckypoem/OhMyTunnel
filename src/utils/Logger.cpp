//
// Created by sen on 2017/3/25.
//

#include "Logger.h"

LOG_LV &getLogLv()
{
	static LOG_LV lv = LOG_TRACE;
	return lv;
}

void initLog(LOG_LV lv)
{
    if(lv >= LOG_LAST)
        lv = LOG_ERROR;
	getLogLv() = lv;
}


