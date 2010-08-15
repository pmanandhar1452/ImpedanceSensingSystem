#include "integratedimpedancesensingsystem.h"
#include "ImpRobotExp.h"
#include "eit8electrodedlg.h"
#include <QDebug>

IntegratedImpedanceSensingSystem::IntegratedImpedanceSensingSystem(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.actImpRobotExp, SIGNAL(triggered()), this, SLOT(newImpRobotExp()));
        connect(ui.actionEIT_8_Electrodes, SIGNAL(triggered()), this, SLOT(newEIT8ElecExp()));

}

IntegratedImpedanceSensingSystem::~IntegratedImpedanceSensingSystem()
{

}

void IntegratedImpedanceSensingSystem::newImpRobotExp()
{
	ImpRobotExp e;
	e.exec();
}

void IntegratedImpedanceSensingSystem::newEIT8ElecExp()
{
        EIT8ElectrodeDlg e;
        e.exec();
}
