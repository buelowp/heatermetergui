/*
 * Copyright (c) 2019 <copyright holder> <email>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "timerlabel.h"

TimerLabel::TimerLabel(QLabel *parent) : QLabel(parent)
{
    m_elapsed.setHMS(0, 0, 0);

    m_timer = new QTimer();
    m_timer->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

TimerLabel::~TimerLabel()
{
}

void TimerLabel::timeout()
{
    m_elapsed = m_elapsed.addSecs(1);
    setText(m_elapsed.toString("h:mm:ss"));
}

void TimerLabel::showEvent(QShowEvent*)
{
    setText(m_elapsed.toString("h:mm:ss"));
    qDebug() << __PRETTY_FUNCTION__ << ": height:" << height();
}

bool TimerLabel::event(QEvent* e)
{
    switch (e->type()) {
        case QEvent::TouchBegin:
            qDebug() << __PRETTY_FUNCTION__ << ": Touch Begin";
            e->setAccepted(true);
            break;
        case QEvent::TouchCancel:
            qDebug() << __PRETTY_FUNCTION__ << ": Touch Cancel";
            e->setAccepted(true);
            break;
        case QEvent::TouchEnd:
            qDebug() << __PRETTY_FUNCTION__ << ": Touch End";
            e->setAccepted(true);
            break;
        case QEvent::TouchUpdate:
            qDebug() << __PRETTY_FUNCTION__ << ": Touch Update";
            e->setAccepted(true);
            break;
        case QEvent::MouseButtonRelease:
            qDebug() << __PRETTY_FUNCTION__ << ": Mouse Button Release";
            e->setAccepted(true);
            break;
        case QEvent::MouseButtonPress:
            qDebug() << __PRETTY_FUNCTION__ << ": Mouse Button Press";
            e->setAccepted(true);
            break;
        default:
            break;
    }

    e->setAccepted(false);
    return e->isAccepted();
}
