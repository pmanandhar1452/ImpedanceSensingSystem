/*
 * Created on: Aug 11, 2010
 *      Author: Prakash Manandhar
 */

#include "integratedimpedancesensingsystem.h"

#include <QtGui>
#include <QApplication>
#include "LoggerTime.h"

int main(int argc, char *argv[])
{
    LoggerTime::reset();
    QApplication a(argc, argv);
    IntegratedImpedanceSensingSystem w;
    w.show();
    return a.exec();
}
