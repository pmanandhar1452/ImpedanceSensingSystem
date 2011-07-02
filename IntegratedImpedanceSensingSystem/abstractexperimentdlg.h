#ifndef ABSTRACTEXPERIMENTDLG_H
#define ABSTRACTEXPERIMENTDLG_H

#include <QDialog>
#include <QList>
#include "Measurement.h"

class AbstractExperimentDlg : public QDialog
{
    Q_OBJECT
public:
    explicit AbstractExperimentDlg(QWidget *parent = 0);
    virtual QList<ImpedanceMeasurement> * getImpMeasurement();

protected:
    QList<ImpedanceMeasurement> impMsmt;

signals:

public slots:

};

#endif // ABSTRACTEXPERIMENTDLG_H
