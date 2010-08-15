/*
 * MeasurementLoop.cpp
 *
 *  Created on: Mar 26, 2010
 *      Author: Prakash Manandhar
 */

#include "MeasurementLoop.h"
#include <QDebug>
#include <QDir>
#include <QMessageBox>

#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <math.h>

#include "FrequencyListGenerator.h"
#include "MeasurementThread.h"

MeasurementLoop::MeasurementLoop(ImpRobotExp * exp,
		QList<ImpedanceMeasurement>* pIM)
        :AbstractMeasurementLoop(exp, pIM) { }


void MeasurementLoop::measure()
{
	finishedSteps = 0;
	estimatedTimeRemaining = FrequencyListGenerator::calculateTotalTime();
	Global *g = Global::instance();
	totalSteps = FrequencyListGenerator::
				getFrequencies().size() * g->getNumSweep();
	pDlg.setMaximum(totalSteps);
        MeasurementThread thread(this, (ImpRobotExp*)exp);
	connect(&pDlg, SIGNAL(cancelled()), &thread, SLOT(cancel()));
	connect(&thread, SIGNAL(updateProg()), this, SLOT(updateProg()));
	connect(&thread, SIGNAL(finishProg()), this, SLOT(finishProg()));
	pDlg.setFinished(false);
	thread.start();
	pDlg.exec();
}


QString MeasurementLoop::getTimeRemainingDisplayText()
{
	QString str = QString(
			"Impedance Measurement in Progress [%1/%2]\nEstimated Time Remaining: ")
				.arg(finishedSteps).arg(totalSteps);
	if (estimatedTimeRemaining > 60)
                str += Global::toHrMinSec(estimatedTimeRemaining);
	else
		str += QString("%1 seconds").arg(estimatedTimeRemaining);
	return str;
}

MeasurementLoop::~MeasurementLoop() { }

