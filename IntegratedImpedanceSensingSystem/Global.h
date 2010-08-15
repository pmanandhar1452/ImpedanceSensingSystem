/*
 * Global.h
 *
 *  Created on: Mar 24, 2010
 *      Author: Prakash Manandhar
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <QString>
#include <QSettings>

#include "ChannelInformation.h"

class Global: public QObject {
	Q_OBJECT;
public:
	static const int N_CHANNELS = 16;

	// singleton
	static Global * instance () {
		if(pI == NULL)
			pI = new Global();
		return pI;
	};

	static void destroy () {
		if (pI != NULL)
			delete pI;
		pI = NULL;
	}

        static QString toHrMinSec(double T); // convert from seconds to hour minute second
        static short vMaxToGain(ChannelInformation::DAQ_SCALING);

        QString getA33220ADeviceID ();
        void setA33220ADeviceID (QString id);

	double getAmplitude      (); // amplitude in mV
	double getFrequencyStart (); // frequency in mHz
	double getFrequencyEnd   (); // frequency in mHz

	// No. of frequency steps... Use FrequencyListGenerator::getFrequencies instead
	// of this to get actual number of frequencies
	int    getFrequencySteps ();

	int    getCyclesPerIm    (); // Cycles per impedance measurement
	int    getNumSweep       (); // Number of sweeps
	int    getSamplingFactor (); // Sampling rate as multiple of frequency
	int    getSamplingMax    (); // Sampling rate maximum
	QString getDataFolder    ();

	// related to robotic arm experiment
	int getSeed(); // seed for random number generator
	int getNumEvents();
	int getAngleAdjust();
	int getSpeedAdjust();
	int getTimeBetweenEvents();
	int getTimeBetweenEventsSD();
	int getRbtSpeed();
	int getRbtSpeedSD();
	int getMechCycles();
	int getMechCyclesSD();
	int getMechAmp();
	int getMechAmpSD();

	enum FREQ_SWEEP {LINEAR = 0, LOG = 1, DECADE = 2}; // type of frequency sweep

	int getFreqSweep();

	QVector<bool> getChannelSelect ();    // whether the given channel is to be sampled
	void setChannelSelect(QVector<bool>);

	QVector<ChannelInformation::DAQ_SCALING> getChannelScaling ();    // whether the given channel is to be sampled
	void setChannelScaling(QVector<ChannelInformation::DAQ_SCALING>);

	QVector<ChannelInformation::CHAN_TYPE> getChannelType ();    // what kind of data are we processing?
	void setChannelType(QVector<ChannelInformation::CHAN_TYPE>);

	QVector<double> getSeriesR ();     // series resistance
	void setSeriesR (QVector<double>);

	QVector<ChannelInformation> getChannelInformation ();

        // EIT experiment
        int getEITExpCycles ();    // number of cycles of stimulation
        int getEITSamplingFreq (); // sampling frequency in Hz
        int getEITOnTime ();       // on time of a stimulation pattern in ms
        int getEITOffTime ();      // time between stimulation patterns in ms
        double getEITSeriesR ();   // source circuit series resistor in ohms


        QString getU2751ADeviceID ();         // Switch matrix device IDs
        void setU2751ADeviceID (QString id);


public slots:
	void setAmplitude (double);
	void setFrequencyStart (double); // frequency in mHz
	void setFrequencyEnd (double); // frequency in mHz
	void setFrequencySteps (int);
	void setFreqSweep(int);
	void setCyclesPerIm(int);
	void setNumSweep(int);
	void setSamplingFactor(int);
	void setSamplingMax   (int);
	void setDataFolder (QString);

	// Related to robotic experiment
	void setSeed(int);
	void setNumEvents (int);
	void setAngleAdjust(int);
	void setSpeedAdjust(int);
	void setTimeBetweenEvents(int);
	void setTimeBetweenEventsSD(int);
	void setRbtSpeed(int);
	void setRbtSpeedSD(int);
	void setMechCycles(int);
	void setMechCyclesSD(int);
	void setMechAmp(int);
	void setMechAmpSD(int);

        // EIT experiment
        void setEITExpCycles (int);
        void setEITSamplingFreq (int);
        void setEITOnTime (int);
        void setEITOffTime (int);
        void setEITSeriesR (double);

private:
	Global();
	virtual ~Global();
	static Global * pI;
	QSettings * settings;
};

#endif /* GLOBAL_H_ */
