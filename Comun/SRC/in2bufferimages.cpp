/*  Copyright (C) 2013 In2 Printing Solutions <info@in2ps.com>
    Author: Jose Maria Martinez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "in2bufferimages.h"

In2BufferImages::In2BufferImages(QObject *parent) :
    QObject(parent)
{
}


struct In2VarImage In2BufferImages::at(int index){
    QMutexLocker lock(&m_mutex);
    struct In2VarImage rtn=m_buffer.at(index);
    return rtn;
}

void In2BufferImages::replace(int index, struct In2VarImage & element ){
    QMutexLocker lock(&m_mutex);
    m_buffer.replace(index,element);
}

void In2BufferImages::clear(){
    QMutexLocker lock(&m_mutex);
    m_buffer.clear();
}

void In2BufferImages::append(struct In2VarImage & element){
    QMutexLocker lock(&m_mutex);
    m_buffer.append(element);
}

int In2BufferImages::count(){
    QMutexLocker lock(&m_mutex);
    int rtn=m_buffer.count();
    return rtn;
}
