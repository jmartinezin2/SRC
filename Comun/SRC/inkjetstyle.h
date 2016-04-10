/** @addtogroup inkjetstyle
 *  @{*/
/** @file inkjetstyle.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Fichero de estilo de la aplicación
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: inkjetstyle.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef InkjetSTYLE_H
#define InkjetSTYLE_H

#include "globals.h"

#if QT_VERSION >= 0x050000
#include <QProxyStyle>
class InkjetStyle : public QProxyStyle{
#else
#include <QWindowsStyle>
class InkjetStyle : public QWindowsStyle{
#endif

public:
    InkjetStyle();
    InkjetStyle(int theme);
    void setTheme(int theme);
    void drawHoverRect(QPainter *painter, const QRect &rect) const;

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget = 0) const;
//     void drawControl(ControlElement element, const QStyleOption *option,
//                      QPainter *painter, const QWidget *widget) const;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                            QPainter *painter, const QWidget *widget) const;
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const;

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                         SubControl sc, const QWidget *widget) const;

//     SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option,
// 				     const QPoint &pos, const QWidget *widget = 0) const;

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const;

    void polish(QPalette &palette);
    void polish(QWidget *widget);
    void unpolish(QWidget *widget);
};

#endif
