#include "gpsanalyzer.h"
#include "ui_gpsanalyzer.h"
#include <iostream>
#include <string>

GPSAnalyzer::GPSAnalyzer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GPSAnalyzer)
{
    ui ->setupUi(this);
    ui ->tableWidget ->horizontalHeader() ->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);

    // Check all serial ports
    for (const QSerialPortInfo &portinfor : QSerialPortInfo::availablePorts())
    {
        qDebug() << "Port Name : " << portinfor.portName();
        qDebug() << "Port Location : " << portinfor.systemLocation();
        qDebug() << "Device Manufacturer : " << portinfor.systemLocation();

        ui->comboBox->addItem(portinfor.portName());
    }

    // Choosing port and baud by user
    QString currentPort = ui ->comboBox ->currentText();
    QString baudRateText = ui->comboBox_2->currentText();
    qint32 currentBaudrate = baudRateText.toInt();

    // Setting serial port for 353
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

    connect(COMPORT, SIGNAL(readyRead()), this, SLOT(readData()));

}

GPSAnalyzer::~GPSAnalyzer()
{
    delete ui;
}

void GPSAnalyzer::readData()
{
    if (COMPORT -> isOpen())
    {
        while (COMPORT ->bytesAvailable()){
            data = COMPORT ->readAll();
            dataBuffer.append(data);
            if (data.contains("\n")){
                QString myString(dataBuffer);
                if(myString.startsWith("$GNRMC"))
                {
                    splitData(myString);
                }
                dataBuffer = "";
            }
        }
    }
}

void GPSAnalyzer::splitData(QString splitString){
    ui ->tableWidget -> setRowCount(row);

    QStringList listGNRMC = splitString.split(QLatin1Char(','));
    // ui -> textEdit ->append(listGNRMC);
    // qDebug() << listGNRMC;

    // Splitting Date
    QString date_string = listGNRMC[9]+ " " + listGNRMC[1];
    // qDebug() << date_string;
    QDateTime Date = QDateTime::fromString(date_string,"ddMMyy HHmmss.zzz");
    // qDebug() << Date;
    Date = Date.addYears(100);
    // qDebug() << Date.toString("dd-MM-yyyy HH:mm:ss") ;

    // Speed knots to Kph
    listGNRMC[7] = QString::number(listGNRMC[7].toFloat() * 1.825);

    // Convert Longitude Latitude Hemisphere
    if (listGNRMC[4] == "S")
        listGNRMC[3] = "-" + listGNRMC[3];
    if (listGNRMC[6] == "W")
        listGNRMC[5] = "-" + listGNRMC[5];

    // Remove Format
    listGNRMC.pop_front();
    // Remove UTC Time
    listGNRMC.pop_front();
    // Remove Latitude Hemisphere
    listGNRMC.removeAt(2);
    // Remove Longitude Hemisphere
    listGNRMC.removeAt(3);
    // Remove UTC Date
    listGNRMC.removeAt(5);
    // Remove XOR Check
    listGNRMC.removeAt(8);

    //Add timestamp
    listGNRMC.push_front(Date.toString("dd-MM-yyyy HH:mm:ss"));

    qDebug() << listGNRMC;

    writeCSV(listGNRMC);

}

void GPSAnalyzer::writeCSV(QStringList listCSV){
    QFile CSVFile("C:/Users/Iqbal/OneDrive - UGM 365/Inka/Project GUI GPS/GPSAnalyzer/log.csv");
    if(CSVFile.open(QIODevice::ReadWrite | QIODevice::Append)){
        QTextStream Stream(&CSVFile);
        for (int i = 0; i < listCSV.size(); i++){
            ui -> tableWidget ->setItem(k, i, new QTableWidgetItem(listCSV[i]));
            if (i == listCSV.size()-1)
                Stream << listCSV[i];
            else
                Stream << listCSV[i] << ",";
        }
        Stream << "\r\n";
        k += 1;
        row +=1;
    }
    CSVFile.close();
}
