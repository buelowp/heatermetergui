/**
 * HeaterMeter
 * tempgraph.h
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

#ifndef TEMPGRAPH_H
#define TEMPGRAPH_H

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include "lineseries.h"
/**
 * @todo write docs
 */
class TempGraph : public QWidget
{
    Q_OBJECT
public:
    TempGraph(QWidget *parent = 0);
    ~TempGraph();
    
    void addLineSeries(QString, LineSeries*);
    void setMin(int t) { m_min = t; }
    void setMax(int t) { m_max = t; }
    
protected:
    void showEvent(QShowEvent*);
    void paintEvent(QPaintEvent*);

public slots:
    void graphUpdate();

private:
    QPoint normalize(QPoint, double, double);
    
    QMap<QString, LineSeries*> m_lines;
    int m_max;
    int m_min;
    bool m_firstPaint;
};

#endif // TEMPGRAPH_H
