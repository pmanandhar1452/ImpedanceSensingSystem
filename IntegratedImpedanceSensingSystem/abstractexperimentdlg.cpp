#include "abstractexperimentdlg.h"

AbstractExperimentDlg::AbstractExperimentDlg(QWidget *parent) :
    QDialog(parent)
{
}

QList<ImpedanceMeasurement> * AbstractExperimentDlg::getImpMeasurement()
{
        return &impMsmt;
}

