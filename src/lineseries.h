/**
 * HeaterMeter
 * lineseries.h
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

#ifndef LINESERIES_H
#define LINESERIES_H

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>

/**
 * @todo write docs
 */
class LineSeries : public QObject
{
    Q_OBJECT
public:
    LineSeries(int, QString, QObject *parent = 0);
    ~LineSeries();
    
    void enqueue(QPoint);
    void enqueue(int, int);
    void enqueue(int);
    void dequeue();
    int size() { return m_queue.size(); }
    QPoint next();
    bool hasNext();
    void resetIterator() { m_iterator = 0; }
    QString whoAmI() { return m_name; }
    void setWidth(int w);

signals:
    void update(QString);
    
private:
    QQueue<int> m_queue;
    QString m_name;
    int m_index;
    int m_iterator;
    int m_width;
};

#endif // LINESERIES_H
