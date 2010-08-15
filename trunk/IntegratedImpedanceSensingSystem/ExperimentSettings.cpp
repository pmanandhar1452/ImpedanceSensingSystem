/*
 * ExperimentSettings.cpp
 *
 *  Created on: Apr 1, 2010
 *      Author: Chhitiz
 */

#include "ExperimentSettings.h"
#include <QDomElement>
#include <QFile>
#include <QDebug>

const char * ExperimentSettings::DT_FORMAT = "dd.MM.yyyy hh.mm.ss.zzz";

void ExperimentSettings::load(QDomDocument doc)
{
	Channels.clear();
	Frequencies.clear();
	mList.clear();
	QDomElement root = doc.documentElement();
	Amp_mV = root.attribute("Amplitude_mV").toInt();
	CyclesPerIm = root.attribute("CyclesPerIm").toInt();
	NumSweeps = root.attribute("NumSweeps").toInt();
	QDomNodeList freqs = root.elementsByTagName("Frequency");
	for (int i = 0; i < freqs.size(); i++)
	{
		FrequencySweep f;
		QDomElement e = freqs.at(i).toElement();
		f.Index = e.attribute("Index").toInt();
		f.CarrierHz = e.attribute("Freq_Hz").toInt();
		f.SamplingHz = e.attribute("Sampling_Hz").toInt();
		Frequencies.append(f);
	}
	QDomNodeList ch = root.elementsByTagName("Channel");
	for (int i = 0; i < ch.size(); i++)
	{
		ChannelInformation c;
		QDomElement e = ch.at(i).toElement();
		c.ChannelNumber = e.attribute("Index").toInt();
		c.SeriesR = e.attribute("SeriesR_Ohm").toDouble();
		c.Type = (ChannelInformation::CHAN_TYPE)(e.attribute("TypeId").toInt());
		c.Units = (ChannelInformation::DAQ_SCALING)(e.attribute("UnitsId").toInt());
		Channels.append(c);
	}
	loadTInitial();
}

void ExperimentSettings::loadTInitial()
{
	QDomDocument doc ("Experiment");
	QFile file(dirPath + "/S0_F0.xml");
	file.open(QFile::ReadOnly);
	doc.setContent(&file);
	file.close();
	QDomElement root = doc.documentElement();
	QString tstartStr = root.attribute("DateTime");
	tInitial = QDateTime::fromString(tstartStr, ExperimentSettings::DT_FORMAT);
}


QString ExperimentSettings::getSummary()
{
	QString str = QString("Amplitude: %1mV, Cycles Per Measurement: %2, Number of Sweeps: %3\n")
			.arg(Amp_mV).arg(CyclesPerIm).arg(NumSweeps);
	str += QString("\nFrequencies(Carrier, Sampling): [%1 per sweep]\n{ ")
			.arg(Frequencies.size());
	for (int i = 0; i < Frequencies.size(); i++)
	{

		str += QString("(%1, %2) ").arg(Frequencies[i].CarrierHz)
				.arg(Frequencies[i].SamplingHz);
	}
	str += "}\n";
	str += QString("\nChannels: [%1]\n{ ").arg(Channels.size());
	for (int i = 0; i < Channels.size(); i++)
	{
		ChannelInformation c = Channels[i];
		str += QString("(%1: Rs = %2 [%3 %4]) ").arg(c.ChannelNumber)
			.arg(c.SeriesR).arg(c.getTypeText()).arg(c.getScalingText());
	}
	str += "}\n";
	return str;
}


void ExperimentSettings::writeResultsToFile()
{
	QFile ft(dirPath + "/t.dat");
	QFile ftC(dirPath + "/tC.dat"); // temperature dCelcius
	QFile fHu(dirPath + "/Hu.dat"); // Humidity RH%
	QFile fcfs(dirPath + "/FcFs.dat"); // fc and fs
	QFile * fCh[Channels.size()];
	for (int c = 0; c < Channels.size(); c++)
	{
		fCh[c] = new QFile(dirPath + QString("/CH%1.dat").arg(c));
		if (Channels[c].Type == ChannelInformation::CT_IMPEDANCE)
			fCh[c]->open(QFile::WriteOnly);
	}
	ft. open(QFile::WriteOnly);
	ftC.open(QFile::WriteOnly);
	fHu.open(QFile::WriteOnly);
	fcfs.open(QFile::WriteOnly);
	for (int i = 0; i < mList.size(); i++) {
		ft.write((char *)&(mList[i].t), sizeof(double));
		fcfs.write((char *)&(mList[i].fc), sizeof(double));
		fcfs.write((char *)&(mList[i].fs), sizeof(double));
		for (int c = 0; c < Channels.size(); c++)
		{
			double rl, im;
			rl = real(mList[i].X[c]);
			im = imag(mList[i].X[c]);
			switch(Channels[c].Type) {
			case ChannelInformation::CT_TEMPERATURE:
                        case ChannelInformation::CT_TEMP_5V:
				ftC.write((char *)&rl, sizeof(double));
				break;
                        case ChannelInformation::CT_HUMIDITY:
                               // qDebug() << "Channel " << c << " is humidity";
				fHu.write((char *)&rl, sizeof(double));
				break;
			case ChannelInformation::CT_IMPEDANCE:
                                //qDebug() << "Channel " << c << " is impedance";
				fCh[c]->write((char *)&rl, sizeof(double));
				fCh[c]->write((char *)&im, sizeof(double));
				break;
			case ChannelInformation::CT_CARRIER:
                        case ChannelInformation::CT_DC_VOLTS:
                        case ChannelInformation::CT_AC_VOLTS:
				continue;
			}
		}
	}
	ft.close(); ftC.close(); fHu.close(); fcfs.close();
	for (int c = 0; c < Channels.size(); c++)
	{
		fCh[c]->close();
		delete fCh[c];
	}
}

void ExperimentSettings::addMeasurement(ImpedanceMeasurement m)
{
	mutex.lock();
	mList.append(m);
	mutex.unlock();
}
