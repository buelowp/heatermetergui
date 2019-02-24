/**
 * HeaterMeter
 * tempgraph.cpp
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

#include "tempgraph.h"

TempGraph::TempGraph(QWidget *parent) : QWidget(parent)
{
    m_max = 400;
    m_min = 50;
    m_firstPaint = true;
    QPalette pal = palette();

    // set black background
    pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);
}

TempGraph::~TempGraph()
{
}

void TempGraph::addLineSeries(QString name, LineSeries *ls)
{
    if (ls) {
        m_lines[name] = ls;
        connect(ls, SIGNAL(update()), this, SLOT(graphUpdate()));
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << ": LineSeries pointer is NULL";
    }
}

void TempGraph::showEvent(QShowEvent*)
{
    QMapIterator<QString, LineSeries*> i(m_lines);
    while (i.hasNext()) {
        i.next();
        LineSeries* ls = i.value();
        ls->setWidth(width());
    }
}

void TempGraph::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QColor color;
    QColor green(Qt::darkGreen);
    int which = 0;
    int r;
    int b;
    int g;
    
    QMapIterator<QString, LineSeries*> i(m_lines);
    while (i.hasNext()) {
        i.next();
        LineSeries* ls = i.value();
        ls->resetIterator();
        switch (which) {
            case 0:
                r = 255;
                b = 0;
                g = 0;
                which++;
                break;
            case 1:
                r = 0;
                b = 255;
                g = 0;
                which++;
                break;
            case 2:
                r = green.red();
                b = green.blue();
                g = green.green();
                which++;
                break;
            default:
                r = 255;
                b = 255;
                g = 255;
                break;
        }

        color.setRgb(r, b, g);
        painter.setPen(color);
        painter.setBrush(Qt::SolidPattern);
        while (ls->hasNext()) {
            QPoint p = ls->next();
            int xoffset = width() * .1;
            int x = p.x() + xoffset;
            QPoint newp(x, p.y());
            qDebug() << __PRETTY_FUNCTION__ << ": QPoint at" << newp;
            if (p.y() > m_min && p.y() < m_max) {
                if (!ls->hasNext()) {
                    painter.drawEllipse(normalize(newp, m_min, m_max), 3, 3);
                }
                else {
                    painter.drawPoint(normalize(newp, m_min, m_max));
                }
            }
        }
    }
    
    painter.setPen(Qt::black);
    QFont f = painter.font();
    f.setFamily("Roboto");
    f.setPixelSize(25);
    painter.setFont(f);
    painter.setBrush(Qt::SolidPattern);
    painter.drawText(10, height() - 20, QString("%1").arg(m_min));
    painter.drawText(10, 60, QString("%1").arg(m_max));
}

QPoint TempGraph::normalize(QPoint p, double min, double max)
{
    double a = 0;
    double b = (double)height();
    double normal = ((b - a) * ((p.y() - min) / (max - min))) - a;
    return QPoint(p.x(), (height() - (int)normal));
}

void TempGraph::graphUpdate()
{
    repaint();
}

