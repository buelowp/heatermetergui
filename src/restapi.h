/**
 * HeaterMeter
 * main.cpp
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



#ifndef RESTAPI_H
#define RESTAPI_H

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

#define RA_VERSION  0
#define RA_STATUS   1
#define RA_CONFIG   2

#define RA_PROBE1   1
#define RA_PROBE2   2
#define RA_PROBE3   3

/**
 * @todo write docs
 */
class RestAPI : public QObject
{
    Q_OBJECT
public:
    RestAPI(QString, QObject *parent = 0);
    ~RestAPI();
    
    void getVersion();
    void getConfig();
    void run();

public slots:
    void commandFinished(QNetworkReply*);
    void timeout();
    
signals:
    void statusUpdate(QString, double);
    void probeFound(int, QString);
    void apiVersion(int);
    void firmwareVersion(QString&);
    void lowTrigger(QString, int);
    void highTrigger(QString, int);
    
private:
    void decodeVersion(QJsonObject);
    void decodeConfig(QJsonObject);
    void decodeStatus(QJsonObject);
    bool spinLock();
    
    QNetworkAccessManager *m_manager;
    QString m_version;
    QMutex m_mutex;
    QTimer *m_update;
    QString m_host;
    int m_apiVersion;
    int m_which;
};

#endif // RESTAPI_H
