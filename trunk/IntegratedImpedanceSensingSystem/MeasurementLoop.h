/*
 * MeasurementLoop.h
 *
 *  Created on: Mar 26, 2010
 *      Author: Prakash Manandhar
 */

#ifndef MEASUREMENTLOOP_H_
#define MEASUREMENTLOOP_H_

#include <QWidget>
#include <QProgressDialog>
#include <QVector>
#include <cbw.h>
#include <visa.h>

#include "Global.h"
#include "Measurement.h"
#include "measurementprogdlg.h"
#include "improbotexp.h"
#include "AbstractMeasurementLoop.h"

class MeasurementLoop: public AbstractMeasurementLoop {
	Q_OBJECT
public:
	MeasurementLoop(ImpRobotExp *exp, QList<ImpedanceMeasurement> *);
	void measure ();
	virtual ~MeasurementLoop();

protected:
        QString getTimeRemainingDisplayText();
};

#endif /* MEASUREMENTLOOP_H_ */
