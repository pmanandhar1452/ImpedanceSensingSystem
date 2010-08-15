#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include <complex>
#include <QVector>

class PositionMeasurement {
public:
	long   t; //time in ms
	int    pos; //position
};

class ImpedanceMeasurement {
public:
	ImpedanceMeasurement();
	ImpedanceMeasurement(double fc, double fs, long t, int Nc);
	long t; // time in ms
	double fc; // frequency in Hz
	double fs; // sampling frequency in Hz
	QVector< std::complex<double> > X;// carrier voltage, impedance, temperature (in C) or humidity (RH%)
	QVector<double> r; // residual error after measurement
	long N; // number of samples
};

#endif
