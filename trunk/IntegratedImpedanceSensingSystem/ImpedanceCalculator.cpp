/*
 * ImpedanceCalculator.cpp
 *
 *  Created on: Apr 2, 2010
 *      Author: Prakash
 */

#include "ImpedanceCalculator.h"
#include <math.h>
#include <QFile>
#include <QDebug>

#include "ChannelInformation.h"
#include "Measurement.h"

ImpedanceCalculator::ImpedanceCalculator (
		QVector<ChannelInformation> cInfo ):cInf(cInfo) { }

ImpedanceCalculator::~ImpedanceCalculator() { }

ImpedanceMeasurement ImpedanceCalculator::processData
	( WORD *data, long N, double fc, double fs )
{
	this->_data = data;
	this->N = N; this->fc = fc; this->fs = fs;
	L = N/cInf.size();
        //qDebug() << "data[1] == " << data[1] << ", L = " << L << ", N = " << N;
	ImpedanceMeasurement m(fc, fs, -1, cInf.size());
        //qDebug() << "Calculating Basis";
	calculateBasis  ();
	calculateCarrier();
	for (int i = 0; i < cInf.size(); i++) processChannel(i, L, &m);
        //qDebug() << "Calculated Impedance";
	return m;
}


void ImpedanceCalculator::processChannel(int c, long L, ImpedanceMeasurement *m)
{
	ChannelInformation cI = cInf.value(c);
	const int Nc = cInf.size();
	double x[L];
	if (cI.Type != ChannelInformation::CT_CARRIER) gatherData(x, L, c);
	std::complex<double> V, I;
    double Vtemp, Itemp, absV, angleV;
	switch(cI.Type) {
	case ChannelInformation::CT_CARRIER:
		m->X[c] = Vc; break;
	case ChannelInformation::CT_IMPEDANCE:
		substractMean(x, L);
		V = sumBasis(x, L);
		absV = abs(V); angleV = arg(V);
                angleV = angleV
                         - 2*M_PI*fc*(c - carrierChannel)/((double)(Nc)*fs);
                         //- 2*M_PI*fc*(c - carrierChannel)/((double)(Nc + 0.05)*fs);
		//qDebug() << "Channel " << c << " [IMP]: " << absV << ", " << angleV;
		V = std::polar(absV, angleV);
		I = (Vc - V)/cI.SeriesR;
		m->X[c] = V/I;
		break;
	case ChannelInformation::CT_TEMPERATURE:
		substractMean(x, L);
		V = sumBasis(x, L);
		I = (Vc - V)/cI.SeriesR;
		m->X[c] = getTemperature(real(V/I));
		break;
        case ChannelInformation::CT_TEMP_5V:
                Vtemp = mean(x, L);
                Itemp = (5.0 - Vtemp)/cI.SeriesR;
                m->X[c] = getTemperature(Vtemp/Itemp);
                break;
	case ChannelInformation::CT_HUMIDITY:
		m->X[c] = 0.03892*mean(x, L)*1000 - 41.98;
		break;
        case ChannelInformation::CT_DC_VOLTS:
                m->X[c] = mean(x, L);
                break;
        case ChannelInformation::CT_AC_VOLTS:
                substractMean(x, L);
                V = sumBasis(x, L);
                absV = abs(V); angleV = arg(V);
                angleV = angleV - 2*M_PI*fc*(c - carrierChannel)/((double)(Nc)*fs);
                //qDebug() << "Channel " << c << " [IMP]: " << absV << ", " << angleV;
                V = std::polar(absV, angleV);
                m->X[c] = V;
                break;
	}
}

void ImpedanceCalculator::calculateBasis()
{
	b = QVector< std::complex<double> >(L);
	double w = 2*M_PI*fc/fs;
	std::complex<double> j(0, 1);
	for (long k = 0; k < L; k++) b[k] = exp(-j*(w*k));
}

void ImpedanceCalculator::calculateCarrier()
{
    for (int c = 0; c < cInf.size(); c++)
    {
        if (cInf.value(c).Type == ChannelInformation::CT_CARRIER)
        {
                carrierChannel = c;
                double x[L];
                gatherData(x, L, c);
//                for (int i = 0; i < L; i++)
//                    qDebug() << "x[<< " << i << "] = " << x[i];
                substractMean(x, L);
                Vc = sumBasis(x, L);
                //qDebug() << "Vc = " << abs(Vc);
                break;
        }
    }
}

double ImpedanceCalculator::getData(int c, int ti)
{
	int vi = _data[cInf.size()*ti + c];
	double v = (vi - pow(2, 15))/pow(2, 15);
	return v*cInf.value(c).getScalingConstant();
}

double ImpedanceCalculator::mean(double *x, int N)
{
	double sum = 0;
	for (int i = 0; i < N; i++) sum += x[i];
	return sum/N;
}

void ImpedanceCalculator::substractMean(double *x, int N)
{
	double m = mean(x, N);
	for (int i = 0; i < N; i++) x[i] -= m;
}

void ImpedanceCalculator::gatherData(double *x, int N, int c)
{
	for (int i = 0; i < N; i++) x[i] = getData(c, i);
}

std::complex<double> ImpedanceCalculator::sumBasis(double *x, int N)
{
	std::complex<double> s(0,0);
	for (int i = 0; i < N; i++) s += x[i]*b[i];
	return (2.0/N)*s;
}


double ImpedanceCalculator::getTemperature(double r)
{
	 int T[] = {-30, -29, -28, -27, -26, -25, -24, -23, -22,
			 -21, -20, -19, -18, -17, -16, -15, -14,
			 -13, -12, -11, -10, -9, -8, -7, -6, -5,
			 -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7,
			 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
			 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
			 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
			 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
			 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
			 59, 60, 61, 62, 63, 64, 65, 66, 67, 68,
			 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80 };
	 long R[] = { 144790, 136664, 129054, 121925, 115243, 109030,
			 103115, 97565, 92354, 87460, 82923, 78581, 74497, 70655,
			 67039, 63591, 60381, 57356, 54503, 51813, 49204, 46767,
			 44467, 42296, 40247, 38279, 36455, 34731, 33100, 31557,
			 30029, 28627, 27299, 26042, 24852, 23773, 22708, 21698,
			 20739, 19829, 18959, 18128, 17338, 16588, 15876, 15207,
			 14569, 13962, 13384, 12834, 12280, 11777, 11297, 10840,
			 10404, 10000, 9600, 9218, 8853, 8506, 8178, 7866, 7568,
			 7283, 7011, 6734, 6484, 6244, 6015, 5796, 5575, 5373,
			 5180, 4995, 4817, 4636, 4473, 4316, 4166, 4021, 3874, 3737,
			 3606, 3481, 3360, 3237, 3126, 3019, 2917, 2819, 2720, 2629, 2542,
			 2458, 2378, 2304, 2229, 2158, 2089, 2022, 1960, 1898, 1839, 1782,
			 1727, 1673, 1622, 1573, 1526, 1480, 1432 };
	 int N = 111;
	 if (r >= R[0  ]) return T[0  ];
	 if (r <= R[N-1]) return T[N-1];
	 double t = 0;
	 //qDebug() << "r = " << r;
	 for (int n = 1; n < N; n++)
	 {
		 if (r < R[n-1] && r >= R[n])
		 {
			 t = T[n-1] + (r - R[n-1])*(T[n]-T[n-1])/(double)(R[n]-R[n-1]);
			 //qDebug() << "t = " << t;
			 break;
		 }
	 }
	 return t;
}

