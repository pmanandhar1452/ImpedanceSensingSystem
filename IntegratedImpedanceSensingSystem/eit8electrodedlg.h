#ifndef EIT8ELECTRODEDLG_H
#define EIT8ELECTRODEDLG_H

#include <QDialog>
#include <QList>
#include <cbw.h>

#include "Global.h"
#include "abstractexperimentdlg.h"
#include "Measurement.h"
#include "ChannelInformation.h"

namespace Ui {
    class eit8electrodedlg;
}

class EIT8ElectrodeDlg : public AbstractExperimentDlg
{
    Q_OBJECT

public:
    explicit EIT8ElectrodeDlg(QWidget *parent = 0);
    ~EIT8ElectrodeDlg();

    double getTotalTime (); // calculates the total time of the whole experiment in seconds
    double getCycleTime (); // calculates the time per cycle in seconds
    double getSweepTime (); // sweep time of impedance calculator in seconds

    static const int SWITCHING_DELAY = 10; // switch matrix switching delay in ms
    static const int N_ELECTRODES = 12;
    static const int N_ELECTRODES_ACTIVE = 8;
    static const ChannelInformation::DAQ_SCALING V_MAX = ChannelInformation::DAQPM_500mV;

    QVector<ChannelInformation> getChannelInformation ();

private:
    Ui::eit8electrodedlg *ui;
    Global * global;

    QVector<ChannelInformation> cI;

    QString dirPath;

    bool writeSettings ();
    void writeDataToFiles ();
    void writeMATLABCode();

    void testAgConn(QString strName, QString strId);
    void createChannelInformationVector();

private slots:
    void selectDataFolder();

    void selectSMDeviceID();
    void selectSGDeviceID();
    void testSMAgConn();
    void testSGAgConn();

    void startEITExp();

    void updateStatus();

    void onExit();
};

#endif // EIT8ELECTRODEDLG_H
