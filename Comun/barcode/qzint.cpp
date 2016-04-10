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

#include "qzint.h"
#include <QString>
#include <stdio.h>

#define	FACTOR_PIXELS_PUNTO		(float) 5.0039

namespace Zint
{

static const qreal maxi_diagonal=11;
static const qreal maxi_width=1.73205807568877*maxi_diagonal/2;
//static const char* fontstyle="Arial";
/*static const int fontPixelSizeSmall=6;
static const int fontPixelSizeLarge=8;*/
/*JORGE
static const int fontPixelSizeSmall=9;
static const int fontPixelSizeLarge=10;
*/

QZint::QZint()
{
	m_symbol=BARCODE_CODE128;
    m_height=50;
	m_border=NO_BORDER;
	m_borderWidth=1;
	m_securityLevel=-1;
	m_pdf417CodeWords=928;
	m_fgColor=Qt::black;
	m_bgColor=Qt::white;
	m_zintSymbol=0;
	m_error=0;
	m_input_mode = UNICODE_MODE;
	m_scale = 1.0;
	m_option_3 = 0;
    m_hidetext = false;

    fontPixelSizeSmall=9;//JORGE
    fontPixelSizeLarge=10;//JORGE
    fontstyle=QString("Arial");//JORGE
}

QZint::~QZint()
{
	if (m_zintSymbol)
		ZBarcode_Delete(m_zintSymbol);
}

void QZint::encode()
{
	if (m_zintSymbol)
		ZBarcode_Delete(m_zintSymbol);

	m_lastError.clear();
	m_zintSymbol = ZBarcode_Create();
	m_zintSymbol->output_options=m_border;
	m_zintSymbol->symbology=m_symbol;
    m_zintSymbol->height=m_height;
	m_zintSymbol->whitespace_width=m_whitespace;
	m_zintSymbol->border_width=m_borderWidth;
	m_zintSymbol->option_1=m_securityLevel;
	m_zintSymbol->input_mode = m_input_mode;
    m_zintSymbol->option_2=m_width;
	if(m_hidetext) {
		m_zintSymbol->show_hrt = 0;
	} else {
		m_zintSymbol->show_hrt = 1;
	}
	if(m_symbol == BARCODE_PDF417) {
		m_zintSymbol->option_3=m_pdf417CodeWords;
	} else {
		m_zintSymbol->option_3 = m_option_3;
	}
	QByteArray bstr=m_text.toUtf8();
#if QT_VERSION >= 0x050000
    QByteArray pstr=m_primaryMessage.left(99).toLatin1();
#else
    QByteArray pstr=m_primaryMessage.left(99).toAscii();
#endif
	strcpy(m_zintSymbol->primary,pstr.data());
	int error = ZBarcode_Encode(m_zintSymbol, (unsigned char*)bstr.data(), bstr.length());
	if (error > ZWARN_INVALID_OPTION)
		m_lastError=m_zintSymbol->errtxt;

	if (m_zintSymbol->symbology == BARCODE_MAXICODE)
		m_zintSymbol->height = 33;
	
	switch(m_zintSymbol->output_options) {
		case 0: m_border = NO_BORDER; break;
		case 2: m_border = BIND; break;
		case 4: m_border = BOX; break;
	}
	m_borderWidth = (BorderType)m_zintSymbol->border_width;
	m_whitespace = m_zintSymbol->whitespace_width;
}

int  QZint::symbol()
{
	return m_symbol;
}
void QZint::setSymbol(int symbol)
{
	m_symbol=symbol;
}

int QZint::textAlignment(){
    return this->m_textAlignment;
}

void QZint::setTextAlignment(int flags){
    this->m_textAlignment=flags;
}

void QZint::setInputMode(int input_mode)
{
	m_input_mode = input_mode;
}

int QZint::getPixmapWidth(){
    int width=m_zintSymbol->bitmap_width;
    return width;
}

int QZint::getPixmapHeight(){
    int height=m_zintSymbol->bitmap_height;
    return height;
}

void QZint::setfontPixelSizeSmall(int sizeSmall)
{
    fontPixelSizeSmall=sizeSmall;
}

void QZint::setfontPixelSizeLarge(int sizeLarge)
{
    fontPixelSizeLarge=sizeLarge;
}

void QZint::setfontType(QString fontType)
{
    fontstyle=QString("%1").arg(fontType);
}

void QZint::setforegroundColor(QColor ColorFg)
{
    m_fgColor=ColorFg;
}

void QZint::setbackgroundColor(QColor ColorBg)
{
    m_bgColor=ColorBg;
}

/*void QZint::setaspectRatio(Zint::QZint::AspectRatioMode ratio)
{
    ar=ratio;
}*/

QString QZint::text()
{
	return m_text;
}
void QZint::setText(const QString & text)
{
	m_text=text;
}

QString QZint::primaryMessage()
{
	return m_primaryMessage;
}
void QZint::setPrimaryMessage(const QString & primaryMessage)
{
	m_primaryMessage=primaryMessage;
}

int QZint::height()
{
	encode();
	return (m_zintSymbol->height+(m_border!=NO_BORDER)?m_borderWidth*2:0)*(m_zintSymbol->symbology == BARCODE_MAXICODE?(maxi_width+1):1);
}

void QZint::setHeight(int height)
{
	m_height=height;
}

void QZint::setWidth(int width)
{
	m_width=width;
}

void QZint::setOption3(int option)
{
	m_option_3 = option;
}

int QZint::width()
{
	encode();
	return (m_zintSymbol->width+(m_border==BOX)?m_borderWidth*2:0)*(m_zintSymbol->symbology == BARCODE_MAXICODE?(maxi_width+1):1);
}

float QZint::scale()
{
	return m_scale;
}

void QZint::setScale(float scale)
{
	m_scale = scale;
}

QColor QZint::fgColor()
{
	return m_fgColor;
}
void QZint::setFgColor(const QColor & fgColor)
{
	m_fgColor=fgColor;
}

QColor QZint::bgColor()
{
	return m_bgColor;
}
void QZint::setBgColor(const QColor & bgColor)
{
	m_bgColor=bgColor;
}

QZint::BorderType QZint::borderType()
{
	return m_border;
}
void QZint::setBorderType(BorderType border)
{
	m_border=border;
}

int QZint::borderWidth()
{
	return m_borderWidth;
}
void QZint::setBorderWidth(int boderWidth)
{
	if (boderWidth<1 || boderWidth>16)
		boderWidth=1;
	m_borderWidth=boderWidth;
}

void QZint::setWhitespace(int whitespace)
{
	m_whitespace = whitespace;
}

int QZint::pdf417CodeWords()
{
	return m_pdf417CodeWords;
}
void QZint::setPdf417CodeWords(int pdf417CodeWords)
{
	m_pdf417CodeWords=pdf417CodeWords;
}

int QZint::securityLevel()
{
	return m_securityLevel;
}
void QZint::setSecurityLevel(int securityLevel)
{
	m_securityLevel=securityLevel;
}

QString QZint::error_message()
{
	return m_lastError;
}

int QZint::mode()
{
	return m_securityLevel;
}
void QZint::setMode(int securityLevel)
{
	m_securityLevel=securityLevel;
}

void QZint::setHideText(bool hide)
{
	m_hidetext = hide;
}

void QZint::setOption2(int option2)
{
    m_width=option2;
}

bool QZint::save_to_file(QString filename)
{
	if (m_zintSymbol)
		ZBarcode_Delete(m_zintSymbol);
	
	QString fg_colour_hash = m_fgColor.name();
	QString bg_colour_hash = m_bgColor.name();

	m_lastError.clear();
	m_zintSymbol = ZBarcode_Create();
	m_zintSymbol->output_options=m_border;
	m_zintSymbol->symbology=m_symbol;
	m_zintSymbol->height=m_height;
	m_zintSymbol->whitespace_width=m_whitespace;
	m_zintSymbol->border_width=m_borderWidth;
	m_zintSymbol->option_1=m_securityLevel;
	m_zintSymbol->input_mode = m_input_mode;
	m_zintSymbol->option_2=m_width;
	if(m_hidetext) {
		m_zintSymbol->show_hrt = 0;
	} else {
		m_zintSymbol->show_hrt = 1;
	}
	if(m_symbol == BARCODE_PDF417) {
		m_zintSymbol->option_3=m_pdf417CodeWords;
	} else {
		m_zintSymbol->option_3 = m_option_3;
	}
	m_zintSymbol->scale=m_scale;
#if QT_VERSION >= 0x050000
    QByteArray bstr=m_text.toUtf8();
    QByteArray pstr=m_primaryMessage.left(99).toLatin1();
    QByteArray fstr=filename.left(255).toLatin1();
	strcpy(m_zintSymbol->primary,pstr.data());
	strcpy(m_zintSymbol->outfile,fstr.data());
    QByteArray fgcol=fg_colour_hash.right(6).toLatin1();
    QByteArray bgcol=bg_colour_hash.right(6).toLatin1();
#else
    QByteArray bstr=m_text.toUtf8();
    QByteArray pstr=m_primaryMessage.left(99).toAscii();
    QByteArray fstr=filename.left(255).toAscii();
    strcpy(m_zintSymbol->primary,pstr.data());
    strcpy(m_zintSymbol->outfile,fstr.data());
    QByteArray fgcol=fg_colour_hash.right(6).toAscii();
    QByteArray bgcol=bg_colour_hash.right(6).toAscii();
#endif

    strcpy(m_zintSymbol->fgcolour,fgcol.data());
	strcpy(m_zintSymbol->bgcolour,bgcol.data());
	int error = ZBarcode_Encode(m_zintSymbol, (unsigned char*)bstr.data(), bstr.length());
	if (error > ZWARN_INVALID_OPTION)
		m_lastError=m_zintSymbol->errtxt;
	error = ZBarcode_Print(m_zintSymbol, 0);
	if (error > ZWARN_INVALID_OPTION)
		m_lastError=m_zintSymbol->errtxt;
	if(error == 0) { return true; } else { return false; }
}

int QZint::module_set(int y_coord, int x_coord)
{
	int x_char, x_sub, result;
	
	x_char = x_coord / 7;
	x_sub = x_coord % 7;
	result = 0;
	
	switch(x_sub) {
		case 0: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x01) != 0) { result = 1; } break;
		case 1: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x02) != 0) { result = 1; } break;
		case 2: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x04) != 0) { result = 1; } break;
		case 3: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x08) != 0) { result = 1; } break;
		case 4: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x10) != 0) { result = 1; } break;
		case 5: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x20) != 0) { result = 1; } break;
		case 6: if((m_zintSymbol->encoded_data[y_coord][x_char] & 0x40) != 0) { result = 1; } break;
	}
	
	return result;
}

void QZint::render(QPainter & painter, const QRectF & paintRect, AspectRatioMode mode)
{
	encode();
	bool textdone;
	int comp_offset = 0, xoffset = m_whitespace, j, main_width = 0, addon_text_height = 0;
	int yoffset = 0;
	QString caption = QString::fromUtf8((const char *)m_zintSymbol->text, -1);
	QFont fontSmall(fontstyle);
    //fontSmall.setPixelSize(fontPixelSizeSmall);
    fontSmall.setPointSize(fontPixelSizeSmall);
    fontSmall.setWeight(QFont::Normal);
    QFont fontLarge(fontstyle);
	fontLarge.setPixelSize(fontPixelSizeLarge);




	if (m_lastError.length())
	{
		painter.setFont(fontLarge);
		painter.drawText(paintRect,Qt::AlignCenter,m_lastError);
		return;
	}

	painter.save();
    painter.setClipRect(paintRect,Qt::IntersectClip);


    qreal xtr=paintRect.x();
    qreal ytr=paintRect.y();

	int zrow_height=m_zintSymbol->height;
	int zrows=0;
	for (int i=0;i<m_zintSymbol->rows;i++)
	{
		zrow_height-=m_zintSymbol->row_height[i];
		if (!m_zintSymbol->row_height[i])
			zrows++;
	}
	if (zrows)
	{
		zrow_height/=zrows;
		for (int i=0;i<m_zintSymbol->rows;i++)
			if (!m_zintSymbol->row_height[i])
				m_zintSymbol->row_height[i]=zrow_height;
	}
	else
		m_zintSymbol->height-=zrow_height;


	qreal gwidth=m_zintSymbol->width;
	qreal gheight=m_zintSymbol->height;
	if (m_zintSymbol->symbology == BARCODE_MAXICODE)
	{
		gheight*=(maxi_width);
		gwidth*=(maxi_width+1);
	}

	qreal xsf=1;
	qreal ysf=1;
	qreal textoffset = 0;

	gwidth+=((m_border==BOX)?m_borderWidth*2:0);
	gheight+=((m_border!=NO_BORDER)?m_borderWidth*2:0);
	if(QString((const char*)m_zintSymbol->text).isEmpty() == false) {
        textoffset = fontPixelSizeSmall;//Antes estaba en 9
		gheight += textoffset;
	} else {
		textoffset = 0;
	}
	gwidth+=m_zintSymbol->whitespace_width*2;
	switch(mode)
	{
        //Modificamos el reescalado si hay que pintar texto
		case IgnoreAspectRatio:
				xsf=(qreal)paintRect.width()/gwidth;
                if (!m_hidetext) ysf=((qreal)paintRect.height()-fontPixelSizeSmall*FACTOR_PIXELS_PUNTO)/gheight;
                else ysf=(qreal)paintRect.height()/gheight;
			break;

		case KeepAspectRatio:
			if (paintRect.width()/gwidth<paintRect.height()/gheight)
			{
                if (!m_hidetext){
                    ysf=xsf=((qreal)paintRect.width()-fontPixelSizeSmall)/gwidth;
                    ytr+=(qreal)(paintRect.height()-gheight*ysf-fontPixelSizeSmall*FACTOR_PIXELS_PUNTO)/2;
                }
                else {
                    ysf=xsf=(qreal)paintRect.width()/gwidth;
                    ytr+=(qreal)(paintRect.height()-gheight*ysf)/2;
                }


			}
			else
			{
                if (!m_hidetext) ysf=xsf=((qreal)paintRect.height()-fontPixelSizeSmall*FACTOR_PIXELS_PUNTO)/gheight;
                else ysf=xsf=((qreal)paintRect.height())/gheight;
				xtr+=(qreal)(paintRect.width()-gwidth*xsf)/2;

			}
			break;

		case CenterBarCode:
				xtr+=((qreal)paintRect.width()-gwidth*xsf)/2;
                if (!m_hidetext) ytr+=((qreal)paintRect.height()-gheight*ysf-fontPixelSizeSmall*FACTOR_PIXELS_PUNTO)/2;
                else ytr+=((qreal)paintRect.height()-gheight*ysf)/2;

			break;
	}

	painter.setBackground(QBrush(m_bgColor));
	painter.fillRect(paintRect,QBrush(m_bgColor));
    painter.translate(xtr,ytr);
    painter.scale(xsf,ysf);

	QPen p;
	p.setColor(m_fgColor);
	p.setWidth(m_borderWidth);
	painter.setPen(p);

	QPainterPath pt;
	if(m_zintSymbol->symbology != BARCODE_MAXICODE) {

		switch(m_border)
		{
			case BOX:
                painter.fillRect(0,m_borderWidth,m_borderWidth,m_zintSymbol->height,QBrush(m_bgColor));
                painter.fillRect(m_zintSymbol->width + xoffset + xoffset + m_borderWidth,m_borderWidth,m_borderWidth,m_zintSymbol->height,QBrush(m_bgColor));
                painter.fillRect(0,0,m_zintSymbol->width + xoffset + xoffset + m_borderWidth + m_borderWidth,m_borderWidth,QBrush(m_bgColor));
                painter.fillRect(0,m_zintSymbol->height + m_borderWidth,m_zintSymbol->width + xoffset + xoffset + m_borderWidth + m_borderWidth, m_borderWidth,QBrush(m_bgColor));
				painter.translate(m_borderWidth+m_zintSymbol->whitespace_width,m_borderWidth);
				yoffset = m_borderWidth;
				break;
			case BIND:
                painter.fillRect(0,0,m_zintSymbol->width + xoffset + xoffset,m_borderWidth,QBrush(m_bgColor));
                painter.fillRect(0,m_zintSymbol->height + m_borderWidth,m_zintSymbol->width + xoffset + xoffset, m_borderWidth,QBrush(m_bgColor));
				painter.translate(m_zintSymbol->whitespace_width,m_borderWidth);
				yoffset = m_borderWidth;
				break;
	
			default:
				painter.translate(m_zintSymbol->whitespace_width,0);
				break;;
		}
	}

	while(!(module_set(m_zintSymbol->rows - 1, comp_offset))) {
		comp_offset++;
	}
	xoffset = comp_offset;
	

	main_width = m_zintSymbol->width;
	if ((((m_zintSymbol->symbology == BARCODE_EANX) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_EANX_CC))
		|| (m_zintSymbol->symbology == BARCODE_ISBNX)) {
		switch(caption.size()) {
            case 13:
			case 16:
			case 19:
				if(m_zintSymbol->whitespace_width == 0) {
					m_zintSymbol->whitespace_width = 10;
				}
				main_width = 96 + comp_offset;
				break;
			default:
				main_width = 68 + comp_offset;
				break;
		}
	}
	
	if (((m_zintSymbol->symbology == BARCODE_UPCA) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_UPCA_CC)) {
		if(m_zintSymbol->whitespace_width == 0) {
			m_zintSymbol->whitespace_width = 10;
		}
		main_width = 96 + comp_offset;
	}
	
	if (((m_zintSymbol->symbology == BARCODE_UPCE) && (m_zintSymbol->rows == 1)) || (m_zintSymbol->symbology == BARCODE_UPCE_CC)) {
		if(m_zintSymbol->whitespace_width == 0) {
			m_zintSymbol->whitespace_width = 10;
		}
		main_width = 51 + comp_offset;
	}
	
	p.setWidth(1);
	painter.setPen(p);

	if (m_zintSymbol->symbology == BARCODE_MAXICODE)
	{

		painter.save();
        painter.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);
		for (int r=0;r<m_zintSymbol->rows;r++)
		{
			for (int c=0;c<m_zintSymbol->width;c++)
			{
				if (module_set(r, c))
				{
					qreal col=(qreal)c*(maxi_width+1)+(r%2)*((maxi_width+1)/2);
					qreal row=(qreal)r*(maxi_width+1)*0.868;
					QPainterPath pt;
					pt.moveTo(col+maxi_width/2, 	row);
					pt.lineTo(col+maxi_width, 	row+maxi_diagonal/4);
					pt.lineTo(col+maxi_width, 	row+(maxi_diagonal-maxi_diagonal/4));
					pt.lineTo(col+maxi_width/2, 	row+maxi_diagonal);
					pt.lineTo(col, 			row+(maxi_diagonal-maxi_diagonal/4));
					pt.lineTo(col, 			row+maxi_diagonal/4);
					pt.lineTo(col+maxi_width/2, 	row);
					painter.fillPath(pt,QBrush(m_fgColor));
				}
			}
		}
		p.setWidth(maxi_width);
		painter.setPen(p);
		const qreal w=maxi_width+1;
		painter.drawEllipse(QPointF(14.5*w,16.5*w*0.868),w,w);
		painter.drawEllipse(QPointF(14.5*w,16.5*w*0.868),w+w*1.5,w+w*1.5);
		painter.drawEllipse(QPointF(14.5*w,16.5*w*0.868),w+w*3,w+w*3);
		painter.restore();
	}
	else
	{

		int y=0;
		for (int row=0;row<m_zintSymbol->rows;row++)
		{
			for (int i=0;i<m_zintSymbol->width;i++) {
				if (module_set(row, i))
				{
					int ed = module_set(row, i);
					int linewidth=0;
					for (int j=i;j<m_zintSymbol->width;j++,linewidth++)
						if (ed != module_set(row, j))
							break;
					QColor color;
					color=m_fgColor;

					if(!((i > main_width) && (row == m_zintSymbol->rows - 1)))  {
						painter.fillRect(i,y,linewidth,m_zintSymbol->row_height[row],QBrush(color));
					} else {
						painter.fillRect(i,y + 8,linewidth,m_zintSymbol->row_height[row] - 3,QBrush(color));
						addon_text_height = y;
					}
				}
			}

			if(((m_zintSymbol->symbology == BARCODE_CODE16K) || (m_zintSymbol->symbology == BARCODE_CODE49)) && (row != 0)) {
				painter.fillRect(0,y - 1,m_zintSymbol->width,2,QBrush(m_fgColor));
			}
			y+=m_zintSymbol->row_height[row];
		}
	}

	textdone = false;
	
	if(m_hidetext == false) {
		painter.setFont(fontSmall);
		if(((m_zintSymbol->symbology == BARCODE_EANX) || (m_zintSymbol->symbology == BARCODE_EANX_CC)) ||
			(m_zintSymbol->symbology == BARCODE_ISBNX)) {

			switch(caption.size()) {
				case 8:
				case 11:
				case 14:
                    painter.fillRect(0 + xoffset,m_zintSymbol->height,1,40,QBrush(m_fgColor));
                    painter.fillRect(2 + xoffset,m_zintSymbol->height,1,40,QBrush(m_fgColor));
                    painter.fillRect(32 + xoffset,m_zintSymbol->height,1,40,QBrush(m_fgColor));
                    painter.fillRect(34 + xoffset,m_zintSymbol->height,1,40,QBrush(m_fgColor));
                    painter.fillRect(64 + xoffset,m_zintSymbol->height,1,40,QBrush(m_fgColor));
                    painter.fillRect(66 + xoffset,m_zintSymbol->height,1,40,QBrush(m_fgColor));
					painter.setFont(fontLarge);
					painter.drawText(3 + xoffset, m_zintSymbol->height + yoffset, 29, 9,Qt::AlignCenter, caption.mid(0,4));
					painter.drawText(35 + xoffset, m_zintSymbol->height + yoffset, 29, 9,Qt::AlignCenter, caption.mid(4,4));
                    if(caption.size() == 11) {  painter.drawText(76 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(9,2)); };
                    if(caption.size() == 14) {  painter.drawText(76 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(9,5)); };
					painter.setFont(fontSmall);
					textdone = true;
					break;
				case 13:
				case 16:
				case 19:
                    painter.fillRect(0 + xoffset,m_zintSymbol->height,1,25,QBrush(m_fgColor));
                    painter.fillRect(2 + xoffset,m_zintSymbol->height,1,25,QBrush(m_fgColor));
                    painter.fillRect(46 + xoffset,m_zintSymbol->height,1,25,QBrush(m_fgColor));
                    painter.fillRect(48 + xoffset,m_zintSymbol->height,1,25,QBrush(m_fgColor));
                    painter.fillRect(92 + xoffset,m_zintSymbol->height,1,25,QBrush(m_fgColor));
                    painter.fillRect(94 + xoffset,m_zintSymbol->height,1,25,QBrush(m_fgColor));
                    /*painter.setFont(fontLarge);
					painter.drawText(xoffset - 7, m_zintSymbol->height + yoffset, 7, 9,Qt::AlignCenter, caption.mid(0,1));
					painter.drawText(3 + xoffset, m_zintSymbol->height + yoffset, 43, 9,Qt::AlignCenter, caption.mid(1,6));
					painter.drawText(49 + xoffset, m_zintSymbol->height + yoffset, 43, 9,Qt::AlignCenter, caption.mid(7,6));
                    if(caption.size() == 16) {  painter.drawText(104 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(14,2)); };
                    if(caption.size() == 19) {  painter.drawText(104 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(14,5)); };
                    painter.setFont(fontSmall);*/
					textdone = true;
					break;
			}
			if(textdone == false) {
				painter.setFont(fontLarge);
                painter.drawText(0, m_zintSymbol->height, m_zintSymbol->width, 9,Qt::AlignCenter, caption);
				painter.setFont(fontSmall);
				textdone = true;
			}
		}
		
		if((m_zintSymbol->symbology == BARCODE_UPCA) || (m_zintSymbol->symbology == BARCODE_UPCA_CC)) {

			int block_width;
			bool latch = true;
			
			j = 0 + comp_offset;
			do {
				block_width = 0;
				do {
					block_width++;
				} while (module_set(m_zintSymbol->rows - 1, j + block_width) == module_set(m_zintSymbol->rows - 1, j));
				if(latch == true) {

					painter.fillRect(j + xoffset - comp_offset,m_zintSymbol->height,block_width,5,QBrush(m_fgColor));
					latch = false;
				} else {

					latch = true;
				}
				j += block_width;
			} while (j < 11 + comp_offset);
			painter.fillRect(46 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
			painter.fillRect(48 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
			latch = true;
			j = 85 + comp_offset;
			do {
				block_width = 0;
				do {
					block_width++;
				} while (module_set(m_zintSymbol->rows - 1, j + block_width) == module_set(m_zintSymbol->rows - 1, j));
				if(latch == true) {

					painter.fillRect(j + xoffset - comp_offset,m_zintSymbol->height,block_width,5,QBrush(m_fgColor));
					latch = false;
				} else {

					latch = true;
				}
				j += block_width;
			} while (j < 96 + comp_offset);
			painter.drawText(xoffset - 7, m_zintSymbol->height + yoffset + 2, 7, 7,Qt::AlignCenter, caption.mid(0,1));
			painter.drawText(96 + xoffset, m_zintSymbol->height + yoffset + 2, 7, 7,Qt::AlignCenter, caption.mid(11,1));
			painter.setFont(fontLarge);
			painter.drawText(11 + xoffset, m_zintSymbol->height + yoffset, 35, 9,Qt::AlignCenter, caption.mid(1,5));
			painter.drawText(49 + xoffset, m_zintSymbol->height + yoffset, 35, 9,Qt::AlignCenter, caption.mid(6,5));
            if(caption.size() == 15) {  painter.drawText(104 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(13,2)); };
            if(caption.size() == 18) {  painter.drawText(104 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(13,5)); };
			painter.setFont(fontSmall);
			textdone = true;
		}
		
		if((m_zintSymbol->symbology == BARCODE_UPCE) || (m_zintSymbol->symbology == BARCODE_UPCE_CC)) {

			painter.fillRect(0 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
			painter.fillRect(2 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
			painter.fillRect(46 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
			painter.fillRect(48 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
			painter.fillRect(50 + xoffset,m_zintSymbol->height,1,5,QBrush(m_fgColor));
			painter.drawText(xoffset - 7, m_zintSymbol->height + yoffset + 2, 7, 7,Qt::AlignCenter, caption.mid(0,1));
			painter.drawText(51 + xoffset, m_zintSymbol->height + yoffset + 2, 7, 7,Qt::AlignCenter, caption.mid(7,1));
			painter.setFont(fontLarge);
			painter.drawText(3 + xoffset, m_zintSymbol->height + yoffset, 43, 9,Qt::AlignCenter, caption.mid(1,6));
            if(caption.size() == 11) {  painter.drawText(60 + xoffset, addon_text_height, 20, 9,Qt::AlignCenter, caption.mid(9,2)); };
            if(caption.size() == 14) {  painter.drawText(60 + xoffset, addon_text_height, 47, 9,Qt::AlignCenter, caption.mid(9,5)); };
			painter.setFont(fontSmall);
			textdone = true;
		}
    }
	
	if((m_hidetext == false) && (textdone == false)) {
        //painter.scale(paintRect.x(),paintRect.y());
/*        painter.setFont(fontSmall);
        painter.drawText(0, m_zintSymbol->height + yoffset+1, m_zintSymbol->width, fontPixelSizeSmall, Qt::AlignCenter, caption);*/

    }
    painter.restore();

    if((!m_hidetext)&&(!textdone)) {

        painter.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);
        fontSmall.setPixelSize(fontPixelSizeSmall*FACTOR_PIXELS_PUNTO);
        fontSmall.setStyleStrategy(QFont::PreferAntialias);

        painter.setFont(fontSmall);
        int offset=painter.device()->height()*ysf;
        int height=painter.device()->height()-offset;
        if (height<0) height=fontPixelSizeSmall;
        int flags=m_textAlignment|Qt::AlignTop;
        painter.drawText(0,painter.device()->height()*ysf, painter.device()->width(), height,flags, caption);

        //painter.drawText(0,painter.device()->height()*ysf, painter.device()->width(), height,Qt::AlignHCenter|Qt::AlignTop, caption);


    }

}




const QString & QZint::lastError()
{
	return m_lastError;
}

bool QZint::hasErrors()
{
	return m_lastError.length();
}

}

