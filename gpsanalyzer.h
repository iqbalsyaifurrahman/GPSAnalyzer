#ifndef GPSANALYZER_H
#define GPSANALYZER_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QtSerialPort>


QT_BEGIN_NAMESPACE
namespace Ui {
class GPSAnalyzer;
}
QT_END_NAMESPACE

class GPSAnalyzer : public QMainWindow
{
    Q_OBJECT

public:
    GPSAnalyzer(QWidget *parent = nullptr);
    ~GPSAnalyzer();

private slots:
    void Read_Data();

private:
    Ui::GPSAnalyzer *ui;
    QSerialPort* COMPORT;
    QByteArray data;
    QByteArray dataBuffer;
    bool IS_Data_Received = false;
    int k = 0;
    int row = 1;

};
#endif // GPSANALYZER_H
