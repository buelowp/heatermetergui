/**
 * HeaterMeter
 * lineseries.cpp
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

#include "lineseries.h"

LineSeries::LineSeries(int width, QString name, QObject *parent) : QObject(parent), m_width(width), m_name(name)
{
    m_index = 0;
    m_iterator = 0;
}

LineSeries::~LineSeries()
{
}

QPoint LineSeries::next()
{
    int y = m_queue[m_iterator];
    int x = m_iterator;
    
    m_iterator++;
    
    return QPoint(x, y);
}

bool LineSeries::hasNext()
{
    if (m_iterator < m_queue.size())
        return true;

    return false;
}

void LineSeries::enqueue(QPoint p)
{
    if (m_queue.size() >= m_width)
        m_queue.dequeue();

    m_queue.insert(p.x(), p.y());
    emit update();
}

void LineSeries::enqueue(int x, int y)
{
    if (m_queue.size() >= m_width)
        m_queue.dequeue();

    m_queue.insert(x, y);
    emit update();
}

void LineSeries::enqueue(int y)
{
    if (m_queue.size() >= m_width)
        m_queue.dequeue();

    m_queue.enqueue(y);
    emit update();
}

void LineSeries::dequeue()
{
    m_queue.dequeue();
    emit update();
}

void LineSeries::setWidth(int w)
{
    if (m_width > w && m_queue.size()) {
        for (int i = 0; i < (m_width - w); i++) {
            m_queue.dequeue();
        }
    }
}
