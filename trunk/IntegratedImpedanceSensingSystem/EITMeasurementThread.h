#ifndef EITMEASUREMENTTHREAD_H
#define EITMEASUREMENTTHREAD_H

/*
 * EITMeasurementThread.h
 *
 *  Created on: Aug 11, 2010
 *      Author: Prakash Manandhar
 */

#include <QThread>
#include <visa.h>

#include "eitmeasurementloop.h"
#include "eit8electrodedlg.h"
#include "ImpedanceCalculator.h"

class EITMeasurementThread : public QThread
{
    Q_OBJECT
public:
    explicit EITMeasurementThread(EITMeasurementLoop *, EIT8ElectrodeDlg *);
    virtual ~EITMeasurementThread();

public slots:
    void cancel();

signals:
    void updateProg();
    void finishProg();

protected:
    virtual void run();

private:
    EITMeasurementLoop *ml;
    EIT8ElectrodeDlg *exp;
    volatile bool canceled;

    void *_data;
    short _ChanArray[Global::N_CHANNELS];
    short _ChanTypeArray[Global::N_CHANNELS];
    short _GainArray[Global::N_CHANNELS];
    static const int NUM_CHANNELS = EIT8ElectrodeDlg::N_ELECTRODES + 1;
    static const int BOARD_NUM = 0;

    void cleanup();

    bool createSweepErrorMsg (int ci, int si, QString);
    bool measureStep(int ns, int fi);
    bool initAcquisitionCard();
    bool initSwitchMatrix();

    bool closeSwitch(int si);
    bool openSwitch(int si);

    Global * g;

    const int onTime, offTime;
    const double stepTime;

    const long fsamp;
    const long fcarr;
    const double amplitude;

    const long DATA_BUF_SIZE;

    QList<ImpedanceMeasurement> * pMLst;
    QVector<ChannelInformation> cInfo;
    ImpedanceCalculator iCal;

    const QString swMatID, siGenID;
    ViSession defaultRM, viSwMat, viSiGen;

    static const char * CMD_SWITCH_CLOSE[EIT8ElectrodeDlg::N_ELECTRODES_ACTIVE];
    static const char * CMD_SWITCH_OPEN [EIT8ElectrodeDlg::N_ELECTRODES_ACTIVE];
    static const char * CMD_SWITCH_CLOSE_TEST[EIT8ElectrodeDlg::N_ELECTRODES_ACTIVE];
    static const char * CMD_SWITCH_OPEN_TEST [EIT8ElectrodeDlg::N_ELECTRODES_ACTIVE];
};

#endif // EITMEASUREMENTTHREAD_H
