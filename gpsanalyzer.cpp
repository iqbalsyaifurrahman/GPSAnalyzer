#include "gpsanalyzer.h"
#include "ui_gpsanalyzer.h"

GPSAnalyzer::GPSAnalyzer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GPSAnalyzer)
{
    ui->setupUi(this);

    for (const QSerialPortInfo &portinfor : QSerialPortInfo::availablePorts())
    {
        qDebug() << "Port Name : " << portinfor.portName();
        qDebug() << "Port Location : " << portinfor.systemLocation();
        qDebug() << "Device Manufacturer : " << portinfor.systemLocation();

        ui->comboBox->addItem(portinfor.portName());
    }

    COMPORT = new QSerialPort();
    COMPORT ->setPortName("COM3");
    COMPORT ->setBaudRate(QSerialPort::BaudRate::Baud4800);
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

    connect(COMPORT, SIGNAL(readyRead()), this, SLOT((Read_Data())));
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
            Data_From_SerialPort += COMPORT ->readAll();
            qDebug() << Data_From_SerialPort;
        }
    }
}


