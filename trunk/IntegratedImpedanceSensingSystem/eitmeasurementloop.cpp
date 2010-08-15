/*
 * EITMeasurementLoop.cpp
 *
 *  Created on: Aug 11, 2010
 *      Author: Prakash Manandhar
 */

#include "eitmeasurementloop.h"
#include "EITMeasurementThread.h"

EITMeasurementLoop::EITMeasurementLoop(EIT8ElectrodeDlg * exp,
                                       QList<ImpedanceMeasurement>* pIM)
                               :AbstractMeasurementLoop(exp, pIM), pEITExp(exp) { }

void EITMeasurementLoop::measure()
{
        finishedSteps = 0;
        estimatedTimeRemaining = ((EIT8ElectrodeDlg*)exp)->getTotalTime();
        Global *g = Global::instance();
        totalSteps = pEITExp->N_ELECTRODES * g->getEITExpCycles();
        pDlg.setMaximum(totalSteps);
        EITMeasurementThread thread(this, (EIT8ElectrodeDlg*)exp);
        connect(&pDlg, SIGNAL(cancelled()), &thread, SLOT(cancel()));
        connect(&thread, SIGNAL(updateProg()), this, SLOT(updateProg()));
        connect(&thread, SIGNAL(finishProg()), this, SLOT(finishProg()));
        pDlg.setFinished(false);
        thread.start();
        pDlg.exec();
}


QString EITMeasurementLoop::getTimeRemainingDisplayText()
{
        QString str = QString(
                        "EIT Experiment in Progress [%1/%2]\nEstimated Time Remaining: ")
                                .arg(finishedSteps).arg(totalSteps);
        if (estimatedTimeRemaining > 60)
                str += Global::toHrMinSec(estimatedTimeRemaining);
        else
                str += QString("%1 seconds").arg(estimatedTimeRemaining);
        return str;
}
