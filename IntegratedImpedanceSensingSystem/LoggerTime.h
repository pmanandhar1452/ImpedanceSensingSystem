/*
 * LoggerTime.h
 *
 *  Created on: Oct 15, 2008
 *      Author: PManandhar
 */

#ifndef LOGGERTIME_H_
#define LOGGERTIME_H_

#include <QTime>

/** Keeps global time in ms **/
class LoggerTime {
public:
	static void reset ();
	static int timer ();
private:
	static QTime t;
};

#endif /* LOGGERTIME_H_ */
