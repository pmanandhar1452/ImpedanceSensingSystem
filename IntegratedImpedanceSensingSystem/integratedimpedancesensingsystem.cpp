#include "integratedimpedancesensingsystem.h"
#include "ImpRobotExp.h"
#include "eit8electrodedlg.h"
#include "singlefreqimpdlg.h"
#include <QDebug>

IntegratedImpedanceSensingSystem::IntegratedImpedanceSensingSystem(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.actImpRobotExp, SIGNAL(triggered()), this, SLOT(newImpRobotExp()));
        connect(ui.actionEIT_8_Electrodes, SIGNAL(triggered()), this, SLOT(newEIT8ElecExp()));
        connect(ui.actionContSingFreqImp, SIGNAL(triggered()), this, SLOT(newContSingFreqImpExp()));

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

void IntegratedImpedanceSensingSystem::newContSingFreqImpExp()
{
        SingleFreqImpDlg e;
        e.exec();
}

