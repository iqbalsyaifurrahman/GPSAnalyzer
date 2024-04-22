#include "gpsanalyzer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GPSAnalyzer w;
    w.show();
    return a.exec();
}
