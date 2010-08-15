/*
 * LoggerTime.cpp
 *
 *  Created on: Oct 15, 2008
 *      Author: PManandhar
 */

#include "LoggerTime.h"

QTime LoggerTime::t;

void LoggerTime::reset()
{
	t.start();
}

int LoggerTime::timer()
{
	return t.elapsed();
}
