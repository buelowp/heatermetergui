/**
 * HeaterMeter
 * heatermeter.h
 * Copyright Peter Buelow (goballstate@gmail.com)
 * 
 * MIT License
 *
 * Copyright (c) 2019 Peter Buelow
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef HEATERMETER_H
#define HEATERMETER_H

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include "restapi.h"
#include "lineseries.h"
#include "tempgraph.h"

class HeaterMeter : public QWidget
{
    Q_OBJECT

public:
    HeaterMeter(QString, QWidget *parent = 0);
    ~HeaterMeter();
    void getVersion();
    void getConfig();
    void setMinGraphTemp(int t) { m_minGraphTemp = t; }
    void setMaxGraphTemp(int t) { m_maxGraphTemp = t; }

protected slots:
    void apiVersion(int);
    void firmwareVersion(QString&);
    void statusUpdate(QString, double);
    void probe(int, QString);
    void lowTriggerValue(QString, int);
    void highTriggerValue(QString, int);
    void configComplete();
    void timerStateChange();
    void timeout();
    
private:
    RestAPI *m_rest;
    TempGraph *m_graph;

    QMap<QString, QLabel*> m_probeNames;
    QMap<QString, QLCDNumber*> m_probeValues;
    QMap<QString, int> m_lowTriggerValue;
    QMap<QString, int> m_highTriggerValue;
    QMap<QString, LineSeries*> m_series;
    QGridLayout *m_layout;
    QString m_host;
    QPushButton *m_timerButton;
    QLabel *m_lidState;
    QTimer *m_timer;
    QTime m_timerValue;
    int m_minGraphTemp;
    int m_maxGraphTemp;
};

#endif // HEATERMETER_H
