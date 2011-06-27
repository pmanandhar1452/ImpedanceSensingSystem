/*
 * FrequencyListGenerator.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: Prakash Manandhar
 */
#include <math.h>
#include <QDebug>

#include "FrequencyListGenerator.h"
#include "Global.h"

QList<double> FrequencyListGenerator::getFrequencies
	(double start, double end, int steps, int type)
{
    QList<double> lst;
    if (start == end || steps == 1)
    {
        lst.append(start);
        return lst;
    }
    if (start > end) {
            double temp = start;
            start = end;
            end = temp;
    }
    switch (type) {
    case Global::LINEAR:
            getFrequenciesLinear (start, end, steps, type, &lst);
            return lst;
    case Global::LOG:
            getFrequenciesLog (start, end, steps, type, &lst);
            return lst;
    default:
            getFrequenciesDecade (start, end, steps, type, &lst);
            return lst;
    }
}

void FrequencyListGenerator::getFrequenciesLinear(double start, double end,
		int steps, int type, QList<double> *fLst)
{
	long d = round((end*1000 - start*1000)/(steps));
	long f = start*1000;
	while (fLst->length() < steps + 1)
	{
		fLst->append(f/1000.0f);
		f += d;
	}
}

void FrequencyListGenerator::getFrequenciesLog(double start, double end,
		int steps, int type, QList<double> *fLst)
{
	double d = (log2(end) - log2(start))/(steps);
	double f = log2(start);
	while (fLst->length() < steps + 1)
	{
		fLst->append((round(pow(2, f)*1000))/1000.0f);
		f += d;
	}
}



void FrequencyListGenerator::getFrequenciesDecade(double start, double end,
		int steps, int type, QList<double> *fLst)
{
	long f = round(start*1000);
	long end_l = round(end*1000);
	int n = floor(log10(f));
	if (n < 0) n = 0;
	for (int j = n; pow(10, j) <= end_l; j = j + 1)
	{
		if (j > n) f = 2*floor(pow(10, j));
		long next_j10 = floor(pow(10, j+1));
		while (f <= next_j10 && f <= end_l)
		{
			fLst->append(f/1000.0l);
			f += round(pow(10, j));
		}
	}
}

double FrequencyListGenerator::calculateTotalTime()
{
	Global * g = Global::instance();
	return g->getNumSweep()*getSweepTime();
}

double FrequencyListGenerator::getSweepTime()
{
	Global * g = Global::instance();
	QList<double> fL = getFrequencies(
			g->getFrequencyStart(), g->getFrequencyEnd(),
			g->getFrequencySteps(), g->getFreqSweep());
	int nc = g->getCyclesPerIm();
	double T = 0;
        for (int i = 0; i < fL.size(); i++)
            T += (double)(nc)/(double)(fL.value(i));
	return T;
}


QList<int> FrequencyListGenerator::getSamplingRates()
{
	Global * g = Global::instance();
	QList<int> sL;
	QList<double> fL = getFrequencies();
	int sF = g->getSamplingFactor();
	int sM = g->getSamplingMax();
	for (int i = 0; i < fL.size(); i++)
	{
		int sR = ceil(fL[i]*sF);
		if (sR < 0 || sR > sM) sR = sM;
		sL.append(sR);
	}
	return sL;
}

QList<double> FrequencyListGenerator::getFrequencies()
{
	Global * g = Global::instance();
	QList<double> fL = getFrequencies(
				g->getFrequencyStart(), g->getFrequencyEnd(),
				g->getFrequencySteps(), g->getFreqSweep());
	return fL;
}

double FrequencyListGenerator::getTotalDataSize()
{
	Global * g = Global::instance();
	QList<int>    sL = getSamplingRates();
	QList<double> fL = getFrequencies();
	int nc = g->getCyclesPerIm();
	const int BYTES_PER_SAMPLE = 4;
	double S = 0;
	for (int i = 0; i < fL.size(); i++) {
		double T = nc/fL.value(i);
		S += T*sL[i]*BYTES_PER_SAMPLE;
	}
	QVector<bool> csel = g->getChannelSelect();
	return S*csel.count(true)*g->getNumSweep();
}


