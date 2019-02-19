/**
 * HeaterMeter
 * restapi.cpp
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


#include "restapi.h"

RestAPI::RestAPI(QString ip, QObject *parent) : QObject(parent), m_host(ip)
{
    m_manager = new QNetworkAccessManager(this);
    m_update = new QTimer(this);
    connect(m_update, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(commandFinished(QNetworkReply*)));
}

RestAPI::~RestAPI()
{
    m_update->stop();
    delete m_update;
    delete m_manager;
}

void RestAPI::run()
{
    m_update->setInterval(1000);
    m_update->start();
}

void RestAPI::decodeVersion(QJsonObject json)
{
    if (json.contains("api")) {
        m_apiVersion = json.value("api").toInt();
        emit apiVersion(m_apiVersion);
    }
    if (json.contains("ucid")) {
        m_version = json.value("ucid").toString();
        emit firmwareVersion(m_version);
    }
}

void RestAPI::decodeConfig(QJsonObject json)
{
    if (json.contains("pn1")) {
        emit probeFound(RA_PROBE1, json.value("pn1").toString());
    }
    if (json.contains("pn2")) {
        emit probeFound(RA_PROBE2, json.value("pn2").toString());
    }
    if (json.contains("pn3")) {
        emit probeFound(RA_PROBE3, json.value("pn3").toString());
    }
}

void RestAPI::decodeStatus(QJsonObject json)
{
    if (json.contains("temps") && json["temps"].isArray()) {
        QJsonArray probe = json["temps"].toArray();
        for (int i = 0; i < probe.size(); ++i) {
            QJsonObject o = probe[i].toObject();
            if (o.contains("n") && o.contains("c")) {
                emit statusUpdate(o["n"].toString(), o["c"].toDouble());
            }
            if (o.contains("a")) {
                QJsonObject a = o["a"].toObject();
                int h = a["h"].toInt();
                int l = a["l"].toInt();

                if (l > 0) {
                    emit lowTrigger(o["n"].toString(), l);
                }
                if (h > 0) {
                    emit highTrigger(o["n"].toString(), h);
                }
            }
        }
    }
}

void RestAPI::commandFinished(QNetworkReply *reply)
{
    QJsonDocument rval; 
    QByteArray ba;
    QJsonParseError error;
    QByteArray text = reply->rawHeader("content-type");
    
    if (text != "application/json") {
        qWarning() << __PRETTY_FUNCTION__ << ": Got non JSON back from the server";
        return;
    }
    
    if (!reply->error()) {
        ba = reply->readAll();
        rval = QJsonDocument::fromJson(ba, &error);
        switch (m_which) {
            case RA_VERSION:
                if (!rval.isEmpty()) {
                    decodeVersion(rval.object());
                }
                else {
                    qDebug() << __PRETTY_FUNCTION__ << error.errorString();
                }
                break;
            case RA_CONFIG:
                if (!rval.isEmpty()) {
                    decodeConfig(rval.object());
                }
                break;
            case RA_STATUS:
                if (!rval.isEmpty()) {
                    decodeStatus(rval.object());
                }
                break;
            default:
                qDebug() << __PRETTY_FUNCTION__ << ": which is unknown" << m_which;
        }
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << ": Reply error";
    }
}

bool RestAPI::spinLock()
{
    int count = 0;
    
    if (!m_mutex.tryLock()) {
        if (count++ > 10)
            return false;
        QThread::msleep(50);
    }
    return true;
}

void RestAPI::getVersion()
{
    if (!spinLock())
        return;
    
    QUrl url(QString("http://" + m_host + "/luci/lm/api/version"));
    QNetworkRequest request(url);
    
    m_which = RA_VERSION;
    m_manager->get(request);
}

void RestAPI::getConfig()
{
    if (!spinLock())
        return;
    
    QUrl url(QString("http://" + m_host + "/luci/lm/api/config"));
    QNetworkRequest request(url);
    
    m_which = RA_CONFIG;
    m_manager->get(request);
}

void RestAPI::timeout()
{
    /* If something else is going on, just skip this update */
    if (!spinLock())
        return;
    
    QUrl url(QString("http://" + m_host + "/luci/lm/api/status"));
    QNetworkRequest request(url);
    
    m_which = RA_STATUS;
    m_manager->get(request);
}

