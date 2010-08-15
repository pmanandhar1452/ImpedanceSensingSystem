/*
 * ImpedanceCalculator.h
 *
 *  Created on: Apr 2, 2010
 *      Author: Prakash Manandhar
 */

#ifndef IMPEDANCECALCULATOR_H_
#define IMPEDANCECALCULATOR_H_

#include <QList>
#include <QVector>
#include <QDateTime>
#include <complex>
#include <cbw.h>

#include "ChannelInformation.h"
#include "Measurement.h"

class ImpedanceCalculator {
public:
	ImpedanceCalculator(QVector<ChannelInformation>);
	// data, length of data, carrier frequency, sampling frequency
	// channel information

	ImpedanceMeasurement
		processData(WORD *data, long N, double fc, double fs);
	virtual ~ImpedanceCalculator();

private:

	long N;
	long L;
	double fc;
	double fs;
	WORD * _data;

	QVector< std::complex<double> > b;  // basis vector set
	std::complex<double> Vc; // carrier transform

	void processChannel(int c, long L, ImpedanceMeasurement * m);
	void calculateBasis();
	void calculateCarrier();

	int carrierChannel;
	QVector<ChannelInformation> cInf;

	double getData(int c, int ti);
	double mean(double *x, int N);
	std::complex<double> sumBasis(double *x, int N);
	void substractMean(double *x, int N);
	void gatherData(double *x, int N, int c);
	double getTemperature(double r);

};

#endif /* IMPEDANCECALCULATOR_H_ */
