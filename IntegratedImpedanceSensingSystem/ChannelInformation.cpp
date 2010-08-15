/*
 * ChannelInformation.cpp
 *
 *  Created on: Apr 25, 2010
 *      Author: Prakash Manandhar
 */

#include "ChannelInformation.h"

#include <QString>

double ChannelInformation::getScalingConstant()
{
	switch(Units) {
		case DAQPM_5V    : return 5.0;
		case DAQPM_500mV : return 0.5;
		case DAQPM_50mV  : return 0.05;
	}
	return 0.0;
}

QString ChannelInformation::getScalingText()
{
	switch(Units) {
		case DAQPM_5V    : return "+/- 5V";
		case DAQPM_500mV : return "+/- 500mV";
		case DAQPM_50mV  : return "+/- 50mV";
	}
	return "Unknown Scaling";
}

QString ChannelInformation::getTypeText()
{
	switch(Type) {
	case CT_CARRIER    : return "Carrier";
	case CT_HUMIDITY   : return "Humidity";
	case CT_IMPEDANCE  : return "Impedance";
	case CT_TEMPERATURE: return "Temperature";
        case CT_TEMP_5V    : return "Temp_5V";
        case CT_DC_VOLTS   : return "DC Volts";
        case CT_AC_VOLTS   : return "AC Volts";
	}
	return "Unknown Type";
}
