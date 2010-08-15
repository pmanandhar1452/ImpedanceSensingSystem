/*
 * Measurement.cpp
 *
 *  Created on: Apr 25, 2010
 *      Author: Prakash Manandhar
 */

#include "Measurement.h"

ImpedanceMeasurement::ImpedanceMeasurement
	(double fc, double fs, long t, int Nc):
	 t(t), fc(fc), fs(fs), X(Nc), r(Nc)
{

}

ImpedanceMeasurement::ImpedanceMeasurement()
{
	ImpedanceMeasurement(0, 0, 0, 0);
}
