/*
 * AbstractMeasurementLoop.h
 *
 *  Created on: Aug 11, 2010
 *      Author: Prakash Manandhar
 */

#ifndef ABSTRACTMEASUREMENTLOOP_H
#define ABSTRACTMEASUREMENTLOOP_H

#include <QObject>
#include <QList>

#include "abstractexperimentdlg.h"
#include "Measurement.h"
#include "measurementprogdlg.h"

class AbstractMeasurementLoop : public QObject
{
    Q_OBJECT
public:
    explicit AbstractMeasurementLoop(AbstractExperimentDlg *exp, QList<ImpedanceMeasurement> *);
    virtual void measure () = 0;
    virtual ~AbstractMeasurementLoop();

    void incFinishedSteps(double esTDelta);

public slots:
    void updateProg();
    void finishProg();

protected:

    virtual QString getTimeRemainingDisplayText() = 0;

    QString dirPath;

    volatile int finishedSteps;
    volatile double estimatedTimeRemaining;

    int totalSteps;

    MeasurementProgDlg pDlg;
    AbstractExperimentDlg * exp;
    QList<ImpedanceMeasurement> * pIM;

};

#endif // ABSTRACTMEASUREMENTLOOP_H
