/*
 * MeasurementThread.h
 *
 *  Created on: Apr 25, 2010
 *      Author: Prakash Manandhar
 */

#ifndef MEASUREMENTTHREAD_H_
#define MEASUREMENTTHREAD_H_

#include <QThread>

#include "ImpRobotExp.h"
#include "MeasurementLoop.h"
#include "ImpedanceCalculator.h"

class MeasurementThread: public QThread {
	Q_OBJECT
public:
	MeasurementThread(MeasurementLoop *, ImpRobotExp *);
	virtual ~MeasurementThread();

public slots:
	void cancel();

signals:
	void updateProg();
	void finishProg();

protected:
	virtual void run();

private:
	MeasurementLoop *ml;
	ImpRobotExp *exp;
	volatile bool canceled;

	void *_data;
	short _ChanArray[Global::N_CHANNELS];
	short _ChanTypeArray[Global::N_CHANNELS];
	short _GainArray[Global::N_CHANNELS];
	int NUM_CHANNELS;
	static const int BOARD_NUM = 0;

	QString fGenID;
	ViSession defaultRM, vi;

	void cleanup();
        bool createSweepErrorMsg (int ns, int fi, QString);
	bool measureStep(int ns, int fi);
	bool initAcquisitionCard();
	bool initFreqGen();
	long calculateBufferSize (int fi);

	QList<double> fList;
	QList<int> sRList;

	Global * g;
	QVector<ChannelInformation> cInfo;
	ImpedanceCalculator iCal;

	QList<ImpedanceMeasurement> * pMLst;

	int numSweep;
	double amplitude;
	int cyclesPerIm;

};

#endif /* MEASUREMENTTHREAD_H_ */
