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
