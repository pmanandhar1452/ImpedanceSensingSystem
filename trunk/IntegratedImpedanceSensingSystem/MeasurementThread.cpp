/*
 * MeasurementThread.cpp
 *
 *  Created on: Apr 25, 2010
 *      Author: Prakash Manandhar
 */

#include <QDateTime>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

#include "MeasurementThread.h"
#include "FrequencyListGenerator.h"
#include "LoggerTime.h"

MeasurementThread::MeasurementThread
		(MeasurementLoop * ml, ImpRobotExp * exp)
		: ml(ml), exp(exp), g(Global::instance()),
		  cInfo(g->getChannelInformation()), iCal(cInfo)
{
    pMLst = exp->getImpMeasurement();
    fList = FrequencyListGenerator::getFrequencies();
    sRList = FrequencyListGenerator::getSamplingRates();
    numSweep = g->getNumSweep();
    cyclesPerIm = g->getCyclesPerIm();
    amplitude = g->getAmplitude();
    NUM_CHANNELS = cInfo.size();
    fGenID = g->getA33220ADeviceID();
    _data = 0;
}

void MeasurementThread::run() {
    //qDebug() << "Inside MesurementThread::run()";
    canceled = false;
    initAcquisitionCard();
    initFreqGen();
    for (int ns = 0; ns < numSweep; ns++)
    {
            bool response;
            for (int fi = 0; fi < fList.size(); fi++)
            {
                emit updateProg();
                response = measureStep(ns, fi);
                ml->incFinishedSteps(cyclesPerIm/fList[fi]);
                if (canceled || response == false) break;
            }
            if (canceled || response == false) break;
    }
    emit finishProg();
}

bool MeasurementThread::measureStep(int ns, int fi)
{
    //qDebug() << "MeasurementStep: ns = " << ns << ", fi = " << fi;
    char buf [1000] = {0};
    QString strFreq = QString ("FREQ %1\n").arg(fList[fi]);
    viPrintf (vi, strFreq.toAscii().data());
    // allocate buffer
    long DATA_BUFFER_SIZE = calculateBufferSize(fi);

    int e_code;
    e_code = cbALoadQueue (BOARD_NUM, _ChanArray, _GainArray, NUM_CHANNELS);
    if (e_code != 0) {
            bool response =	createSweepErrorMsg(ns, fi, "Error in cbLoadQueue()");
            cbWinBufFree(_data); return response;
    }
    long actualSamplingRate = sRList[fi];
    viPrintf(vi, (ViString)"FREQ?\n");
    /* Read results */
    viScanf (vi, (ViString)"%t", &buf);

    //qDebug() << "Starting Scan";
    int startTime = LoggerTime::timer();
    e_code = cbAInScan (BOARD_NUM, 0, 15, DATA_BUFFER_SIZE, &actualSamplingRate,
                            _GainArray[0], _data, CONVERTDATA + DMAIO);
    int endTime = LoggerTime::timer();
    //qDebug() << "Ending Scan";
    if (e_code != 0) {
            bool response = createSweepErrorMsg(ns, fi, "Error in cbInScan()");
            cbWinBufFree(_data); return response;
    }
    ImpedanceMeasurement m = iCal.processData((WORD*)_data,
                    DATA_BUFFER_SIZE, fList[fi], actualSamplingRate);
    m.t = (startTime + endTime)/2;
    pMLst->append(m);
    return true;
}

bool MeasurementThread::initAcquisitionCard ()
{
	float RevLevel = (float)CURRENTREVNUM;
	cbDeclareRevision(&RevLevel);
	cbErrHandling (PRINTALL, DONTSTOP);
	// calculate maximum buffer size and allocate it
	long DATA_BUF_SIZE = 0;
	for (int i = 0; i < fList.size(); i++)
	{
		long s = calculateBufferSize(i);
		if (s > DATA_BUF_SIZE) DATA_BUF_SIZE = s;
	}
	_data = cbWinBufAlloc (DATA_BUF_SIZE);
	if (_data == 0)
	{
		bool response = createSweepErrorMsg(0, 0, "Cannot allocate memory");
		return response;
	}
	// setup channels
	for (int i = 0; i < cInfo.size(); ++i)
	{
		_ChanArray[i] = cInfo.value(i).ChannelNumber;
		_ChanTypeArray[i] = ANALOG;
                _GainArray[i] = Global::vMaxToGain(cInfo.value(i).Units);

	}
	return true;
}

long MeasurementThread::calculateBufferSize (int fi)
{
	double TOTAL_TIME = ((double)cyclesPerIm)/fList[fi];
        long bsize = ceil(TOTAL_TIME*sRList[fi]*NUM_CHANNELS);
        //qDebug() << "Total Time: " << TOTAL_TIME << ", Buffer size: " << bsize;
        return bsize;
}

bool MeasurementThread::initFreqGen()
{
	viOpenDefaultRM (&defaultRM);
	viOpen (defaultRM, fGenID.toAscii().data(), VI_NULL,VI_NULL, &vi);
	viPrintf (vi, (ViString)"*RST\n");
	viPrintf (vi, (ViString)"FUNC SIN\n");
	QString amplStr = QString ("VOLT %1\n").arg(amplitude/1000.0);
	viPrintf (vi, amplStr.toAscii().data());
	QString strFreq = QString ("FREQ %1\n").arg(fList[0]);
	viPrintf (vi, strFreq.toAscii().data());
	viPrintf (vi, (ViString)"OUTP ON\n");
	return true;
}


void MeasurementThread::cancel() {
	canceled = true;
}




void MeasurementThread::cleanup()
{
	/* Close visa session */
	viPrintf (vi, (ViString)"OUTP OFF\n");
	viClose (vi);
	viClose (defaultRM);
	if (_data != 0) cbWinBufFree(_data);
}

MeasurementThread::~MeasurementThread() {
	cleanup();
}

bool MeasurementThread::createSweepErrorMsg(int ns, int fi, QString mes)
{
	QString strMes =
			QString ("Sweep %1: Frequency(%2): %3 Hz\n")
			.arg(ns).arg(fi).arg(fList[fi]);
	strMes += mes;
	QMessageBox::StandardButtons res =
			QMessageBox::critical(exp, "Impedance Measurement DAQ", strMes,
			QMessageBox::Abort|QMessageBox::Ok);
	if (res == QMessageBox::Abort) return false;
	return true;
}
