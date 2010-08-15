#ifndef INTEGRATEDIMPEDANCESENSINGSYSTEM_H
#define INTEGRATEDIMPEDANCESENSINGSYSTEM_H

#include <QtGui/QMainWindow>
#include "ui_integratedimpedancesensingsystem.h"

class IntegratedImpedanceSensingSystem : public QMainWindow
{
    Q_OBJECT

public:
    IntegratedImpedanceSensingSystem(QWidget *parent = 0);
    ~IntegratedImpedanceSensingSystem();

private:
    Ui::IntegratedImpedanceSensingSystemClass ui;

private slots:
    void newImpRobotExp();
    void newEIT8ElecExp();
};

#endif // INTEGRATEDIMPEDANCESENSINGSYSTEM_H
