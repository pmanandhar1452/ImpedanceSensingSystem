#ifndef MEASUREMENTPROGDLG_H
#define MEASUREMENTPROGDLG_H

#include <QtGui/QDialog>
#include <QCloseEvent>
#include "ui_measurementprogdlg.h"

class MeasurementProgDlg : public QDialog
{
    Q_OBJECT

public:
    MeasurementProgDlg(QWidget *parent = 0);
    ~MeasurementProgDlg();

    void setStatus(QString);
    void setProgress(int);
    void setMaximum(int);
    void setFinished(bool);

signals:
	void cancelled();

protected:
	virtual void closeEvent(QCloseEvent*);

private:
    Ui::MeasurementProgDlgClass ui;
    bool finished;
};

#endif // MEASUREMENTPROGDLG_H
