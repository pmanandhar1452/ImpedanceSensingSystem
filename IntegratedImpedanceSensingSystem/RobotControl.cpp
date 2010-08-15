/*
 * RobotControl.cpp
 *
 *  Created on: Oct 15, 2008
 *      Author: PManandhar
 */

#include "RobotControl.h"
#include "LoggerTime.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <QFile>
#include <QDebug>

#include "Global.h"

RobotControl::RobotControl():
		port("COM1"), is_time_synced(false),
		is_recording(false), posTimer(this), moveTimer(this)
{
	connect(&posTimer, SIGNAL(timeout()), this, SLOT(writePos()));
	connect(&moveTimer, SIGNAL(timeout()), this, SLOT(moveTimeout()));
	initPort();
}

class SleeperThread: public QThread {
public:
    SleeperThread(int ms): ms(ms) { }
protected:

    virtual void run () {
        usleep(ms);
    }
private:
    const int ms;
};

void sleepMs (int ms) {
    SleeperThread s(ms);
    s.start();
    s.wait();
}

void RobotControl::initPort ()
{
	port.setBaudRate(BAUD57600);
	port.setFlowControl(FLOW_OFF);
	port.setParity(PAR_NONE);
	port.setDataBits(DATA_8);
	port.setStopBits(STOP_2);
        bool status = port.open(QIODevice::ReadWrite|QIODevice::Unbuffered);
        qDebug() << "Port Open Status: " << status;
        sleepMs(1000);
}

void RobotControl::retrySynchronizeTime()
{
        //port.close();
        //initPort();
	synchronizeTime();
}

void RobotControl::release15 ()
{
	const char * buffer = "m 15\n";
	port.write(buffer, strlen(buffer));
}

void RobotControl::flex15 ()
{
    const char * buffer = "m -15\n";
    port.write(buffer, strlen(buffer));
}

void RobotControl::flex(int angle, int speed)
{
    setSpeed(speed);
    char buffer[100];
    sprintf(buffer,"m %d\n", angle);
    //bool writeStatus =
    port.write(buffer, strlen(buffer));
    //qDebug() << "Flex write status: " << writeStatus;
}

void RobotControl::setSpeed(int s)
{
	char buffer[100];
	sprintf(buffer, "s %d\n", s);
	port.write(buffer, strlen(buffer));
}

void RobotControl::moveSine(int cycles)
{
	char buffer[100];
	sprintf(buffer, "b %d\n", cycles);
	port.write(buffer, strlen(buffer));
}

void RobotControl::setAmplitude(int a)
{
	char buffer[100];
	sprintf(buffer, "a %d\n", a);
	port.write(buffer, strlen(buffer));
}


RobotControl::~RobotControl() {
	stopPosRecording();
	port.close();
}

void RobotControl::setPosReporting(bool v)
{
	char buffer[100];
	sprintf(buffer, "p %d\n", v ? 1: 0);
	port.write(buffer, strlen(buffer));
	port.flush();
}

void RobotControl::synchronizeTime()
{
	char buffer[1000];
        bool writeStatus;
	sprintf(buffer, "t %d\n", LoggerTime::timer());
        writeStatus = port.write(buffer, strlen(buffer));
        port.flush();
        sleepMs(200);
        //qDebug() << "Writing to port: " << buffer << " Stauts: " << writeStatus;
	int count = 0;
        const int C_MAX = 100;
	is_time_synced = false;
	while (!is_time_synced && count < C_MAX)
	{
            sprintf(buffer, "t %d\n", LoggerTime::timer());
            writeStatus = port.write(buffer, strlen(buffer));
            sleepMs(200);
            //qDebug() << "Writing to port: " << buffer << " Stauts: " << writeStatus;
            RobotOutput output = parse_robot_out();
            if (output.type == TIMESTAMP)
            {
                    is_time_synced = true;
                    delta_time = output.param2 - output.param1;
            }
            ++count;
	}
}

RobotControl::RobotOutput RobotControl::parse_robot_out()
{
	const long N = 1000;
	RobotOutput output;
	char buffer[N];
	qint64 n = port.readLine(buffer, N);
	if (n < 3 || (buffer[1] != ':'))
	{
		output.type = INVALID;
	}
	else
	{
		switch(*buffer) {
		case 'p':
			output.type = POSITION;
			break;
		case 't':
			output.type = TIMESTAMP;
			break;
		case 'm':
			output.type = MOVE;
			break;
		case 's':
			output.type = SPEED;
			break;
		}
		scan_params(buffer, &output);
	}
        //qDebug() << output.type << ":" << output.param1 << "," <<
        //                   output.param2;
	return output;
}

void RobotControl::scan_params (char * buffer, RobotOutput * output)
{
	int pos = 2;
	output->param1 = 0;
	bool neg = false;
	if (buffer[pos] == '-') {
		neg = true;
		pos++;
	}
	while(buffer[pos] != ',')
	{
		int v = buffer[pos] - '0';
		if (v < 0 || v > 9) {
			output->type = INVALID;
			return;
		}
		output->param1 = output->param1*10 + v;
		++pos;
	}
	if (neg) output->param1 = -output->param1;
	++pos;
	output->param2 = 0;
	while(buffer[pos] != ';')
	{
		int v = buffer[pos] - '0';
		if (v < 0 || v > 9) {
			output->type = INVALID;
			return;
		}
		output->param2 = output->param2*10 + v;
		++pos;
	}
}

bool RobotControl::isTimeSynced()
{
	return is_time_synced;
}

long RobotControl::getDeltaTime()
{
	return delta_time;
}


void RobotControl::startPosRecording(QList<PositionMeasurement> * pM)
{
    pMmt = pM;
    if (!is_time_synced) return;
    stopPosRecording();
    stopEvents();
    setPosReporting(true);
    is_recording = true;
    posTimer.start(200);
    startEvents();
}

void RobotControl::startEvents()
{
	events = getEventList();
	if (events.size() == 0) return;
	moveIndex = 0;
	moveTimer.setSingleShot(true);
        moveTimer.start(events.value(0).StartTime*1000);
}

void RobotControl::stopPosRecording()
{
	posTimer.stop();
	stopEvents();
	is_recording = false;
	setPosReporting(false);
}

void RobotControl::stopEvents()
{
	moveTimer.stop();
}

void RobotControl::writePos()
{
	if (!is_recording) return;
	int count = 0;
	const int C_MAX = 10;
	while (count < C_MAX)
	{
		RobotOutput output = parse_robot_out();
		if (output.type == POSITION)
		{
			PositionMeasurement m;
			m.t = output.param2 - delta_time;
			m.pos = output.param1;
			pMmt->append(m);
		}
		++count;
	}
}


bool RobotControl::isRecording()
{
	return is_recording;
}

QVector<RoboticArmEvent> RobotControl::getEventList()
{
	Global * g = Global::instance();
	int N = g->getNumEvents();
	QVector<RoboticArmEvent> l(N);
	srand(g->getSeed());
	int t = 0;
	const int dt = g->getTimeBetweenEvents();
	const int dt_sd = g->getTimeBetweenEventsSD();
	const int amean = g->getMechAmp();
	const int a_sd  = g->getMechAmpSD();
	const int sp = g->getRbtSpeed();
	const int sp_sd = g->getRbtSpeedSD();
	const int nc = g->getMechCycles();
	const int nc_sd = g->getMechCyclesSD();
	for (int i = 0; i < N; i++)
	{
		t = t + getUniformRnd(dt, dt_sd);
		l[i].StartTime = t;
		l[i].Amplitude = getUniformRnd(amean, a_sd);
                if (l[i].Amplitude < 1) l[i].Amplitude = 1;
		l[i].Speed = getUniformRnd(sp, sp_sd);
                if (l[i].Speed < 1) l[i].Speed = 1;
                if (l[i].Speed > 100) l[i].Speed = 100;
		l[i].NCycles = getUniformRnd(nc, nc_sd);
                if (l[i].NCycles < 1) l[i].NCycles = 1;
	}
	return l;
}

int RobotControl::getUniformRnd(int mean, int SD)
{
	double range = sqrt(12)*(double)SD/2.0;
	int M = round(mean - range);
	int N = round(mean + range);
	return M + rand() / (RAND_MAX / (N - M + 1) + 1);
}

QString RoboticArmEvent::toQString() {
	QString str = QString("{ %1 s, %2 cycles, %3 steps, %4 sp }")
			.arg(StartTime).arg(NCycles).arg(Amplitude).arg(Speed);
	return str;
}

void RobotControl::moveTimeout()
{
	setSpeed(events.value(moveIndex).Speed);
	setAmplitude(events.value(moveIndex).Amplitude);
	moveSine(events.value(moveIndex).NCycles);
	moveIndex++;
	if (moveIndex >= events.size()) return;
	long deltaT =
			events.value(moveIndex).StartTime -
			events.value(moveIndex-1).StartTime;
        moveTimer.start(deltaT*1000);
}
