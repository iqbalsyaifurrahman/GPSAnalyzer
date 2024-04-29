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
                    // Call splitData function
                    splitData(myString);
                }
                dataBuffer = "";
            }
        }
    }
}

void GPSAnalyzer::splitData(QString splitString){

    // Create list, splitting by ','
    QStringList listGNRMC = splitString.split(QLatin1Char(','));
    // qDebug() << listGNRMC;

    // Splitting Date
    QString date_string = listGNRMC[9];
    QString time_string = listGNRMC[1];
    // qDebug() << date_string;
    QDateTime temp_date = QDateTime::fromString(date_string,"ddMMyy");
    QDateTime temp_time = QDateTime::fromString(time_string,"HHmmss.zzz");
    // qDebug() << Date;
    temp_date = temp_date.addYears(100);
    // qDebug() << Date.toString("dd-MM-yyyy HH:mm:ss") ;

    // Speed knots to Kph
    listGNRMC[7] = QString::number(listGNRMC[7].toFloat() * 1.825);

    // Convert Longitude Latitude Hemisphere
    // QString latdummy = "0737.0224";
    // QString longdummy = "11131.4729";

    // float lati = (latdummy.mid(0,2)).toFloat() + ((latdummy.mid(2,7)).toFloat() / 60);
    // float longi = (longdummy.mid(0,3)).toFloat() + ((longdummy.mid(3,7)).toFloat() / 60);

    float latitude = (listGNRMC[3].mid(0,2)).toFloat() + ((listGNRMC[3].mid(2,7)).toFloat() / 60);
    float longitude = (listGNRMC[5].mid(0,3)).toFloat() + ((listGNRMC[5].mid(3,7)).toFloat() / 60);

    listGNRMC[3] = QString::number(latitude);
    listGNRMC[5] = QString::number(longitude);

    // qDebug() << listGNRMC[3];
    // qDebug() << listGNRMC[5];

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
    listGNRMC.push_front(temp_time.toString("HH:mm:ss"));
    listGNRMC.push_front(temp_date.toString("dd-MM-yyyy"));

    qDebug() << listGNRMC;

    // Call writeCSV funtion
    writeCSV(listGNRMC);

}

void GPSAnalyzer::writeCSV(QStringList listCSV){
    // Set row initiation for tableWidget
    ui ->tableWidget -> setRowCount(row);

    // Create file "log.csv"
    QFile CSVFile("log.csv");

    // Write and append splitted data to CSV
    if(CSVFile.open(QIODevice::ReadWrite | QIODevice::Append)){
        QTextStream Stream(&CSVFile);
        for (int i = 0; i < listCSV.size(); i++){
            ui -> tableWidget ->setItem(k, i, new QTableWidgetItem(listCSV[i]));
            if (i == listCSV.size()-1)
                Stream << listCSV[i];
            else
                Stream << listCSV[i] << ",";
        }

        // Create new row in CSV and table widget
        Stream << "\r\n";
        k += 1;
        row += 1;
    }
    CSVFile.close();
}

void GPSAnalyzer::on_pushButton_clicked()
{
    if (start == 0)
    {
        start = 1;
        ui ->pushButton -> setText(QString("Stop"));
        // Choosing port and baud by user
        QString currentPort = ui ->comboBox ->currentText();
        QString baudRateText = ui->comboBox_2->currentText();
        qint32 currentBaudrate = baudRateText.toInt();

        // Setting serial port for BN353N
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

        // Create signal that serial port is ready to use to readData function
        connect(COMPORT, SIGNAL(readyRead()), this, SLOT(readData()));
    }
    else
    {
        ui ->pushButton -> setText(QString("Start"));
        start = 0;
        COMPORT -> close();
        if (!(COMPORT -> isOpen()))
            qDebug() << "Serial closed";
    }
}

