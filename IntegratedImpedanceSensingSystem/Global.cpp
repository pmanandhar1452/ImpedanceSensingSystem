/*
 * Global.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: Chhitiz
 */

#include "Global.h"
#include <QDebug>
#include <math.h>
#include <cbw.h>

Global * Global::pI = NULL;

Global::Global() {
	// load settings
	settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
			"p-manandhar.info", "ImpedanceAnalsysisDAQ");
}

QString Global::getA33220ADeviceID()
{
	return settings->value("Ag33220ID").toString();
}

void Global::setA33220ADeviceID(QString id)
{
	settings->setValue("Ag33220ID", id);
	settings->sync();
}

QString Global::getU2751ADeviceID()
{
        return settings->value("U2751AID").toString();
}

void Global::setU2751ADeviceID(QString id)
{
        settings->setValue("U2751AID", id);
        settings->sync();
}

void Global::setNumSweep(int v)
{
	settings->setValue("NumSweep", v);
	settings->sync();
}

int Global::getNumSweep()
{
	return (settings->value("NumSweep").toUInt());
}

Global::~Global() {
	delete settings;
}

double Global::getAmplitude()
{
	return settings->value("Amplitude").toDouble();
}

void Global::setAmplitude(double v)
{
	settings->setValue("Amplitude", v);
	settings->sync();
}

double Global::getFrequencyStart()
{
	return settings->value("FrequencyStart").toDouble();
}

void Global::setFrequencyStart(double v)
{
	settings->setValue("FrequencyStart", v);
	settings->sync();
}

double Global::getFrequencyEnd()
{
	return settings->value("FrequencyEnd").toDouble();
}

void Global::setFrequencyEnd(double v)
{
	settings->setValue("FrequencyEnd", v);
	settings->sync();
}

int Global::getFrequencySteps()
{
	int s = settings->value("FrequencySteps").toUInt();
	if (s == 0)
	{
		s = 1;
		setFrequencySteps(s);
	}
	return s;
}

void Global::setFrequencySteps(int v)
{
	settings->setValue("FrequencySteps", v);
	settings->sync();
}

void Global::setFreqSweep(int v)
{
	qDebug() << "Frequency Sweep Changed To: " << v;
	settings->setValue("FrequencySweep", v);
	settings->sync();
}

int Global::getFreqSweep()
{
	return (settings->value("FrequencySweep").toUInt());
}

void Global::setCyclesPerIm(int v)
{
	settings->setValue("CyclesPerImpedanceMeasurement", v);
	settings->sync();
}


int Global::getCyclesPerIm()
{
	return (settings->value("CyclesPerImpedanceMeasurement").toUInt());
}

void Global::setChannelSelect(QVector<bool> csel)
{
	QString s;
	for (int i = 0; i < N_CHANNELS; i++)
	{
		if (csel[i]) s += '1';
		else s += '0';
	}
	settings->setValue("DAQChannelsSelect", s);
	settings->sync();
}

QVector<bool> Global::getChannelSelect()
{
	QVector<bool> csel(N_CHANNELS);
	QString s(settings->value("DAQChannelsSelect").toString());
	for (int i = 0; i < N_CHANNELS; i++)
	{
		if (s.size() <= i) csel[i] = false;
		else
			csel[i] = (s[i] == QChar('1'));
	}
	return csel;
}

void Global::setChannelScaling(QVector<ChannelInformation::DAQ_SCALING> c)
{
	QString s;
	for (int i = 0; i < N_CHANNELS; i++)
	{
		s += QString("%1").arg(c[i]);
	}
	settings->setValue("DAQChannelsResolution", s);
	settings->sync();
}

QVector<ChannelInformation::DAQ_SCALING> Global::getChannelScaling()
{
	QVector<ChannelInformation::DAQ_SCALING> csel(N_CHANNELS);
	QString s(settings->value("DAQChannelsResolution").toString());
	for (int i = 0; i < N_CHANNELS; i++)
	{
		if (s.size() <= i) csel[i] = ChannelInformation::DAQPM_5V;
		else
			csel[i] = (ChannelInformation::DAQ_SCALING)(QString(s[i]).toInt());
	}
	return csel;
}

void Global::setChannelType(QVector<ChannelInformation::CHAN_TYPE> c)
{
	QString s;
	for (int i = 0; i < N_CHANNELS; i++)
	{
		s += QString("%1").arg(c[i]);
	}
	settings->setValue("DAQChannelType", s);
	settings->sync();
}

QVector<ChannelInformation::CHAN_TYPE> Global::getChannelType()
{
	QVector<ChannelInformation::CHAN_TYPE> csel(N_CHANNELS);
	QString s(settings->value("DAQChannelType").toString());
	for (int i = 0; i < N_CHANNELS; i++)
	{
		if (s.size() <= i) csel[i] = ChannelInformation::CT_IMPEDANCE;
		else
			csel[i] = (ChannelInformation::CHAN_TYPE)
						(QString(s[i]).toInt());
	}
	return csel;
}


void Global::setSamplingFactor(int v)
{
	settings->setValue("SamplingFactor", v);
	settings->sync();
}


int Global::getSamplingFactor()
{
	int v (settings->value("SamplingFactor").toUInt());
	if (v < 2)
	{
		v = 2;
		setSamplingFactor(2);
	}
	return v;
}

void Global::setSamplingMax(int v)
{
	settings->setValue("SamplingMax", v);
	settings->sync();
}


int Global::getSamplingMax()
{
	int v = (settings->value("SamplingMax").toUInt());
	if (v < 100)
	{
		v = 100;
		setSamplingMax(v);
	}
	return v;
}

void Global::setSeed(int v)
{
	settings->setValue("Seed", v);
	settings->sync();
}


int Global::getSeed()
{
	int v = (settings->value("Seed").toUInt());
	return v;
}

void Global::setNumEvents(int v)
{
	settings->setValue("NumEvents", v);
	settings->sync();
}


int Global::getNumEvents()
{
	int v = (settings->value("NumEvents").toUInt());
	return v;
}

QVector<ChannelInformation> Global::getChannelInformation() {
	QVector<bool> cSel = getChannelSelect();
	QVector<ChannelInformation::DAQ_SCALING> cS = getChannelScaling();
	QVector<ChannelInformation::CHAN_TYPE> cT = getChannelType();
	QVector<double> cR = getSeriesR();
	QVector<ChannelInformation> cI(cSel.count(true));
	int n = 0;
	for (int i = 0; i < N_CHANNELS; i++) {
		if (cSel.value(i)) {
			cI[n].ChannelNumber = i;
			cI[n].Units = cS.value(i);
			cI[n].Type = cT.value(i);
			cI[n].SeriesR = cR.value(i);
			n++;
		}
	}
	return cI;
}


void Global::setAngleAdjust(int v)
{
	settings->setValue("AngleAdjust", v);
	settings->sync();
}


int Global::getAngleAdjust()
{
        int v = (settings->value("AngleAdjust").toInt());
	return v;
}

void Global::setSpeedAdjust(int v)
{
	settings->setValue("SpeedAdjust", v);
	settings->sync();
}


int Global::getSpeedAdjust()
{
	int v = (settings->value("SpeedAdjust").toUInt());
	return v;
}

int Global::getTimeBetweenEvents()
{
	return settings->value("TimeBetweenEvents").toUInt();
}

void Global::setTimeBetweenEvents(int v)
{
	settings->setValue("TimeBetweenEvents", v);
	settings->sync();
}

int Global::getTimeBetweenEventsSD()
{
	return settings->value("TimeBetweenEventsSD").toUInt();
}

void Global::setTimeBetweenEventsSD(int v)
{
	settings->setValue("TimeBetweenEventsSD", v);
	settings->sync();
}


void Global::setRbtSpeed(int v)
{
	settings->setValue("RbtSpeed", v);
	settings->sync();
}


int Global::getRbtSpeed()
{
	int v = (settings->value("RbtSpeed").toUInt());
	return v;
}

void Global::setRbtSpeedSD(int v)
{
	settings->setValue("RbtSpeedSD", v);
	settings->sync();
}

int Global::getRbtSpeedSD()
{
	int v = (settings->value("RbtSpeedSD").toUInt());
	return v;
}

void Global::setMechCycles(int v)
{
	settings->setValue("MechCycles", v);
	settings->sync();
}


int Global::getMechCycles()
{
	int v = (settings->value("MechCycles").toUInt());
	return v;
}

void Global::setMechCyclesSD(int v)
{
	settings->setValue("MechCyclesSD", v);
	settings->sync();
}


int Global::getMechCyclesSD()
{
	int v = (settings->value("MechCyclesSD").toUInt());
	return v;
}

void Global::setMechAmp(int v)
{
	settings->setValue("MechAmp", v);
	settings->sync();
}


int Global::getMechAmp()
{
	int v = (settings->value("MechAmp").toUInt());
	return v;
}

void Global::setMechAmpSD(int v)
{
	settings->setValue("MechAmpSD", v);
	settings->sync();
}


int Global::getMechAmpSD()
{
	int v = (settings->value("MechAmpSD").toUInt());
	return v;
}

void Global::setDataFolder(QString v)
{
	settings->setValue("DataFolder", v);
	settings->sync();
}


QString Global::getDataFolder()
{
	QString v = (settings->value("DataFolder").toString());
	if (v.isEmpty())
	{
		v = "C:/ImpedanceData";
		setDataFolder(v);
	}
	return v;
}

QVector<double> Global::getSeriesR()
{
	QVector<double> rlist(Global::N_CHANNELS);
	for (int i = 0; i < Global::N_CHANNELS; i++)
	{
		QString key = QString("SeriesR%1").arg(i);
		double v = (settings->value(key).toDouble());
		rlist[i] = v;
	}
	return rlist;
}

void Global::setSeriesR(QVector<double> rlist)
{
	for (int i = 0; i < Global::N_CHANNELS; i++)
	{
		QString key = QString("SeriesR%1").arg(i);
		settings->setValue(key, rlist[i]);
	}
	settings->sync();
}

int Global::getEITExpCycles()
{
        return settings->value("EITExpCycles").toUInt();
}

void Global::setEITExpCycles(int v)
{
        settings->setValue("EITExpCycles", v);
        settings->sync();
}

int Global::getEITSamplingFreq()
{
        return settings->value("EITSamplingFreq").toUInt();
}

void Global::setEITSamplingFreq(int v)
{
        settings->setValue("EITSamplingFreq", v);
        settings->sync();
}

int Global::getEITOnTime()
{
        return settings->value("EITOnTime").toUInt();
}

void Global::setEITOnTime(int v)
{
        settings->setValue("EITOnTime", v);
        settings->sync();
}

int Global::getEITOffTime()
{
        return settings->value("EITOffTime").toUInt();
}

void Global::setEITOffTime(int v)
{
        settings->setValue("EITOffTime", v);
        settings->sync();
}

double Global::getEITSeriesR()
{
    return settings->value("EITSeriesR").toDouble();
}

void Global::setEITSeriesR(double v)
{
    settings->setValue("EITSeriesR", v);
    settings->sync();
}


QString Global::toHrMinSec (double T)
{
        QString strT = QString("%1s").arg(T);
        if (T > 60) {
                int h = floor(T/60/60);
                int m = floor((T - h*60*60)/60);
                double s = T - h*60*60 - m*60;
                strT += QString("\n [ %1 hr %2 min %3 secs]").arg(h).arg(m).arg(s);
        }
        return strT;
}

short Global::vMaxToGain(ChannelInformation::DAQ_SCALING r)
{
        switch(r) {
        case ChannelInformation::DAQPM_50mV:
                return BIPPT05VOLTS;
        case ChannelInformation::DAQPM_500mV:
                return BIPPT5VOLTS;
        case ChannelInformation::DAQPM_5V:
                return BIP5VOLTS;
        }
        return BIP10VOLTS;
}

