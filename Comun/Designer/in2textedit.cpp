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
#include "in2textedit.h"
#include <QFocusEvent>

In2TextEdit::In2TextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

/** Selecciona todo*/
void In2TextEdit::focusInEvent( QFocusEvent *e )
{
  if (e->gotFocus())
    selectAll();
  QTextEdit::focusInEvent(e);
}
