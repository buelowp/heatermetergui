/**
 * HeaterMeter
 * heatermeter.cpp
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

#include "heatermeter.h"
#include "ui_heatermeter.h"

HeaterMeter::HeaterMeter(QString host, QWidget *parent) : QWidget(parent), m_host(host)
{
    m_rest = new RestAPI(host, this);
    m_rest->run();
    m_graph = nullptr;
    
    connect(m_rest, SIGNAL(statusUpdate(QString, double)), this, SLOT(statusUpdate(QString, double)));
    connect(m_rest, SIGNAL(apiVersion(int)), this, SLOT(apiVersion(int)));
    connect(m_rest, SIGNAL(firmwareVersion(QString&)), this, SLOT(firmwareVersion(QString&)));
    connect(m_rest, SIGNAL(probeFound(int, QString)), this, SLOT(probe(int, QString)));
    connect(m_rest, SIGNAL(lowTrigger(QString, int)), this, SLOT(lowTriggerValue(QString, int)));
    connect(m_rest, SIGNAL(highTrigger(QString, int)), this, SLOT(highTriggerValue(QString, int)));
    connect(m_rest, SIGNAL(configComplete()), this, SLOT(configComplete()));
    
    m_layout = new QGridLayout(this);
}

HeaterMeter::~HeaterMeter()
{
}

void HeaterMeter::apiVersion(int version)
{
    qDebug() << __PRETTY_FUNCTION__ << ": API version is" << version;
}

void HeaterMeter::firmwareVersion(QString &version)
{
    qDebug() << __PRETTY_FUNCTION__ << ": Firmware version is" << version;
}

void HeaterMeter::statusUpdate(QString name, double val)
{
    QLCDNumber *num = m_probeValues[name];
    int ltv = 0;
    int htv = 0;
    
    if (m_lowTriggerValue.size() > 0)
        ltv = m_lowTriggerValue[name];

    if (m_highTriggerValue.size() > 0)
        htv = m_highTriggerValue[name];
    
    if (num) {
        LineSeries *ls = m_series[name];
        if (ls) {
            ls->enqueue(val);
        }
        QString v = QString("%1").arg(val, 3, 'f', 1);
        num->display(v);
        if (ltv > 0 && val <= ltv) {
            num->setPalette(Qt::blue);
        }
        else if (htv > 0 && val >= htv) {
            num->setPalette(Qt::red);
        }
        else {
            num->setPalette(Qt::white);
        }
    }
}

void HeaterMeter::getVersion()
{
    m_rest->getVersion();
}

void HeaterMeter::lowTriggerValue(QString name, int val)
{
    m_lowTriggerValue[name] = val;
}

void HeaterMeter::highTriggerValue(QString name, int val)
{
    m_highTriggerValue[name] = val;
}

void HeaterMeter::probe(int which, QString name)
{
    QLabel *n = new QLabel(this);
    QLCDNumber *v = new QLCDNumber(this);
    LineSeries *ls = new LineSeries(width(), name);
    n->setText(name);
    n->setAlignment(Qt::AlignCenter);
    v->setDigitCount(5);
    v->setSegmentStyle(QLCDNumber::Flat);
    v->display(0.0);
    v->setPalette(Qt::red);
    m_layout->addWidget(n, 0, which - 1);
    m_layout->addWidget(v, 1, which - 1);
    m_probeNames[name] = n;
    m_probeValues[name] = v;
    m_series[name] = ls;
    qDebug() << __PRETTY_FUNCTION__ << ": adding pointer to ls" << ls;
}

void HeaterMeter::getConfig()
{
    m_rest->getConfig();
}

void HeaterMeter::configComplete()
{
    if (!m_graph) {
        m_graph = new TempGraph();

        qDebug() << __PRETTY_FUNCTION__ << ": m_series has" << m_series.count() << "entries";
        QMapIterator<QString, LineSeries*> i(m_series);
        while (i.hasNext()) {
            i.next();
            LineSeries *ls = i.value();
            m_graph->addLineSeries(i.key(), ls);
        }

        m_layout->addWidget(m_graph, 2, 0, 1, 3);
    }
}

