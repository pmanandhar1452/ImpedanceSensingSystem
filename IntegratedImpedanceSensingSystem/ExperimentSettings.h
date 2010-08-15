/*
 * ExperimentSettings.h
 *
 *  Created on: Apr 1, 2010
 *      Author: Prakash Manandhar
 */

#ifndef EXPERIMENTSETTINGS_H_
#define EXPERIMENTSETTINGS_H_

#include <QString>
#include <QList>
#include <QVector>
#include <QDomDocument>
#include <QMutex>
#include <QDateTime>
#include <complex>

#include "Measurement.h"
#include "ChannelInformation.h"

class FrequencySweep {
public:
	int Index;
	int CarrierHz;
	int SamplingHz;
};


class ExperimentSettings {
public:
	int Amp_mV;
	int CyclesPerIm;
	int NumSweeps;
	QString dirPath;
	QList<ChannelInformation> Channels;
	QList<FrequencySweep> Frequencies;
	void load (QDomDocument);
	QString getSummary();
	void addMeasurement(ImpedanceMeasurement);
	void writeResultsToFile();
	QDateTime tInitial;

	static const char * DT_FORMAT;// = "dd.MM.yyyy hh.mm.ss.zzz";

private:
	QList<ImpedanceMeasurement> mList;
	QMutex mutex;
	void loadTInitial();
};

#endif /* EXPERIMENTSETTINGS_H_ */
