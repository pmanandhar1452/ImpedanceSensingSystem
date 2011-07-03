#ifndef SINGLEFREQIMPDLG_H
#define SINGLEFREQIMPDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>

#include "Global.h"
#include "abstractexperimentdlg.h"
#include "Measurement.h"

namespace Ui {
    class SingleFreqImpDlg;
}

class SingleFreqImpDlg : public AbstractExperimentDlg
{
    Q_OBJECT

public:
    explicit SingleFreqImpDlg(QWidget *parent = 0);
    ~SingleFreqImpDlg();


private:
    Ui::SingleFreqImpDlg *ui;
    Global * global;

    QVector<ChannelInformation> cI;

    QString dirPath;

    QCheckBox chkChn[Global::N_CHANNELS];
    QComboBox cmbChn[Global::N_CHANNELS];
    QComboBox cmbCType[Global::N_CHANNELS];
    QDoubleSpinBox  spnRs[Global::N_CHANNELS];

    void connectUpdates();
    bool writeSettings ();
    void writeDataToFiles ();
    void writeMATLABCode();

    void testAgConn(QString strName, QString strId);
    void initFreqGenUI();
    void createChannelsUI();


private slots:
    void selectDeviceID();
    void onExit();
    void testAgConn();
    void updateChannelsUI();
    void updateFreqUI();
    void selectDataFolder();
    void processMeasurement();

};

#endif // SINGLEFREQIMPDLG_H
