#include "measurementprogdlg.h"

MeasurementProgDlg::MeasurementProgDlg(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.btnCancel, SIGNAL(clicked()), this, SIGNAL(cancelled()));
}

MeasurementProgDlg::~MeasurementProgDlg() {

}

void MeasurementProgDlg::setStatus(QString str)
{
	ui.lblStatus->setText(str);
}

void MeasurementProgDlg::setProgress(int p) {
	ui.progressBar->setValue(p);
}

void MeasurementProgDlg::setMaximum(int m) {
	ui.progressBar->setMaximum(m);
}

void MeasurementProgDlg::setFinished(bool f) {
	finished = f;
}

void MeasurementProgDlg::closeEvent(QCloseEvent *e)
{
	if (finished) e->accept();
	else {
		emit cancelled();
		e->ignore();
	}
}
