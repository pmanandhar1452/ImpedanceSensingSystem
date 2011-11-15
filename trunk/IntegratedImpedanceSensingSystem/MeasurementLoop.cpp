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

MeasurementLoop::MeasurementLoop(AbstractExperimentDlg * exp,
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
        MeasurementThread thread(this, exp);
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
                str += QString("%1 secs").arg(estimatedTimeRemaining);
        QList<ImpedanceMeasurement> * msmt = exp->getImpMeasurement();
        //qDebug() << "displaying remaining time";
        Global *g = Global::instance();
        if (!msmt->isEmpty()) {
            ImpedanceMeasurement &last = msmt->last();
            QVector<ChannelInformation> cI = g->getChannelInformation();
            for (int i = 0; i < cI.size(); i++) {
                switch(cI.value(i).Type) {
                case ChannelInformation::CT_CARRIER:
                   str += QString("\nVc = %1 V")
                                   .arg(abs(last.X[i]));
                   break;
                case ChannelInformation::CT_IMPEDANCE:
                   str += QString("\nZ = %1 + %2 i")
                          .arg(last.X[i].real()).arg(last.X[i].imag());
                   break;
                case ChannelInformation::CT_HUMIDITY:
                   str += QString("\nRH = %1\%")
                          .arg(last.X[i].real());
                   break;
                case ChannelInformation::CT_TEMPERATURE:
                case ChannelInformation::CT_TEMP_5V:
                   str += QString("\nT = %1C")
                          .arg(last.X[i].real());
                   break;
                case ChannelInformation::CT_DC_VOLTS:
                case ChannelInformation::CT_AC_VOLTS:
                            break;
                }
            }
        }
        return str;
}

MeasurementLoop::~MeasurementLoop() { }

