#ifndef GPSANALYZER_H
#define GPSANALYZER_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QtSerialPort>
#include <QDate>
#include <QDateTime>
#include <QtCore>
#include <iostream>
#include <string>


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
    // void serialPort();
    void readData();
    void splitData(QString splitString);
    void writeCSV(QStringList listCSV);
    void on_pushButton_clicked();

private:
    Ui::GPSAnalyzer *ui;
    QSerialPort* COMPORT;
    QByteArray data;
    QByteArray dataBuffer;
    int k = 0;
    int row = 1;
    bool start = 0;


};
#endif // GPSANALYZER_H
