
/*
 * EITMeasurementThread.cpp
 *
 *  Created on: Aug 11, 2010
 *      Author: Prakash Manandhar
 */

#include <QDateTime>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

#include "EITMeasurementThread.h"
#include "LoggerTime.h"


const char * EITMeasurementThread::CMD_SWITCH_CLOSE[] = {
       "ROUT:CLOS (@101)\n",
       "ROUT:CLOS (@302)\n",
       "ROUT:CLOS (@103)\n",
       "ROUT:CLOS (@304)\n",
       "ROUT:CLOS (@105)\n",
       "ROUT:CLOS (@306)\n",
       "ROUT:CLOS (@107)\n",
       "ROUT:CLOS (@308)\n"
   };

const char * EITMeasurementThread::CMD_SWITCH_OPEN[] = {
       "ROUT:OPEN (@101)\n",
       "ROUT:OPEN (@302)\n",
       "ROUT:OPEN (@103)\n",
       "ROUT:OPEN (@304)\n",
       "ROUT:OPEN (@105)\n",
       "ROUT:OPEN (@306)\n",
       "ROUT:OPEN (@107)\n",
       "ROUT:OPEN (@308)\n"
   };


EITMeasurementThread::EITMeasurementThread
    (EITMeasurementLoop * ml, EIT8ElectrodeDlg * exp)
    : ml(ml), exp(exp), g(Global::instance()),

        onTime(g->getEITOnTime()),
        offTime(g->getEITOffTime()),
        stepTime((onTime + offTime)/1000.0),
        fsamp(g->getEITSamplingFreq()),
        fcarr(g->getFrequencyStart()),
        amplitude(g->getAmplitude()),

        DATA_BUF_SIZE(ceil(g->getEITSamplingFreq()*NUM_CHANNELS*onTime/1000.0)),
        cInfo(exp->getChannelInformation()), iCal(cInfo),
        swMatID(g->getU2751ADeviceID()), siGenID(g->getA33220ADeviceID())
{
    pMLst = exp->getImpMeasurement();
    _data = 0;
}

void EITMeasurementThread::run() {
    //qDebug() << "Inside MesurementThread::run()";
    canceled = false;
    initAcquisitionCard();
    initSwitchMatrix();
    for (int ci = 0; ci < g->getEITExpCycles(); ci++)
    {
            bool response;
            for (int si = 0; si < exp->N_ELECTRODES; si++)
            {
                emit updateProg();
                response = measureStep(ci, si);
                ml->incFinishedSteps(stepTime);
                if (canceled || response == false) break;
            }
            if (canceled || response == false) break;
    }
    emit finishProg();
}

bool EITMeasurementThread::measureStep(int ci, int si)
{

    int e_code;
    e_code = cbALoadQueue (BOARD_NUM, _ChanArray, _GainArray, NUM_CHANNELS);
    if (e_code != 0) {
            bool response =	createSweepErrorMsg(ci, si, "Error in cbLoadQueue()");
            cbWinBufFree(_data); return response;
    }
    long actualSamplingRate = fsamp;
    viPrintf (viSwMat, (ViString)CMD_SWITCH_CLOSE[si]);
    this->msleep(exp->SWITCHING_DELAY);
    //qDebug() << "Starting Scan " << NUM_CHANNELS << " Chans, Buffer Size: " << DATA_BUF_SIZE
    //        << ", Sampling Rate: " << fsamp;
    int startTime = LoggerTime::timer();
    e_code = cbAInScan (BOARD_NUM, 0, 15, DATA_BUF_SIZE, &actualSamplingRate,
                            _GainArray[0], _data, CONVERTDATA + DMAIO);
    int endTime = LoggerTime::timer();
    viPrintf (viSwMat, (ViString)CMD_SWITCH_OPEN[si]);
    this->msleep(exp->SWITCHING_DELAY);
    //qDebug() << "Ending Scan";
    if (e_code != 0) {
            bool response = createSweepErrorMsg(ci, si, "Error in cbInScan()");
            cbWinBufFree(_data); return response;
    }
    ImpedanceMeasurement m = iCal.processData((WORD*)_data,
                    DATA_BUF_SIZE, fcarr, actualSamplingRate);
    m.t = (startTime + endTime)/2;

    pMLst->append(m);
    this->msleep(this->offTime);
    return true;
}

bool EITMeasurementThread::initAcquisitionCard ()
{
    float RevLevel = (float)CURRENTREVNUM;
    cbDeclareRevision(&RevLevel);
    cbErrHandling (PRINTALL, DONTSTOP);

    _data = cbWinBufAlloc (DATA_BUF_SIZE);
    if (_data == 0)
    {
        bool response = createSweepErrorMsg(0, 0, "Cannot allocate memory");
        return response;
    }
    // setup channels
    for (int i = 0; i < NUM_CHANNELS; ++i)
    {
        _ChanArray[i] = i;
        _ChanTypeArray[i] = ANALOG;
        _GainArray[i] = Global::vMaxToGain(exp->V_MAX);
    }
    return true;
}

bool EITMeasurementThread::initSwitchMatrix()
{
    viOpenDefaultRM (&defaultRM);
    viOpen (defaultRM, swMatID.toAscii().data(), VI_NULL,VI_NULL, &viSwMat);
    viPrintf (viSwMat, (ViString)"*RST\n");

    viOpen (defaultRM, siGenID.toAscii().data(), VI_NULL,VI_NULL, &viSiGen);
    viPrintf (viSiGen, (ViString)"*RST\n");
    viPrintf (viSiGen, (ViString)"FUNC SIN\n");
    QString amplStr = QString ("VOLT %1\n").arg(amplitude/1000.0);
    viPrintf (viSiGen, amplStr.toAscii().data());
    QString strFreq = QString ("FREQ %1\n").arg(fcarr);
    viPrintf (viSiGen, strFreq.toAscii().data());
    viPrintf (viSiGen, (ViString)"OUTP ON\n");
    this->msleep(1000);
    return true;
}


void EITMeasurementThread::cancel() { canceled = true; }

void EITMeasurementThread::cleanup()
{
        /* Close visa session */
    viPrintf (viSwMat, (ViString)"*RST\n");
    viClose (viSwMat);

    viPrintf (viSiGen, (ViString)"OUTP OFF\n");
    viClose (viSiGen);

    viClose (defaultRM);

    if (_data != 0) cbWinBufFree(_data);
}

EITMeasurementThread::~EITMeasurementThread() {
        cleanup();
}

bool EITMeasurementThread::createSweepErrorMsg(int ci, int si, QString mes)
{
        QString strMes =
                QString ("Cycle %1: Stimulation Step: %2 \n")
                        .arg(ci).arg(si);
        strMes += mes;
        QMessageBox::StandardButtons res =
                        QMessageBox::critical(exp, "EIT Experiment Error", strMes,
                        QMessageBox::Abort|QMessageBox::Ok);
        if (res == QMessageBox::Abort) return false;
        return true;
}
