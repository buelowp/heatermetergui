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

HeaterMeter::HeaterMeter(QString host, QWidget *parent) : QWidget(parent), m_host(host)
{
    m_rest = new RestAPI(host, this);
    m_rest->run();
    m_graph = nullptr;
    m_timerValue = 0;
    
    m_layout = new QGridLayout(this);
    
    m_timerButton = new QPushButton();
    m_timerButton->setMaximumHeight(50);
    m_timerButton->setText("0:00:00");
    m_layout->addWidget(m_timerButton, 0, 0, 1, 2);
    m_lidState = new QLabel();
    m_layout->addWidget(m_lidState, 0, 2);
    m_lidState->setText("Closed");
    m_lidState->setMaximumHeight(50);
    
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    
    m_resizeEventTimer = new QTimer(this);
    m_resizeEventTimer->setInterval(1000);
    connect(m_resizeEventTimer, &QTimer::timeout, this, &HeaterMeter::setWindowLocation);
    
    m_moveEventTimer = new QTimer(this);
    m_moveEventTimer->setInterval(1000);
    connect(m_moveEventTimer, &QTimer::timeout, this, &HeaterMeter::setWindowLocation);

    connect(m_rest, SIGNAL(statusUpdate(QString, double)), this, SLOT(statusUpdate(QString, double)));
    connect(m_rest, SIGNAL(apiVersion(int)), this, SLOT(apiVersion(int)));
    connect(m_rest, SIGNAL(firmwareVersion(QString&)), this, SLOT(firmwareVersion(QString&)));
    connect(m_rest, SIGNAL(probeFound(int, QString)), this, SLOT(probe(int, QString)));
    connect(m_rest, SIGNAL(lowTrigger(QString, int)), this, SLOT(lowTriggerValue(QString, int)));
    connect(m_rest, SIGNAL(highTrigger(QString, int)), this, SLOT(highTriggerValue(QString, int)));
    connect(m_rest, SIGNAL(lidState(int)), this, SLOT(lidState(int)));
    connect(m_rest, SIGNAL(configComplete()), this, SLOT(configComplete()));
    connect(m_timerButton, SIGNAL(clicked()), this, SLOT(timerStateChange()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    
    QFont f("Roboto");
    f.setPixelSize(30);
    m_timerButton->setFont(f);
    m_lidState->setFont(f);
    m_lidState->setAlignment(Qt::AlignCenter);
    
    QSettings settings("heatermeter", "config");
    bool ta = settings.value("timer", false).toBool();
    if (ta) {
        QTime dt = settings.value("timerstarted").toTime();
        QTime now = QTime::currentTime();
        m_timerValue = dt.secsTo(now);
        qDebug() << __PRETTY_FUNCTION__ << ": Origin at" << dt << ", now is" << now << ", clock is going to start at" << m_timerValue << "seconds";
        timerStateChange();
    }
}

HeaterMeter::~HeaterMeter()
{
}

void HeaterMeter::setWindowLocation()
{
    QSettings settings("heatermeter", "config");
    settings.setValue("geometry", geometry());    
}

void HeaterMeter::closeEvent(QCloseEvent* event)
{
    QSettings settings("heatermeter", "config");
    settings.setValue("timer", false);    
}

void HeaterMeter::moveEvent(QMoveEvent *event)
{
    Q_UNUSED(event)
    
    if (m_moveEventTimer->isActive()) 
        m_moveEventTimer->stop();
    
    m_moveEventTimer->start();
}

void HeaterMeter::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    
    if (m_resizeEventTimer->isActive()) {
        m_resizeEventTimer->stop();
    }
    m_resizeEventTimer->start();
}

void HeaterMeter::lidState(int state)
{
    if (state == 0) {
        m_lidState->setText("Closed");
    }
    else {
        m_lidState->setText(QString("%1").arg(state));
    }
}

void HeaterMeter::timeout()
{
    int s = m_timerValue % 60;
    int m = (m_timerValue / 60) % 60;
    int h = (m_timerValue / 3600) % 24;
    
    m_timerButton->setText(QString("%1:%2:%3").arg(h).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0')));
    m_timerValue++;
}

void HeaterMeter::timerStateChange()
{
    QSettings settings("heatermeter", "config");

    if (m_timer->isActive()) {
        settings.setValue("timer", false);
        m_timer->stop();
    }
    else {
        settings.setValue("timer", true);
        settings.setValue("timerstarted", QTime::currentTime());
        m_timer->start();
        timeout();
    }
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
        QPalette p = num->palette();
        LineSeries *ls = m_series[name];
        if (ls) {
            ls->enqueue(val);
        }
        if (ltv > 0 && val <= ltv) {
            p.setColor(p.WindowText, Qt::blue);
            num->setPalette(p);
        }
        else if (htv > 0 && val <= htv) {
            p.setColor(p.WindowText, Qt::black);
            num->setPalette(p);
        }
        else {
            p.setColor(p.WindowText, Qt::red);
            num->setPalette(p);
        }
        QString v = QString("%1").arg(val, 3, 'f', 1);
        num->display(v);
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
    static int c = 0;
    QPalette p;
    
    QLabel *n = new QLabel(this);
    switch (c) {
        case 0:
            p.setColor(QPalette::Foreground, Qt::red);
            c++;
            break;
        case 1:
            p.setColor(QPalette::Foreground, Qt::blue);
            c++;
            break;
        case 2:
            p.setColor(QPalette::Foreground, Qt::darkGreen);
            c++;
            break;

    }
    n->setPalette(p);
    
    QLCDNumber *v = new QLCDNumber(this);
    LineSeries *ls = new LineSeries(width(), name);
    n->setText(name);
    n->setAlignment(Qt::AlignCenter);
    n->setMaximumHeight(50);
    v->setDigitCount(5);
    v->setSegmentStyle(QLCDNumber::Flat);
    v->display(0.0);
    m_layout->addWidget(n, 1, which - 1);
    m_layout->addWidget(v, 2, which - 1);
    m_probeNames[name] = n;
    m_probeValues[name] = v;
    m_series[name] = ls;
    
    QFont f = n->font();
    f.setPixelSize(n->height() * 1.2);
    n->setFont(f);
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

        m_layout->addWidget(m_graph, 3, 0, 1, 3);
        m_graph->setMax(m_maxGraphTemp);
        m_graph->setMin(m_minGraphTemp);
    }
}

