#ifndef HEATERMETER_H
#define HEATERMETER_H

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include "restapi.h"

class HeaterMeter : public QWidget
{
    Q_OBJECT

public:
    HeaterMeter(QString, QWidget *parent = 0);
    ~HeaterMeter();
    void getVersion();
    void getConfig();

protected slots:
    void apiVersion(int);
    void firmwareVersion(QString&);
    void statusUpdate(QString, double);
    void probe(int, QString);
    void lowTriggerValue(QString, int);
    void highTriggerValue(QString, int);
    
private:
    RestAPI *m_rest;

    QMap<QString, QLabel*> m_probeNames;
    QMap<QString, QLCDNumber*> m_probeValues;
    QMap<QString, int> m_lowTriggerValue;
    QMap<QString, int> m_highTriggerValue;
    QGridLayout *m_layout;
    QString m_host;
};

#endif // HEATERMETER_H
