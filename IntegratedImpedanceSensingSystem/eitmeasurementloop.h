#ifndef EITMEASUREMENTLOOP_H
#define EITMEASUREMENTLOOP_H

/*
 * EITMeasurementLoop.h
 *
 *  Created on: Aug 11, 2010
 *      Author: Prakash Manandhar
 */


#include "AbstractMeasurementLoop.h"
#include "measurementprogdlg.h"
#include "eit8electrodedlg.h"

class EITMeasurementLoop : public AbstractMeasurementLoop
{
    Q_OBJECT
public:
    explicit EITMeasurementLoop(EIT8ElectrodeDlg *exp, QList<ImpedanceMeasurement> *);

    void measure();

protected:

    QString getTimeRemainingDisplayText();

private:
    EIT8ElectrodeDlg * pEITExp;

};

#endif // EITMEASUREMENTLOOP_H
