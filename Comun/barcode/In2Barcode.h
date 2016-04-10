/***************************************************************************
 *   Copyright (C) 2008 by BogDan Vatra                                    *
 *   bogdan@licentia.eu                                                    *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/
#ifndef IN2BARCODE_H
#define IN2BARCODE_H

#include <QGraphicsItem>
#include <QPixmap>
#include "qzint.h"


class In2Barcode : public QGraphicsItem
{
public:
    In2Barcode();
    In2Barcode(int width,int height);
    ~In2Barcode();
	QRectF boundingRect() const;
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
    void SetPixmap();
    void SetPixmap(QString text);
    void SetSymbol(int symbol);


private:
    void setDefault();

public:
	mutable Zint::QZint bc;
	int w,h;
	Zint::QZint::AspectRatioMode ar;
    QPixmap qpxImagen;
};

#endif
