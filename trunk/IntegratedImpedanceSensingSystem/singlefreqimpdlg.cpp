#include "singlefreqimpdlg.h"
#include "ui_singlefreqimpdlg.h"
#include <QVector>
#include <QGridLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QDateTime>
#include <QDomDocument>
#include <QTextStream>
#include <QDebug>

#include <visa.h>
#include <math.h>
#include <stdio.h>

#include "FrequencyListGenerator.h"
#include "MeasurementLoop.h"

SingleFreqImpDlg::SingleFreqImpDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SingleFreqImpDlg)
{
    ui->setupUi(this);
    global = Global::instance();
    connect(this, SIGNAL(finished(int)), this, SLOT(onExit()));

    initFreqGenUI();

    ui->verticalLayout->setMargin(10);
    setLayout(ui->verticalLayout);
}

void SingleFreqImpDlg::updateFreqUI()
{
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
        ui->lblTotalTime->setText(strT);
        ui->lblSweepTime->setText(QString("Sweep time: ")
                        + Global::toHrMinSec(swpT));
}

void SingleFreqImpDlg::initFreqGenUI()
{

        QString strDevID = global->getA33220ADeviceID();
        if (strDevID != "")
                ui->lblSGDevID->setText(strDevID);
        else
                qDebug() << "Device ID was not set previously";

        ui->txtDataFolder->setText(global->getDataFolder());

        ui->spnAmp->setValue(global->getAmplitude());
        ui->spnSamplingFactor->setValue(global->getSamplingFactor());
        ui->spnSamplingMax->setValue(global->getSamplingMax());
        ui->spnFreq->setValue(global->getFrequencyStart());
        ui->spnCyclesPerIm->setValue(global->getCyclesPerIm());
        ui->spnNumSweep->setValue(global->getNumSweep());

        global->setFrequencySteps(1);

        connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(processMeasurement()));
        connect(ui->btnSGDevID, SIGNAL(clicked()), this, SLOT(selectDeviceID()));
        connect(ui->btnSGTestAgConn, SIGNAL(clicked()), this, SLOT(testAgConn()));
        connect(ui->btnSelDataFolder, SIGNAL(clicked()), this, SLOT(selectDataFolder()));
        connect(ui->txtDataFolder, SIGNAL(textChanged(QString)), global, SLOT(setDataFolder(QString)));

        connect(ui->spnAmp, SIGNAL(valueChanged(double)), global, SLOT(setAmplitude(double)));
        connect(ui->spnSamplingFactor, SIGNAL(valueChanged(int)), global, SLOT(setSamplingFactor(int)));
        connect(ui->spnSamplingMax, SIGNAL(valueChanged(int)), global, SLOT(setSamplingMax(int)));

        connect(ui->spnFreq, SIGNAL(valueChanged(double)), global, SLOT(setFrequencyStart(double)));
        connect(ui->spnFreq, SIGNAL(valueChanged(double)), global, SLOT(setFrequencyEnd(double)));
        connect(ui->spnCyclesPerIm, SIGNAL(valueChanged(int)), global, SLOT(setCyclesPerIm(int)));
        connect(ui->spnNumSweep, SIGNAL(valueChanged(int)), global, SLOT(setNumSweep(int)));


        connect(ui->spnFreq, SIGNAL(valueChanged(double)), this, SLOT(updateFreqUI()));
        connect(ui->spnCyclesPerIm, SIGNAL(valueChanged(int)), this, SLOT(updateFreqUI()));
        connect(ui->spnNumSweep, SIGNAL(valueChanged(int)), this, SLOT(updateFreqUI()));
        connect(ui->spnSamplingFactor, SIGNAL(valueChanged(int)), this, SLOT(updateFreqUI()));
        connect(ui->spnSamplingMax, SIGNAL(valueChanged(int)), this, SLOT(updateFreqUI()));

        updateFreqUI ();
        createChannelsUI();
}


void SingleFreqImpDlg::createChannelsUI ()
{
        QGridLayout *vbox = new QGridLayout(ui->channelsGroup);
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
        QFrame * vline = new QFrame(ui->channelsGroup);
        vline->setFrameStyle(QFrame::VLine);
        vbox->addWidget(vline, 0, 5, 8, 1);
        ui->channelsGroup->setLayout(vbox);
}


SingleFreqImpDlg::~SingleFreqImpDlg()
{
    delete ui;
}

void SingleFreqImpDlg::testAgConn()
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

void SingleFreqImpDlg::onExit()
{
        //qDebug() << "Exiting application!!!";
        Global::destroy();
}

void SingleFreqImpDlg::selectDeviceID()
{
        bool ok;
        QString text = QInputDialog::getText(this, tr("Enter Agilent 33220 VISA Device ID"),
                                                  tr("VISA Device ID:"), QLineEdit::Normal,
                                                  global->getA33220ADeviceID(), &ok);
        if (ok && !text.isEmpty()) {
                ui->lblSGDevID->setText(text);
                global->setA33220ADeviceID(text);
        }
}

void SingleFreqImpDlg::selectDataFolder()
{
        QString dir = QFileDialog::getExistingDirectory(this, "Select Data Folder",
                        ui->txtDataFolder->text(), QFileDialog::ShowDirsOnly);
        if (!dir.isEmpty())
        {
                ui->txtDataFolder->setText(dir);
        }
}

void SingleFreqImpDlg::processMeasurement()
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
    MeasurementLoop ml(this, &impMsmt);
    ml.measure();
    writeDataToFiles();
}

bool SingleFreqImpDlg::writeSettings()
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


void SingleFreqImpDlg::writeDataToFiles()
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

void SingleFreqImpDlg::writeMATLABCode()
{
    QFile fileI(dirPath + "/ImpAndRbt.m");
        fileI.open(QIODevice::WriteOnly);
        QVector<ChannelInformation> cI = global->getChannelInformation();
        QString tStr = QString (
                        "fid = fopen('Position.dat', 'r');\n"
                        "pos = fread(fid, [2 inf], 'long');\n"
                        "fclose(fid);\n"
                        "if ~isempty(pos)\n"
                        "   tp = pos(1,:)';\n"
                        "   p = pos(2,:)';\n"
                        "end\n"
                        "clear pos;\n"
                        "fid = fopen('Impedance.dat', 'r');\n"
                        "imp = fread(fid, [%1 inf], 'double');\n"
                        "fclose(fid);\n"
                        "t   = imp(1,:)';\n"
                        "fc  = imp(2,:)';\n"
                        "fs  = imp(3,:)';\n"
                        "Z   = [];\n"
                        "rsZ = [];"
                ).arg(cI.size()*3 + 3);
        fileI.write(tStr.toAscii());
        for (int i = 0; i < cI.size(); i++) {
                int realI = 3*(i + 1) + 1;
                int imagI = realI + 1;
                int  resI = imagI + 1;
                switch(cI.value(i).Type) {
                case ChannelInformation::CT_CARRIER:
                        tStr = QString("V  = imp(%1,:)' + 1i*imp(%2,:)';\n")
                                        .arg(realI).arg(imagI);
                        tStr += QString("rsV  = imp(%1,:)';\n").arg(resI);
                        fileI.write(tStr.toAscii());
                        break;
                case ChannelInformation::CT_IMPEDANCE:
                        tStr = QString("Z = [Z (imp(%1,:)' + 1i*imp(%2,:)')];\n")
                                        .arg(realI).arg(imagI);
                        tStr += QString("rsZ = [rsZ imp(%1,:)'];\n").arg(resI);
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

void SingleFreqImpDlg::updateChannelsUI()
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
}
