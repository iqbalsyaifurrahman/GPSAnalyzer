#include "gpsanalyzer.h"
#include "ui_gpsanalyzer.h"
#include <iostream>
#include <string>

GPSAnalyzer::GPSAnalyzer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GPSAnalyzer)
{
    ui ->setupUi(this);


    for (const QSerialPortInfo &portinfor : QSerialPortInfo::availablePorts())
    {
        qDebug() << "Port Name : " << portinfor.portName();
        qDebug() << "Port Location : " << portinfor.systemLocation();
        qDebug() << "Device Manufacturer : " << portinfor.systemLocation();

        ui->comboBox->addItem(portinfor.portName());
    }

    QString currentPort = ui ->comboBox ->currentText();
    QString baudRateText = ui->comboBox_2->currentText();
    qint32 currentBaudrate = baudRateText.toInt();

    COMPORT = new QSerialPort();
    COMPORT ->setPortName(currentPort);
    COMPORT ->setBaudRate(currentBaudrate);
    COMPORT ->setParity(QSerialPort::Parity::NoParity);
    COMPORT ->setDataBits(QSerialPort::DataBits::Data8);
    COMPORT ->setStopBits(QSerialPort::StopBits::OneStop);
    COMPORT ->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    COMPORT ->open(QIODevice::ReadWrite);

    if (COMPORT -> isOpen()){
        qDebug() << "Serial connected";
    }
    else {
        qDebug() << "Serial not connected";
    }

    connect(COMPORT, SIGNAL(readyRead()), this, SLOT(Read_Data()));
}

GPSAnalyzer::~GPSAnalyzer()
{
    delete ui;
}

void GPSAnalyzer::Read_Data()
{
    if (COMPORT -> isOpen())
    {
        while (COMPORT ->bytesAvailable()){
            data = COMPORT ->readAll();
            dataBuffer.append(data);
            if (data.contains("\n")){
                QString myString(dataBuffer);
                ui ->tableWidget -> setRowCount(row);
                if(myString.startsWith("$GNRMC"))
                {
                    QStringList listGNRMC = myString.split(QLatin1Char(','));
                    // ui -> textEdit ->append(listGNRMC);
                    qDebug() << listGNRMC;

                    for (int i = 0; i < 13; i++){
                        ui -> tableWidget ->setItem(k, i, new QTableWidgetItem(listGNRMC[i]));
                    }
                    k += 1;
                    row +=1;
                }
                dataBuffer = "";
            }

        }
    }
}
