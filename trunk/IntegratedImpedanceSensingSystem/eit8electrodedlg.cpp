/**
  eit8electrodedlg.cpp
  Implementation of eit8electrodedlg.h

  2010-Aug-11, Created Prakash Manandhar
**/

#include "eit8electrodedlg.h"
#include "ui_eit8electrodedlg.h"

#include <QFileDialog>
#include <QDateTime>
#include <QDir>
#include <QInputDialog>
#include <QDebug>
#include <visa.h>
#include <QMessageBox>
#include <QDomDocument>
#include <QList>

#include "eitmeasurementloop.h"

EIT8ElectrodeDlg::EIT8ElectrodeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::eit8electrodedlg), cI(N_ELECTRODES + 1)
{
    ui->setupUi(this);
    global = Global::instance();
    connect(this, SIGNAL(finished(int)), this, SLOT(onExit()));

    createChannelInformationVector();

    ui->txtDataFolder->setText(global->getDataFolder());
    connect(ui->btnSelDataFolder, SIGNAL(clicked()), this, SLOT(selectDataFolder()));

    ui->lblSwitchDelay->setText(QString("%1").arg(SWITCHING_DELAY));

    ui->spnSeriesR->setValue(global->getEITSeriesR());
    ui->spnExpCycles->setValue(global->getEITExpCycles());

    ui->spnSamplingFreq->setValue(global->getEITSamplingFreq());
    ui->spnOnTime->setValue(global->getEITOnTime());
    ui->spnOffTime->setValue(global->getEITOffTime());

    ui->spnAmp->setValue(global->getAmplitude());
    ui->spnFreq->setValue(global->getFrequencyStart());
    ui->spnCyclesPerIm->setValue(global->getCyclesPerIm());

    QString strDevID = global->getU2751ADeviceID();
    if (strDevID != "") ui->lblSMDevID->setText(strDevID);
    strDevID = global->getA33220ADeviceID();
    if (strDevID != "") ui->lblSGDevID->setText(strDevID);

    connect(ui->spnExpCycles, SIGNAL(valueChanged(int)), global, SLOT(setEITExpCycles(int)));

    connect(ui->spnSeriesR, SIGNAL(valueChanged(double)), global, SLOT(setEITSeriesR(double)));
    connect(ui->spnSamplingFreq, SIGNAL(valueChanged(int)), global, SLOT(setEITSamplingFreq(int)));
    connect(ui->txtDataFolder, SIGNAL(textChanged(QString)), global, SLOT(setDataFolder(QString)));
    connect(ui->spnExpCycles, SIGNAL(valueChanged(int)), global, SLOT(setEITExpCycles(int)));
    connect(ui->spnOnTime, SIGNAL(valueChanged(int)), global, SLOT(setEITOnTime(int)));
    connect(ui->spnOffTime, SIGNAL(valueChanged(int)), global, SLOT(setEITOffTime(int)));

    connect(ui->spnAmp, SIGNAL(valueChanged(double)), global, SLOT(setAmplitude(double)));
    connect(ui->spnFreq, SIGNAL(valueChanged(double)), global, SLOT(setFrequencyStart(double)));
    connect(ui->spnCyclesPerIm, SIGNAL(valueChanged(int)), global, SLOT(setCyclesPerIm(int)));

    connect(ui->spnExpCycles, SIGNAL(valueChanged(int)), this, SLOT(updateStatus()));
    connect(ui->spnOnTime, SIGNAL(valueChanged(int)) , this, SLOT(updateStatus()));
    connect(ui->spnOffTime, SIGNAL(valueChanged(int)), this, SLOT(updateStatus()));

    connect(ui->spnFreq, SIGNAL(valueChanged(double)), this, SLOT(updateStatus()));
    connect(ui->spnCyclesPerIm, SIGNAL(valueChanged(int)), this, SLOT(updateStatus()));

    connect(ui->btnSMDevID, SIGNAL(clicked()), this, SLOT(selectSMDeviceID()));
    connect(ui->btnSMTestAgConn, SIGNAL(clicked()), this, SLOT(testSMAgConn()));
    connect(ui->btnSGDevID, SIGNAL(clicked()), this, SLOT(selectSGDeviceID()));
    connect(ui->btnSGTestAgConn, SIGNAL(clicked()), this, SLOT(testSGAgConn()));

    connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(startEITExp()));

    updateStatus();

    ui->verticalLayout->setMargin(10);
    setLayout(ui->verticalLayout);
}

EIT8ElectrodeDlg::~EIT8ElectrodeDlg()
{
    delete ui;
}

void EIT8ElectrodeDlg::onExit()
{
    Global::destroy();
}

void EIT8ElectrodeDlg::selectDataFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Data Folder",
                    ui->txtDataFolder->text(), QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty())
    {
        ui->txtDataFolder->setText(dir);
    }
}

void EIT8ElectrodeDlg::selectSMDeviceID()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter Agilent U2751A VISA Device ID"),
                                              tr("VISA Device ID:"), QLineEdit::Normal,
                                              global->getU2751ADeviceID(), &ok);
    if (ok && !text.isEmpty()) {
            ui->lblSMDevID->setText(text);
            global->setU2751ADeviceID(text);
    }
}

void EIT8ElectrodeDlg::selectSGDeviceID()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Enter Agilent 33220A VISA Device ID"),
                                              tr("VISA Device ID:"), QLineEdit::Normal,
                                              global->getA33220ADeviceID(), &ok);
    if (ok && !text.isEmpty()) {
            ui->lblSGDevID->setText(text);
            global->setA33220ADeviceID(text);
    }
}


void EIT8ElectrodeDlg::testAgConn(QString strName, QString strId)
{
    ViSession defaultRM, vi;
    char buf [1000] = {0};

    /* Open session to GPIB device at address 22 */
    viOpenDefaultRM (&defaultRM);
    viOpen (defaultRM, strId.toAscii().data(), VI_NULL,VI_NULL, &vi);

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

    QMessageBox::information(this, strName, buf);
}

void EIT8ElectrodeDlg::testSMAgConn() {
    testAgConn("Agilent U2751A Switch Matrix", global->getU2751ADeviceID());
}

void EIT8ElectrodeDlg::testSGAgConn() {
    testAgConn("Agilent 33220A Signal Generator", global->getA33220ADeviceID());
}

void EIT8ElectrodeDlg::startEITExp()
{
    impMsmt.clear();
    QDateTime qdt = QDateTime::currentDateTime();
    dirPath = ui->txtDataFolder->text()
                    + "/" + qdt.toString("dd.MM.yyyy hh.mm.ss.zzz");
    QDir dir;
    if (!dir.mkdir(dirPath)) {
            QMessageBox::information(this,
                            "Impedance Measurement DAQ",
                            "Error while creating output directory!");
            return;
    }
    EITMeasurementLoop ml(this, &impMsmt);
    ml.measure();
    writeDataToFiles();
}

void EIT8ElectrodeDlg::updateStatus()
{
    QString str = QString("%1").arg(getCycleTime()*1000);
    ui->lblCycleTime->setText(str);
    ui->lblTotalTime->setText(Global::toHrMinSec(getTotalTime()));
    QString strColor;
    if (getSweepTime() > global->getEITOnTime()/1000.0)
        strColor = "#ff0000";
    else strColor = "#00ff00";
    ui->lblSweepTime->setText(
            QString("<span style=\" font-size:8pt; font-weight:600; color:%1;\">"
                    "Sweep time: %2 s</span>").arg(strColor).arg(getSweepTime()));


}


double EIT8ElectrodeDlg::getSweepTime()
{
    return global->getCyclesPerIm()/global->getFrequencyStart();
}

double EIT8ElectrodeDlg::getCycleTime()
{
    return (ui->spnOnTime->value() + ui->spnOffTime->value()
            + 2*SWITCHING_DELAY
            )*N_ELECTRODES/1000.0;
}

double EIT8ElectrodeDlg::getTotalTime()
{
    return getCycleTime()*ui->spnExpCycles->value();
}


QList<ImpedanceMeasurement> * EIT8ElectrodeDlg::getImpMeasurement()
{
        return &impMsmt;
}

QVector<ChannelInformation> EIT8ElectrodeDlg::getChannelInformation() {
    return cI;
}

void EIT8ElectrodeDlg::createChannelInformationVector() {

    for (int i = 0; i < N_ELECTRODES + 1; i++) {

        cI[i].ChannelNumber = i;
        cI[i].Units = ChannelInformation::DAQPM_500mV;
        if (i == 0)
            cI[i].Type = ChannelInformation::CT_CARRIER;
        else
            cI[i].Type = ChannelInformation::CT_AC_VOLTS;
        cI[i].SeriesR = 0;
    }

}

bool EIT8ElectrodeDlg::writeSettings()
{
    QDomDocument qDom;
    QFile xmlF(dirPath + "/ExpSettings.xml");
    if (!xmlF.open(QIODevice::ReadWrite)) return false;
    QTextStream stream(&xmlF);
    QDomElement root = qDom.createElement("Experiment");
    root.setAttribute("Type", "EIT_8_Electrode");
    root.setAttribute("Version", "0.1");
    root.setAttribute("SamplingFrequency_Hz", global->getEITSamplingFreq());
    root.setAttribute("ExcitationFrequency_Hz", global->getFrequencyStart());
    root.setAttribute("Amplitude_mV", global->getAmplitude());
    root.setAttribute("CyclesPerIm", global->getCyclesPerIm());
    qDom.appendChild(root);

    QDomElement chan = qDom.createElement("Channels");
    root.appendChild(chan);

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


void EIT8ElectrodeDlg::writeDataToFiles()
{
    bool writeSuccess = writeSettings();
    if (!writeSuccess) {
        QMessageBox::information(this,
                        "Impedance Measurement DAQ",
                        "Error while writing settings file!");
        return;
    }

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

void EIT8ElectrodeDlg::writeMATLABCode()
{
    QFile fileI(dirPath + "/ImpAndRbt.m");
    fileI.open(QIODevice::WriteOnly);

    QString tStr = QString (
                    "fid = fopen('Impedance.dat', 'r');\n"
                    "imp = fread(fid, [%1 inf], 'double');\n"
                    "fclose(fid);\n"
                    "t  = imp(1,:)';\n"
                    "fc = imp(2,:)';\n"
                    "fs = imp(3,:)';\n"
                    "Z  = [];\n"
                    "Rs = %2;\n"
            ).arg(cI.size()*3 + 3).arg(global->getEITSeriesR());
    fileI.write(tStr.toAscii());
    for (int i = 0; i < cI.size(); i++) {
            int realI = 3*(i + 1) + 1;
            int imagI = realI + 1;
            switch(cI.value(i).Type) {
            case ChannelInformation::CT_CARRIER:
                    tStr = QString("V  = imp(%1,:)' + j*imp(%2,:)';\n")
                                    .arg(realI).arg(imagI);
                    fileI.write(tStr.toAscii());
                    break;
            case ChannelInformation::CT_IMPEDANCE:
            case ChannelInformation::CT_DC_VOLTS:
            case ChannelInformation::CT_AC_VOLTS:
                    tStr = QString("Z = [Z (imp(%1,:)' + j*imp(%2,:)')];\n")
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
            }
    }
    fileI.write("clear imp;\n");
    fileI.close();
}
