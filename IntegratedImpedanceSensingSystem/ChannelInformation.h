/*
 * ChannelInformation.h
 *
 *  Created on: Apr 25, 2010
 *      Author: Prakash Manandhar
 */

#ifndef CHANNELINFORMATION_H_
#define CHANNELINFORMATION_H_

#include <QString>

class ChannelInformation {
public:
	enum DAQ_SCALING { DAQPM_5V = 0, DAQPM_500mV = 1, DAQPM_50mV = 2 }; // data acquisition scaling
        enum CHAN_TYPE   {
            CT_CARRIER = 0, CT_HUMIDITY = 1, CT_TEMPERATURE = 2,
            CT_IMPEDANCE = 3, CT_TEMP_5V = 4, CT_DC_VOLTS = 5, CT_AC_VOLTS = 6 }; // channel type

	int ChannelNumber;
	DAQ_SCALING Units;
	CHAN_TYPE Type;
	double SeriesR;

	QString getScalingText();
	QString getTypeText();
	double getScalingConstant();
};


#endif /* CHANNELINFORMATION_H_ */
