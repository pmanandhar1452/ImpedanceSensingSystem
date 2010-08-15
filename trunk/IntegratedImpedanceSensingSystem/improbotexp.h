#ifndef IMPROBOTEXP_H
#define IMPROBOTEXP_H

#include <QtGui/QDialog>
#include <QResizeEvent>
#include <QCheckBox>
#include "ui_improbotexp.h"
#include "Measurement.h"
#include "RobotControl.h"
#include "Global.h"

class ImpRobotExp : public QDialog
{
    Q_OBJECT

public:
    ImpRobotExp(QWidget *parent = 0);
    ~ImpRobotExp();

    QString getDirPath();
    QList<ImpedanceMeasurement> * getImpMeasurement();

protected:
    virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::ImpRobotExpClass ui;
	RobotControl rbtCtrl;

	Global *global;

	void updateStatus();
	void connectUpdates();

	class EventListModel : public QAbstractTableModel {
	public:
		int rowCount(const QModelIndex &parent) const;
		int columnCount(const QModelIndex &parent) const;
		QVariant data(const QModelIndex &index, int role) const;
		QVariant headerData(int section,
				Qt::Orientation orientation, int role) const;
		void resetEventList(QVector<RoboticArmEvent> l);
	private:
		QVector<RoboticArmEvent> list;
	};

	class FreqListModel : public QAbstractTableModel {
	public:
		int rowCount(const QModelIndex &parent) const;
		int columnCount(const QModelIndex &parent) const;
		QVariant data(const QModelIndex &index, int role) const;
		QVariant headerData(int section,
				Qt::Orientation orientation, int role) const;
		void resetFreqList(QList<double> fL, QList<int> sL);
	private:
		QList<double> fL;
		QList<int>    sL;
	};


	EventListModel elModel;
	FreqListModel  flModel;

	void initRbtWidgets();
	void initFGenWidgets();

	QCheckBox chkChn[Global::N_CHANNELS];
	QComboBox cmbChn[Global::N_CHANNELS];
	QComboBox cmbCType[Global::N_CHANNELS];
	QDoubleSpinBox  spnRs[Global::N_CHANNELS];

	void fillFreqList ();
	void createChannelsUI ();
	bool writeSettings ();
	void writeDataToFiles ();
	void writeMATLABCode();

	QList<PositionMeasurement>  posMsmt;
	QList<ImpedanceMeasurement> impMsmt;

	QString dirPath;

private slots:
	void retrySync();
	void updateExpList();
	void selectDeviceID();
	void onExit();
	void testAgConn();
	void updateFreqUI();
	void updateChannelsUI();
	void selectDataFolder();
	void processMeasurement();
	void moveArm();

};

#endif // IMPROBOTEXP_H
