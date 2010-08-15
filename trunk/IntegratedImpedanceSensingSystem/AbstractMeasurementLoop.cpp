/*
 * AbstractMeasurementLoop.cpp
 *
 *  Created on: Aug 11, 2010
 *      Author: Prakash Manandhar
 */

#include "AbstractMeasurementLoop.h"

AbstractMeasurementLoop::AbstractMeasurementLoop(QDialog * exp,
                QList<ImpedanceMeasurement>* pIM)
        :exp(exp), pIM(pIM) { }

void AbstractMeasurementLoop::updateProg() {
    //qDebug() << "MeasurementLoop::updateProg()";
    pDlg.setProgress(finishedSteps);
    pDlg.setStatus(getTimeRemainingDisplayText());
}

void AbstractMeasurementLoop::incFinishedSteps(double eD) {
        finishedSteps++;
        estimatedTimeRemaining -= eD;
}

AbstractMeasurementLoop::~AbstractMeasurementLoop() { }

void AbstractMeasurementLoop::finishProg() {
        pDlg.setFinished(true);
        pDlg.close();
}
