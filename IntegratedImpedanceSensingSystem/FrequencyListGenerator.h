/*
 * FrequencyListGenerator.h
 *
 *  Created on: Mar 24, 2010
 *      Author: Prakash Manandhar
 */

#ifndef FREQUENCYLISTGENERATOR_H_
#define FREQUENCYLISTGENERATOR_H_

#include <QString>
#include <QList>

class FrequencyListGenerator {

public:
	static QList<double> getFrequencies  (double start, double end, int steps, int type);
	static QList<double> getFrequencies  ();
	static QList<int> getSamplingRates();
	static double getTotalDataSize();   // data size in bytes
	static double calculateTotalTime(); // returns total time in seconds
	static double getSweepTime(); // returns total time in seconds

private:
	static void getFrequenciesLinear(double start, double end,
				int steps, int type, QList<double>*);
	static void getFrequenciesLog(double start, double end,
				int steps, int type, QList<double>*);
	static void getFrequenciesDecade(double start, double end,
			int steps, int type, QList<double>*);


};

#endif /* FREQUENCYLISTGENERATOR_H_ */
