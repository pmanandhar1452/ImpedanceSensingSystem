/*
 * RobotControl.h
 *
 *  Created on: Oct 15, 2008
 *      Author: PManandhar
 */

#ifndef ROBOTCONTROL_H_
#define ROBOTCONTROL_H_

#include <QExtSerialPort.h>
#include <QTimer>
#include <QObject>
#include <QFile>
#include <QVector>

#include "Measurement.h"

/**
 * An experimental event of robotic arm flex
 */
class RoboticArmEvent {
public:
	int StartTime; // secs
	int NCycles;
	int Speed;     // percent of max
	int Amplitude; // motor steps

	QString toQString();
};

class RobotControl: public QObject {
	Q_OBJECT
public:
	RobotControl();
	~RobotControl();

	void release15 ();
	void flex15 ();
	void moveSine (int cycles);
	void flex(int angle, int speed);

	void setSpeed (int s);
	void setAmplitude (int a);

	void startPosRecording(QList<PositionMeasurement>*);
	void stopPosRecording();
	bool isRecording();

	void setPosReporting (bool);

	bool isTimeSynced();
	void synchronizeTime ();
	void retrySynchronizeTime(); // same as synchronizeTime, but also closes and re-initializes port
	long getDeltaTime();

	QVector<RoboticArmEvent> getEventList();

	enum CommandType { INVALID, TIMESTAMP, POSITION, MOVE, SPEED };

	struct RobotOutput {
		CommandType type;
		long param1;
		long param2;
	};

public slots:
	void writePos ();
	void moveTimeout ();

private:
	QextSerialPort port;
	long int delta_time;
	bool is_time_synced;
	bool is_recording;

	QTimer posTimer;

	int moveIndex;
	QTimer moveTimer;

	RobotOutput parse_robot_out ();
	void scan_params (char * bufffer, RobotOutput * output);
	void initPort();

	int getUniformRnd(int mean, int SD);

	QList<PositionMeasurement> * pMmt;

	void startEvents();
	void stopEvents();
	QVector<RoboticArmEvent> events;


};

#endif /* ROBOTCONTROL_H_ */
