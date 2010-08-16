#include "improbotexp.h"

#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QTime>
#include <QFileDialog>
#include <QDateTime>
#include <QDir>
#include <QDomDocument>
#include <math.h>
#include <visa.h>

#include "FrequencyListGenerator.h"
#include "MeasurementLoop.h"

ImpRobotExp::ImpRobotExp(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	global = Global::instance();
	connect(this, SIGNAL(finished(int)), this, SLOT(onExit()));

	rbtCtrl.synchronizeTime();
	updateStatus();
	initRbtWidgets();
	initFGenWidgets();

	ui.verticalLayout->setMargin(10);
	setLayout(ui.verticalLayout);
}


void ImpRobotExp::initFGenWidgets()
{

        QString strDevID = global->getA33220ADeviceID();
	if (strDevID != "")
		ui.lblDevID->setText(strDevID);
	else
		qDebug() << "Device ID was not set previously";

	ui.txtDataFolder->setText(global->getDataFolder());

	ui.spnAmp->setValue(global->getAmplitude());
	ui.spnSamplingFactor->setValue(global->getSamplingFactor());
	ui.spnSamplingMax->setValue(global->getSamplingMax());
	ui.spnFreqStart->setValue(global->getFrequencyStart());
	ui.spnFreqEnd->setValue(global->getFrequencyEnd());
	ui.spnFreqSteps->setValue(global->getFrequencySteps());
	ui.spnCyclesPerIm->setValue(global->getCyclesPerIm());
	ui.spnNumSweep->setValue(global->getNumSweep());

	ui.cmbFreqSweep->addItem("Linear", Global::LINEAR);
	ui.cmbFreqSweep->addItem("Log", Global::LOG);
	ui.cmbFreqSweep->addItem("Decade", Global::DECADE);
	ui.cmbFreqSweep->setCurrentIndex(global->getFreqSweep());
	connect(ui.cmbFreqSweep, SIGNAL(currentIndexChanged(int)),
					global, SLOT(setFreqSweep(int)));

	connect(ui.cmbFreqSweep, SIGNAL(currentIndexChanged(int)),
						this, SLOT(updateFreqUI()));

	connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(processMeasurement()));
	connect(ui.btnDevID, SIGNAL(clicked()), this, SLOT(selectDeviceID()));
	connect(ui.btnTestAgConn, SIGNAL(clicked()), this, SLOT(testAgConn()));
	connect(ui.btnSelDataFolder, SIGNAL(clicked()), this, SLOT(selectDataFolder()));
	connect(ui.txtDataFolder, SIGNAL(textChanged(QString)), global, SLOT(setDataFolder(QString)));

	connect(ui.spnAmp, SIGNAL(valueChanged(double)), global, SLOT(setAmplitude(double)));
	connect(ui.spnSamplingFactor, SIGNAL(valueChanged(int)), global, SLOT(setSamplingFactor(int)));
	connect(ui.spnSamplingMax, SIGNAL(valueChanged(int)), global, SLOT(setSamplingMax(int)));

	connect(ui.spnFreqStart, SIGNAL(valueChanged(double)), global, SLOT(setFrequencyStart(double)));
	connect(ui.spnFreqEnd, SIGNAL(valueChanged(double)), global, SLOT(setFrequencyEnd(double)));
	connect(ui.spnFreqSteps, SIGNAL(valueChanged(int)), global, SLOT(setFrequencySteps(int)));
	connect(ui.spnCyclesPerIm, SIGNAL(valueChanged(int)), global, SLOT(setCyclesPerIm(int)));
	connect(ui.spnNumSweep, SIGNAL(valueChanged(int)), global, SLOT(setNumSweep(int)));


	connect(ui.spnFreqStart, SIGNAL(valueChanged(double)), this, SLOT(updateFreqUI()));
	connect(ui.spnFreqEnd, SIGNAL(valueChanged(double)), this, SLOT(updateFreqUI()));
	connect(ui.spnFreqSteps, SIGNAL(valueChanged(int)), this, SLOT(updateFreqUI()));
	connect(ui.spnCyclesPerIm, SIGNAL(valueChanged(int)), this, SLOT(updateFreqUI()));
	connect(ui.spnNumSweep, SIGNAL(valueChanged(int)), this, SLOT(updateFreqUI()));
	connect(ui.spnSamplingFactor, SIGNAL(valueChanged(int)), this, SLOT(updateFreqUI()));
	connect(ui.spnSamplingMax, SIGNAL(valueChanged(int)), this, SLOT(updateFreqUI()));

	ui.tvFreqList->setModel(&flModel);

	updateFreqUI ();
	createChannelsUI();
}

void ImpRobotExp::initRbtWidgets()
{
	ui.spnSeed->setValue(global->getSeed());
	ui.spnNEvents->setValue(global->getNumEvents());
	ui.spnAngleAdjust->setValue(global->getAngleAdjust());
	ui.spnSpeedAdjust->setValue(global->getSpeedAdjust());
	ui.spnTime->setValue(global->getTimeBetweenEvents());
	ui.spnTimeSD->setValue(global->getTimeBetweenEventsSD());
	ui.spnSpeed->setValue(global->getRbtSpeed());
	ui.spnSpeedSD->setValue(global->getRbtSpeedSD());
	ui.spnNCycles->setValue(global->getMechCycles());
	ui.spnNCyclesSD->setValue(global->getMechCyclesSD());
	ui.spnMAmp->setValue(global->getMechAmp());
	ui.spnMAmpSD->setValue(global->getMechAmpSD());

	connect(ui.spnSeed, SIGNAL(valueChanged(int)), global, SLOT(setSeed(int)));
	connect(ui.spnNEvents, SIGNAL(valueChanged(int)), global, SLOT(setNumEvents(int)));
	connect(ui.spnAngleAdjust, SIGNAL(valueChanged(int)), global, SLOT(setAngleAdjust(int)));
	connect(ui.spnSpeedAdjust, SIGNAL(valueChanged(int)), global, SLOT(setSpeedAdjust(int)));
	connect(ui.spnTime, SIGNAL(valueChanged(int)), global, SLOT(setTimeBetweenEvents(int)));
	connect(ui.spnTimeSD, SIGNAL(valueChanged(int)), global, SLOT(setTimeBetweenEventsSD(int)));
	connect(ui.spnSpeed, SIGNAL(valueChanged(int)), global, SLOT(setRbtSpeed(int)));
	connect(ui.spnSpeedSD, SIGNAL(valueChanged(int)), global, SLOT(setRbtSpeedSD(int)));
	connect(ui.spnNCycles, SIGNAL(valueChanged(int)), global, SLOT(setMechCycles(int)));
	connect(ui.spnNCyclesSD, SIGNAL(valueChanged(int)), global, SLOT(setMechCyclesSD(int)));
	connect(ui.spnMAmp, SIGNAL(valueChanged(int)), global, SLOT(setMechAmp(int)));
	connect(ui.spnMAmpSD, SIGNAL(valueChanged(int)), global, SLOT(setMechAmpSD(int)));

	connect(ui.btnChkRbtConn, SIGNAL(clicked()), this, SLOT(retrySync()));

	connect(ui.btnMoveArm, SIGNAL(clicked()), this, SLOT(moveArm()));

	ui.tvEvents->setModel(&elModel);

	updateExpList();
	connectUpdates();

}

void ImpRobotExp::connectUpdates()
{
	connect(ui.spnSeed, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnNEvents, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnAngleAdjust, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnSpeedAdjust, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnTime, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnTimeSD, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnSpeed, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnSpeedSD, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnNCycles, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnNCyclesSD, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnMAmp, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
	connect(ui.spnMAmpSD, SIGNAL(valueChanged(int)), this, SLOT(updateExpList()));
}

void ImpRobotExp::updateExpList()
{
	QVector<RoboticArmEvent> l = rbtCtrl.getEventList();
	elModel.resetEventList(l);
}

void ImpRobotExp::retrySync()
{
	rbtCtrl.retrySynchronizeTime();
	updateStatus();
}

void ImpRobotExp::updateStatus()
{
	QString syncText;
	if (rbtCtrl.isTimeSynced())
		syncText = QString(
                        "<font color=green>Synced, D-Time: %1 ms</font>")
					.arg(rbtCtrl.getDeltaTime());
	else
		syncText = QString("<font color=red>Time Sync Failed</font>");
	ui.lblSync->setText(syncText);
}

void ImpRobotExp::moveArm() {
	rbtCtrl.flex(ui.spnAngleAdjust->value(), ui.spnSpeedAdjust->value());
}

QString ImpRobotExp::getDirPath() {
	return dirPath;
}

ImpRobotExp::~ImpRobotExp()
{

}

void ImpRobotExp::resizeEvent(QResizeEvent * event)
{
	//QRect r(QPoint(0,0), event->size());
	//ui.verticalLayout->setGeometry(r);
}

int ImpRobotExp::EventListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
	return list.size();
}

int ImpRobotExp::EventListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
	return 4;
}

QVariant ImpRobotExp::EventListModel::
			data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (index.row() >= list.size() || index.row() < 0)
         return QVariant();

     if (role == Qt::DisplayRole) {
    	 int r = index.row();
    	 switch(index.column()) {
    	 case 0: return list[r].StartTime;
    	 case 1: return list[r].NCycles;
    	 case 2: return list[r].Amplitude;
    	 case 3: return list[r].Speed;
    	 }
     }
     return QVariant();
 }

 QVariant ImpRobotExp::EventListModel::
	 headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role != Qt::DisplayRole)
         return QVariant();

     if (orientation == Qt::Horizontal) {
         switch (section) {
             case 0: return tr("Time[s]");
             case 1: return tr("Num C.");
             case 2: return tr("Amp");
             case 3: return tr("Speed");
             default:
                 return QVariant();
         }
     }
     else if (orientation == Qt::Vertical) {
    	return section+1;
     }
     return QVariant();
}

void ImpRobotExp::EventListModel::resetEventList(QVector<RoboticArmEvent> l)
{
	list = l;
	emit layoutChanged();
}

void ImpRobotExp::createChannelsUI ()
{
	QGridLayout *vbox = new QGridLayout(ui.channelsGroup);
	Global * g = Global::instance();
	QVector<bool> csel = g->getChannelSelect();
	QVector<ChannelInformation::DAQ_SCALING> cres = g->getChannelScaling();
	QVector<ChannelInformation::CHAN_TYPE> ctype = g->getChannelType();
	QVector<double> rs = g->getSeriesR();

	for (int i = 0; i < Global::N_CHANNELS; i++) {
		int row = i%8; int col = (i/8)*7;
		if (csel[i]) chkChn[i].setCheckState(Qt::Checked);
		else chkChn[i].setCheckState(Qt::Unchecked);
		connect(&chkChn[i], SIGNAL(stateChanged(int)), this, SLOT(updateChannelsUI()));
		vbox->addWidget(new QLabel(QString("%1").arg(i)), row, col + 0);
		vbox->addWidget(&chkChn[i], row, col + 1);

		cmbChn[i].addItem("+/- 5V"   , ChannelInformation::DAQPM_5V);
		cmbChn[i].addItem("+/- 500mV", ChannelInformation::DAQPM_500mV);
		cmbChn[i].addItem("+/- 50mV" , ChannelInformation::DAQPM_50mV);
		cmbChn[i].setCurrentIndex(cres[i]);
		connect(&cmbChn[i], SIGNAL(currentIndexChanged(int)), this, SLOT(updateChannelsUI()));
		vbox->addWidget(&cmbChn[i], row, col + 2);

		cmbCType[i].addItem("Carrier", ChannelInformation::CT_CARRIER);
		cmbCType[i].addItem("Humidity", ChannelInformation::CT_HUMIDITY);
		cmbCType[i].addItem("Temperature", ChannelInformation::CT_TEMPERATURE);
		cmbCType[i].addItem("Impedance", ChannelInformation::CT_IMPEDANCE);
                cmbCType[i].addItem("Temp_5V", ChannelInformation::CT_TEMP_5V);
		cmbCType[i].setCurrentIndex(ctype[i]);
		connect(&cmbCType[i], SIGNAL(currentIndexChanged(int)), this, SLOT(updateChannelsUI()));
		vbox->addWidget(&cmbCType[i], row, col + 3);

		spnRs[i].setMaximum(1e9);
		spnRs[i].setDecimals(5);
		spnRs[i].setValue(rs[i]);
		connect(&spnRs[i], SIGNAL(valueChanged(double)), this, SLOT(updateChannelsUI()));
		vbox->addWidget(&spnRs[i], row, col + 4);
	}
	QFrame * vline = new QFrame(ui.channelsGroup);
	vline->setFrameStyle(QFrame::VLine);
	vbox->addWidget(vline, 0, 5, 8, 1);
	ui.channelsGroup->setLayout(vbox);
}

void ImpRobotExp::updateChannelsUI()
{
	Global * g = Global::instance();
	QVector<bool> csel(Global::N_CHANNELS);
	QVector<ChannelInformation::DAQ_SCALING> cres(Global::N_CHANNELS);
	QVector<ChannelInformation::CHAN_TYPE> ctype(Global::N_CHANNELS);
	QVector<double> rs(Global::N_CHANNELS);
	for (int i = 0; i < Global::N_CHANNELS; i++)
	{
		if (chkChn[i].checkState() == Qt::Checked) csel[i] = true;
		else csel[i] = false;
		cres[i] = (ChannelInformation::DAQ_SCALING)(cmbChn[i].currentIndex());
		ctype[i] = (ChannelInformation::CHAN_TYPE)(cmbCType[i].currentIndex());
		rs[i] = spnRs[i].value();
	}
	g->setChannelSelect(csel);
	g->setChannelScaling(cres);
	g->setChannelType(ctype);
	g->setSeriesR(rs);
	updateFreqUI();
}

QList<ImpedanceMeasurement> * ImpRobotExp::getImpMeasurement()
{
	return &impMsmt;
}

void ImpRobotExp::updateFreqUI()
{
	fillFreqList ();
	double T = FrequencyListGenerator::calculateTotalTime();
	double S = FrequencyListGenerator::getTotalDataSize();
	double swpT = FrequencyListGenerator::getSweepTime();
        QString strT = QString("Total time: ") + Global::toHrMinSec(T);
	QString sU = "bytes";
	if (S > 1024) {
		S = S/1024; sU = "KBytes";
		if (S > 1024) {
			S = S/1024; sU = "MBytes";
			if (S > 1024) {
				S = S/1024; sU = "GBytes";
			}
		}
	}
	strT += QString("\nTotal Data Size: %1 ").arg(S) + sU;
	ui.lblTotalTime->setText(strT);
	ui.lblSweepTime->setText(QString("Sweep time: ")
                        + Global::toHrMinSec(swpT));
}

void ImpRobotExp::fillFreqList ()
{
	QList<double> fL = FrequencyListGenerator::getFrequencies();
	QList<int>    sL = FrequencyListGenerator::getSamplingRates();
	flModel.resetFreqList(fL, sL);
}

void ImpRobotExp::testAgConn()
{
	ViSession defaultRM, vi;
	char buf [1000] = {0};

	/* Open session to GPIB device at address 22 */
	viOpenDefaultRM (&defaultRM);
        viOpen (defaultRM, global->getA33220ADeviceID().toAscii().data(), VI_NULL,VI_NULL, &vi);

	/* Initialize device */
	viPrintf (vi, (ViString)"*RST\n"); //@SuppressWarnings("deprecation")

	/* Send an *IDN? string to the device */
	viPrintf (vi, (ViString)"*IDN?\n");

	/* Read results */
	viScanf (vi, (ViString)"%t", &buf);

	if (strlen(buf) == 0) sprintf(buf, "STATUS: INSTRUMENT NOT CONNECTED!!!");

	/* Print results */
	printf ("Instrument identification string: %s\n", buf);
	fflush(stdout);

	/* Close session */
	viClose (vi);
	viClose (defaultRM);

	QMessageBox::information(this, "Agilent Instrument ID", buf);

}

void ImpRobotExp::onExit()
{
	//qDebug() << "Exiting application!!!";
	Global::destroy();
}

void ImpRobotExp::selectDeviceID()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Enter Agilent 33220 VISA Device ID"),
	                                          tr("VISA Device ID:"), QLineEdit::Normal,
                                                  global->getA33220ADeviceID(), &ok);
	if (ok && !text.isEmpty()) {
		ui.lblDevID->setText(text);
                global->setA33220ADeviceID(text);
	}
}

void ImpRobotExp::selectDataFolder()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Select Data Folder",
			ui.txtDataFolder->text(), QFileDialog::ShowDirsOnly);
	if (!dir.isEmpty())
	{
		ui.txtDataFolder->setText(dir);
	}
}

void ImpRobotExp::processMeasurement()
{
    posMsmt.clear(); impMsmt.clear();
    QDateTime qdt = QDateTime::currentDateTime();
    dirPath = ui.txtDataFolder->text()
                    + "/" + qdt.toString("dd.MM.yyyy hh.mm.ss.zzz");
    QDir dir;
    if (!dir.mkdir(dirPath)) {
            QMessageBox::information(this,
                            "Impedance Measurement DAQ",
                            "Error while creating output directory!");
            return;
    }
    rbtCtrl.startPosRecording(&posMsmt);
    MeasurementLoop ml(this, &impMsmt);
    ml.measure();
    rbtCtrl.stopPosRecording();
    writeDataToFiles();
}

bool ImpRobotExp::writeSettings()
{
	QDomDocument qDom;
	QFile xmlF(dirPath + "/ExpSettings.xml");
	if (!xmlF.open(QIODevice::ReadWrite)) return false;
	QTextStream stream(&xmlF);
	QDomElement root = qDom.createElement("Experiment");
	root.setAttribute("NumSweeps", global->getNumSweep());
	root.setAttribute("Amplitude_mV", global->getAmplitude());
	root.setAttribute("CyclesPerIm", global->getCyclesPerIm());
	qDom.appendChild(root);

	QList<double> fList = FrequencyListGenerator::getFrequencies();
	QList<int> sRList = FrequencyListGenerator::getSamplingRates();
	QDomElement freq = qDom.createElement("SweepForward");
	root.appendChild(freq);
	freq.setAttribute("NumPerSweep", fList.size());
	for (int i = 0; i < fList.size(); i++)
	{
		QDomElement fi = qDom.createElement("Frequency");
		fi.setAttribute("Index", i);
		fi.setAttribute("Freq_Hz", fList[i]);
		fi.setAttribute("Sampling_Hz", sRList[i]);
		freq.appendChild(fi);
	}

	QDomElement chan = qDom.createElement("Channels");
	root.appendChild(chan);
	QVector<ChannelInformation> cI = global->getChannelInformation();
	for (int i = 0; i < cI.size(); i++)
	{
		QDomElement ci = qDom.createElement("Channel");
		ci.setAttribute("Index", cI.value(i).ChannelNumber);
		ci.setAttribute("UnitsId", (int)(cI.value(i).Units));
		ci.setAttribute("TypeId",  (int)(cI.value(i).Type ));
		ci.setAttribute("SeriesR_Ohm", cI.value(i).SeriesR);
		chan.appendChild(ci);
	}
	chan.setAttribute("NumChannels", cI.size());
	qDom.save(stream, 3);
	return true;
}


void ImpRobotExp::writeDataToFiles()
{
	bool writeSuccess = writeSettings();
	if (!writeSuccess) {
		QMessageBox::information(this,
				"Impedance Measurement DAQ",
				"Error while writing settings file!");
		return;
	}
	QFile file(dirPath + "/Position.dat");
	file.open(QIODevice::WriteOnly);
	for (int i = 0; i < posMsmt.size(); i++) {
		long t  = posMsmt.value(i).t;
		long pos = (long)(posMsmt.value(i).pos);
		file.write((char*)&t, sizeof(long));
		file.write((char*)&pos, sizeof(long));
	}
	file.close();

    QFile fileI(dirPath + "/Impedance.dat");
	fileI.open(QIODevice::WriteOnly);
	for (int i = 0; i < impMsmt.size(); i++) {
		double t = impMsmt.value(i).t;
		double fc = impMsmt.value(i).fc;
		double fs = impMsmt.value(i).fs;
		fileI.write((char*)&t , sizeof(double));
		fileI.write((char*)&fc, sizeof(double));
		fileI.write((char*)&fs, sizeof(double));
		for (int n = 0; n < impMsmt.value(i).X.size(); n++) {
			double xr = std::real(impMsmt.value(i).X.value(n));
			double xi = std::imag(impMsmt.value(i).X.value(n));
			double r = impMsmt.value(i).r.value(n);
			fileI.write((char*)&xr, sizeof(double));
			fileI.write((char*)&xi, sizeof(double));
			fileI.write((char*)&r , sizeof(double));
		}
	}
	fileI.close();

	writeMATLABCode();
}

void ImpRobotExp::writeMATLABCode()
{
    QFile fileI(dirPath + "/ImpAndRbt.m");
	fileI.open(QIODevice::WriteOnly);
	QVector<ChannelInformation> cI = global->getChannelInformation();
	QString tStr = QString (
			"fid = fopen('Position.dat', 'r');\n"
			"pos = fread(fid, [2 inf], 'long');\n"
			"fclose(fid);\n"
                        "tp = pos(1,:)';\n"
			"p = pos(2,:)';\n"
			"clear pos;\n"
			"fid = fopen('Impedance.dat', 'r');\n"
			"imp = fread(fid, [%1 inf], 'double');\n"
			"fclose(fid);\n"
			"t  = imp(1,:)';\n"
			"fc = imp(2,:)';\n"
			"fs = imp(3,:)';\n"
			"Z  = [];\n"
		).arg(cI.size()*3 + 3);
	fileI.write(tStr.toAscii());
	for (int i = 0; i < cI.size(); i++) {
		int realI = 3*(i + 1) + 1;
		int imagI = realI + 1;
		switch(cI.value(i).Type) {
		case ChannelInformation::CT_CARRIER:
                        tStr = QString("V  = imp(%1,:)' + 1i*imp(%2,:)';\n")
					.arg(realI).arg(imagI);
			fileI.write(tStr.toAscii());
			break;
		case ChannelInformation::CT_IMPEDANCE:
                        tStr = QString("Z = [Z (imp(%1,:)' + 1i*imp(%2,:)')];\n")
					.arg(realI).arg(imagI);
			fileI.write(tStr.toAscii());
			break;
		case ChannelInformation::CT_HUMIDITY:
			tStr = QString("hu  = imp(%1,:)';\n").arg(realI);
			fileI.write(tStr.toAscii());
			break;
		case ChannelInformation::CT_TEMPERATURE:
		case ChannelInformation::CT_TEMP_5V:
			tStr = QString("tc  = imp(%1,:)';\n").arg(realI);
			fileI.write(tStr.toAscii());
			break;
                case ChannelInformation::CT_DC_VOLTS:
                case ChannelInformation::CT_AC_VOLTS:
                        break;
		}
	}
	fileI.write("clear imp;\n");
	fileI.close();
}

int ImpRobotExp::FreqListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
	return fL.size();
}

int ImpRobotExp::FreqListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
	return 2;
}

QVariant ImpRobotExp::FreqListModel::
			data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (index.row() >= fL.size() || index.row() < 0)
         return QVariant();

     if (role == Qt::DisplayRole) {
    	 int r = index.row();
    	 switch(index.column()) {
    	 case 0: return fL[r];
    	 case 1: return sL[r];
    	 }
     }
     return QVariant();
 }

 QVariant ImpRobotExp::FreqListModel::
	 headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role != Qt::DisplayRole)
         return QVariant();

     if (orientation == Qt::Horizontal) {
         switch (section) {
             case 0: return tr("Fc");
             case 1: return tr("Fs");
             default:
                 return QVariant();
         }
     }
     else if (orientation == Qt::Vertical) {
    	return section+1;
     }
     return QVariant();
}

void ImpRobotExp::FreqListModel::resetFreqList(QList<double> fL, QList<int> sL)
{
	this->fL = fL;
	this->sL = sL;
	emit layoutChanged();
}
