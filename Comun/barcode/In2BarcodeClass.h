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
#ifndef IN2BARCODECLASS_H
#define IN2BARCODECLASS_H

#include <QGraphicsItem>
#include <QPixmap>
#include "qzint.h"
#include "MemoryMap.h"

class In2BarcodeClass : public QGraphicsItem
{
public:
    In2BarcodeClass();
    In2BarcodeClass(int width,int height);
    In2BarcodeClass(struct m_BarcodeParameters barcodeParameters);
    In2BarcodeClass(bool fijo, unsigned char NumCampo);
    //QPixmap In2BarcodeClass(bool fijo, int NumCampo,QString texto);
    ~In2BarcodeClass();
	QRectF boundingRect() const;
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
	
	void SetText(const QString & text);
	
	void SetWidth(int width);
	void SetHeight(int height);
	void SetInputMode(int input);
	void SetHideText(bool hide);
	
    void SetPixmap();
    void SetPixmap(QString text);
    void SetSymbol(int symbol);
	
	void SetScale(float scale);
	void SetFgColor(const QColor & fgColor);
	void SetBgColor(const QColor & bgColor);
	
	void SetBorderWidth(int boderWidth);
	void SetWhitespace(int whitespace);
    void SetBorderType(Zint::QZint::BorderType border);

    void SetOption2(int option2);
    void setParams(const struct m_BarcodeParameters & barcodeParameters);
    QString GetLastError();

    int pixmapWidth();
    int pixmapHeight();

    void SetFontSmall(int small_value);
    void SetFontLarge(int large_value);
    void SetFontType(QString fontType);

    void SetForeColor(QColor color_fg);
    void SetBackColor(QColor color_bg);

    void SetSecurityLevel(int level);

private:
    void setDefault();

public:
	mutable Zint::QZint bc;
	int w,h;
	Zint::QZint::AspectRatioMode ar;
    QPixmap qpxImagen;
};

#endif
