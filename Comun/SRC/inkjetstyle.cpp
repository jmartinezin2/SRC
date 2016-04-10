/** @addtogroup inkjetstyle
 *  @{*/
/** @file inkjetstyle.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Fichero de estilo de la aplicación
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: inkjetstyle.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#include "inkjetstyle.h"
#include "inkjetwidgets.h"
#include <QLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPixmapCache>
#include <QRadioButton>
#include <QString>
#include <QStyleOption>
#include <QtDebug>

static QColor qcolorIni;
static QColor qcolorFin;
static QColor qcolorMid;
static QColor qcolorFocusIni;
static QColor qcolorFocusFin;
static QColor qcolorFocusMid;
static int myTheme=THEME_NORMAL;

QPixmap cached(const QString &img)
{
    if (QPixmap *p = QPixmapCache::find(img))
        return *p;

    QPixmap pm;
    pm = QPixmap::fromImage(QImage(img), Qt::OrderedDither | Qt::OrderedAlphaDither);
    if (pm.isNull())
        return QPixmap();

    QPixmapCache::insert(img, pm);
    return pm;
}


InkjetStyle::InkjetStyle()
    : QWindowsStyle()
{
#ifdef __WINDOWS__
    Q_INIT_RESOURCE(shared);
#endif
    setTheme(THEME_NORMAL);
}

InkjetStyle::InkjetStyle(int theme)
    : QWindowsStyle()
{
#ifdef __WINDOWS__
    Q_INIT_RESOURCE(shared);
#endif
    setTheme(theme);
}


/** Cambiamos el tema global*/
void InkjetStyle::setTheme(int theme){
#ifdef  DEPURACION_IN2
    qcolorIni=QColor(180,180,180);
    qcolorFin=QColor(250,250,250);
    qcolorMid=QColor(215,215,215);
    qcolorFocusIni=QColor(150,200,255);
    qcolorFocusFin=QColor(205,225,255);
    qcolorFocusMid=QColor(175,210,255);
    myTheme=THEME_NORMAL;
#else
    switch (theme){
        case THEME_NORMAL:
            qcolorIni=QColor(180,180,180);
            qcolorFin=QColor(250,250,250);
            qcolorMid=QColor(215,215,215);
            qcolorFocusIni=QColor(150,200,255);
            qcolorFocusFin=QColor(205,225,255);
            qcolorFocusMid=QColor(175,210,255);
            break;
        case THEME_EMERGENCY:
            qcolorIni=QColor(253,208,196);
            qcolorFin=QColor(208,52,6);
            qcolorMid=QColor(230,130,91);

            qcolorFocusIni=QColor(150,200,255);
            qcolorFocusFin=QColor(205,225,255);
            qcolorFocusMid=QColor(175,210,255);
        break;
        case THEME_PRINTING:
            qcolorIni=QColor(218,244,215);
            qcolorFin=QColor(60,184,46);
            qcolorMid=QColor(135,214,130);

            qcolorFocusIni=QColor(150,200,255);
            qcolorFocusFin=QColor(205,225,255);
            qcolorFocusMid=QColor(175,210,255);
        break;
        case THEME_CLEANING:
            qcolorIni=QColor(180,180,180);
            qcolorFin=QColor(6,52,208);
            qcolorMid=QColor(91,130,230);
            qcolorFocusIni=QColor(255,200,150);
            qcolorFocusFin=QColor(255,225,205);
            qcolorFocusMid=QColor(255,210,175);
        break;
    }
     myTheme=theme;
#endif

}

void InkjetStyle::drawHoverRect(QPainter *painter, const QRect &r) const
{
    double h = r.height();
    double h2 = r.height() / 2.0;
    QPainterPath path;
    path.addRect(r.x() + h2, r.y() + 0, r.width() - h2 * 2, r.height());
    path.addEllipse(r.x(), r.y(), h, h);
    path.addEllipse(r.x() + r.width() - h, r.y(), h, h);
    path.setFillRule(Qt::WindingFill);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(191, 215, 191));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path);
}


void InkjetStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                QPainter *painter, const QWidget *widget) const
{

    Q_ASSERT(option);

    //Cambiamos el color de letra
    QWidget *parent=widget->parentWidget();
    if (parent){
        QPalette pal=parent->palette();
        pal.setColor(QPalette::WindowText,QColor(10,10,10));
        pal.setColor(QPalette::Text,QColor(10,10,10));
        parent->setPalette(pal);
    }

    switch (element) {
    case PE_FrameFocusRect:
        break;

    case PE_IndicatorRadioButton:
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            bool hover = (button->state & State_Enabled) && (button->state & State_MouseOver);
            painter->save();
            QPixmap radio;
            if (hover)
                drawHoverRect(painter, widget->rect());

            if (button->state & State_Sunken)
                radio = cached(":res/images/radiobutton-on.png");
            else if (button->state & State_On)
                radio = cached(":res/images/radiobutton_on.png");
            else
                radio = cached(":res/images/radiobutton_off.png");
            painter->drawPixmap(button->rect.topLeft(), radio);

            painter->restore();
        }
        break;
    //Con esto eliminamos el recuadrado del default button
    case PE_FrameDefaultButton:
        break;
    case PE_PanelButtonCommand:
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            //bool hover = (button->state & State_Enabled) && (button->state & State_MouseOver);

            const QRect &r = button->rect;

            const QPushButton *pushButton = qobject_cast<const QPushButton *>(widget);
            Q_ASSERT(pushButton);
            painter->save();
            QWidget *parent = pushButton->parentWidget();
            if (parent && qobject_cast<QGroupBox *>(parent)) {
                QLinearGradient lg(0, 0, 0, parent->height());
                lg.setColorAt(0, qcolorIni);
                //lg.setColorAt(0, QColor(18,18,18));
                lg.setColorAt(1, qcolorFin);
                painter->setPen(Qt::NoPen);
                painter->setBrush(lg);
                painter->setBrushOrigin(-widget->mapToParent(QPoint(0,0)));
                painter->drawRect(button->rect);
                painter->setBrushOrigin(0,0);
            }
                //bool down = (button->state & State_Sunken) || (button->state & State_On);


            int radius = 14;
            int radius2 = radius*2;
            QPainterPath clipPath;
            clipPath.moveTo(radius, 0);
            clipPath.arcTo(r.right() - radius2, 0, radius2, radius2, 90, -90);
            clipPath.arcTo(r.right() - radius2, r.bottom() - radius2, radius2, radius2, 0, -90);
            clipPath.arcTo(r.left(), r.bottom() - radius2, radius2, radius2, 270, -90);
            clipPath.arcTo(r.left(), r.top(), radius2, radius2, 180, -90);
            painter->setClipPath(clipPath);
            //QPixmap titleStretch = cached(":res/images/title_stretch.png");
            QPixmap titleStretch ;
            QPixmap topLeft;
            QPixmap topRight;
            QPixmap bottomLeft;
            QPixmap bottomRight;
            QPixmap leftStretch;
            QPixmap topStretch;
            QPixmap rightStretch;
            QPixmap bottomStretch;
            QLinearGradient lg(0, 0, 0, r.height());

            if (myTheme==THEME_NORMAL){
                titleStretch = cached(":res/images/groupframe_top_stretch.png");
                topLeft = cached(":res/images/groupframe_topleft.png");
                topRight = cached(":res/images/groupframe_topright.png");
                bottomLeft = cached(":res/images/groupframe_bottom_left.png");
                bottomRight = cached(":res/images/groupframe_bottom_right.png");
                leftStretch = cached(":res/images/groupframe_left_stretch.png");
                topStretch = cached(":res/images/groupframe_top_stretch.png");
                rightStretch = cached(":res/images/groupframe_right_stretch.png");
                bottomStretch = cached(":res/images/groupframe_bottom_stretch.png");
            }

            lg.setColorAt(0, qcolorIni);
            lg.setColorAt(1, /*qcolorFin*/qcolorMid);

            if (parent){
                QPalette pal = parent->palette();
                pal.setColor(QPalette::ButtonText,QColor(10,10,10));
                parent->setPalette(pal);
            }


            QFont InkjetFont=QFont( "Arial", 9, QFont::Black );

            if (pushButton->hasFocus()){
                if (pushButton->isDown()){
                   lg.setColorAt(0, qcolorFocusIni);
                   lg.setColorAt(1, qcolorFocusIni);
                }
                else{
                    lg.setColorAt(0, qcolorFocusIni);
                    lg.setColorAt(1, qcolorFocusFin);

                }
            }
            else{
                lg.setColorAt(1, qcolorFin);
            }
            painter->setFont(InkjetFont);
            painter->setPen(Qt::NoPen);
            painter->setBrush(lg);
            painter->drawRect(r.adjusted(0, titleStretch.height()/2, 0, 0));
            painter->setClipping(false);

            int topFrameOffset = titleStretch.height()/2 - 2;
            //int topFrameOffset2 = 0;
            //painter->drawPixmap();
            painter->drawPixmap(r.topLeft() + QPoint(0, topFrameOffset), topLeft);
            painter->drawPixmap(r.topRight() - QPoint(topRight.width()-1, 0)
                                + QPoint(0, topFrameOffset), topRight);
            painter->drawPixmap(r.bottomLeft() - QPoint(0, bottomLeft.height()-1), bottomLeft);
            painter->drawPixmap(r.bottomRight() - QPoint(bottomRight.width()-1,
                                bottomRight.height()-1), bottomRight);

            QRect left = r;
            left.setY(r.y() + topLeft.height() + topFrameOffset);
            left.setWidth(leftStretch.width());
            left.setHeight(r.height() - topLeft.height() - bottomLeft.height() - topFrameOffset);
            painter->drawTiledPixmap(left, leftStretch);

            QRect top = r;
            top.setX(r.x() + topLeft.width());
            top.setY(r.y() + topFrameOffset);
            top.setWidth(r.width() - topLeft.width() - topRight.width());
            top.setHeight(topLeft.height());
            painter->drawTiledPixmap(top, topStretch);

            QRect right = r;
            right.setX(r.right() - rightStretch.width()+1);
            right.setY(r.y() + topRight.height() + topFrameOffset);
            right.setWidth(rightStretch.width());
            right.setHeight(r.height() - topRight.height()
                            - bottomRight.height() - topFrameOffset);
            painter->drawTiledPixmap(right, rightStretch);

            QRect bottom = r;
            bottom.setX(r.x() + bottomLeft.width());
            bottom.setY(r.bottom() - bottomStretch.height()+1);
            bottom.setWidth(r.width() - bottomLeft.width() - bottomRight.width());
            bottom.setHeight(bottomLeft.height());
            painter->drawTiledPixmap(bottom, bottomStretch);
            painter->restore();
        }

        break;
    case PE_FrameGroupBox:
        if (const QStyleOptionFrameV2 *group
                = qstyleoption_cast<const QStyleOptionFrameV2 *>(option)) {
            const QRect &r = group->rect;

            painter->save();
            int radius = 14;
            int radius2 = radius*2;
            QPainterPath clipPath;
            clipPath.moveTo(radius, 0);
            clipPath.arcTo(r.right() - radius2, 0, radius2, radius2, 90, -90);
            clipPath.arcTo(r.right() - radius2, r.bottom() - radius2, radius2, radius2, 0, -90);
            clipPath.arcTo(r.left(), r.bottom() - radius2, radius2, radius2, 270, -90);
            clipPath.arcTo(r.left(), r.top(), radius2, radius2, 180, -90);
            painter->setClipPath(clipPath);
            QPixmap titleStretch;
            QPixmap topLeft;
            QPixmap topRight;
            QPixmap bottomLeft;
            QPixmap bottomRight;
            QPixmap leftStretch;
            QPixmap topStretch;
            QPixmap rightStretch;
            QPixmap bottomStretch;
            QLinearGradient lg(0, 0, 0, r.height());

            if (myTheme==THEME_NORMAL){
                titleStretch = cached(":res/images/title_stretch.png");
                topLeft = cached(":res/images/groupframe_topleft.png");
                topRight = cached(":res/images/groupframe_topright.png");
                bottomLeft = cached(":res/images/groupframe_bottom_left.png");
                bottomRight = cached(":res/images/groupframe_bottom_right.png");
                leftStretch = cached(":res/images/groupframe_left_stretch.png");
                topStretch = cached(":res/images/groupframe_top_stretch.png");
                rightStretch = cached(":res/images/groupframe_right_stretch.png");
                bottomStretch = cached(":res/images/groupframe_bottom_stretch.png");
            }



            lg.setColorAt(0, qcolorIni);
            lg.setColorAt(1, qcolorFin);
            painter->setPen(Qt::NoPen);
            painter->setBrush(lg);
            painter->drawRect(r.adjusted(0, titleStretch.height()/2, 0, 0));
            painter->setClipping(false);

            int topFrameOffset = titleStretch.height()/2 - 2;
            painter->drawPixmap(r.topLeft() + QPoint(0, topFrameOffset), topLeft);
            painter->drawPixmap(r.topRight() - QPoint(topRight.width()-1, 0)
                                + QPoint(0, topFrameOffset), topRight);
            painter->drawPixmap(r.bottomLeft() - QPoint(0, bottomLeft.height()-1), bottomLeft);
            painter->drawPixmap(r.bottomRight() - QPoint(bottomRight.width()-1,
                                bottomRight.height()-1), bottomRight);

            QRect left = r;
            left.setY(r.y() + topLeft.height() + topFrameOffset);
            left.setWidth(leftStretch.width());
            left.setHeight(r.height() - topLeft.height() - bottomLeft.height() - topFrameOffset);
            painter->drawTiledPixmap(left, leftStretch);

            QRect top = r;
            top.setX(r.x() + topLeft.width());
            top.setY(r.y() + topFrameOffset);
            top.setWidth(r.width() - topLeft.width() - topRight.width());
            top.setHeight(topLeft.height());
            painter->drawTiledPixmap(top, topStretch);

            QRect right = r;
            right.setX(r.right() - rightStretch.width()+1);
            right.setY(r.y() + topRight.height() + topFrameOffset);
            right.setWidth(rightStretch.width());
            right.setHeight(r.height() - topRight.height()
                            - bottomRight.height() - topFrameOffset);
            painter->drawTiledPixmap(right, rightStretch);

            QRect bottom = r;
            bottom.setX(r.x() + bottomLeft.width());
            bottom.setY(r.bottom() - bottomStretch.height()+1);
            bottom.setWidth(r.width() - bottomLeft.width() - bottomRight.width());
            bottom.setHeight(bottomLeft.height());
            painter->drawTiledPixmap(bottom, bottomStretch);
            painter->restore();

        }
        break;

    default:
        QWidget *parent=widget->parentWidget();
        if (parent){
            QPalette pal=parent->palette();
            pal.setColor(QPalette::WindowText,QColor(10,10,10));
            pal.setColor(QPalette::Text,QColor(10,10,10));
            parent->setPalette(pal);
        }
        QWindowsStyle::drawPrimitive(element, option, painter, widget);
        break;
    }
    return;
}


void InkjetStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                     QPainter *painter, const QWidget *widget) const
{
    switch (control) {
    case CC_Slider:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            QRect groove = subControlRect(CC_Slider, option, SC_SliderGroove, widget);
            QRect handle = subControlRect(CC_Slider, option, SC_SliderHandle, widget);

            painter->save();

            bool hover = (slider->state & State_Enabled) && (slider->state & State_MouseOver);
            if (hover) {
                QRect moderated = widget->rect().adjusted(0, 4, 0, -4);
                drawHoverRect(painter, moderated);
            }

            if ((option->subControls & SC_SliderGroove) && groove.isValid()) {
                QPixmap grv = cached(":res/images/slider_bar.png");
                painter->drawPixmap(QRect(groove.x() + 5, groove.y(),
                                          groove.width() - 10, grv.height()),
                                    grv);
            }
            if ((option->subControls & SC_SliderHandle) && handle.isValid()) {
                QPixmap hndl = cached(":res/images/slider_thumb_on.png");
                painter->drawPixmap(handle.topLeft(), hndl);
            }

            painter->restore();
        }
        break;
    case CC_GroupBox:
        if (const QStyleOptionGroupBox *groupBox
                = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            QStyleOptionGroupBox groupBoxCopy(*groupBox);
            groupBoxCopy.subControls &= ~SC_GroupBoxLabel;
            QWindowsStyle::drawComplexControl(control, &groupBoxCopy, painter, widget);

            if (groupBox->subControls & SC_GroupBoxLabel) {
                const QRect &r = groupBox->rect;
                QPixmap titleLeft = cached(":res/images/title_cap_left.png");
                QPixmap titleRight = cached(":res/images/title_cap_right.png");
                QPixmap titleStretch = cached(":res/images/title_stretch.png");
                int txt_width = groupBox->fontMetrics.width(groupBox->text) + 20;
                painter->drawPixmap(r.center().x() - txt_width/2, 0, titleLeft);
                QRect tileRect = subControlRect(control, groupBox, SC_GroupBoxLabel, widget);
                painter->drawTiledPixmap(tileRect, titleStretch);
                painter->drawPixmap(tileRect.x() + tileRect.width(), 0, titleRight);
                int opacity = 31;
                painter->setPen(QColor(0, 0, 0, opacity));
                painter->drawText(tileRect.translated(0, 1),
                                  Qt::AlignVCenter | Qt::AlignHCenter, groupBox->text);
                painter->drawText(tileRect.translated(2, 1),
                                  Qt::AlignVCenter | Qt::AlignHCenter, groupBox->text);
                painter->setPen(QColor(0, 0, 0, opacity * 2));
                painter->drawText(tileRect.translated(1, 1),
                                  Qt::AlignVCenter | Qt::AlignHCenter, groupBox->text);
                painter->setPen(Qt::white);
                painter->drawText(tileRect, Qt::AlignVCenter | Qt::AlignHCenter, groupBox->text);
            }
        }
        break;
    default:
        QWindowsStyle::drawComplexControl(control, option, painter, widget);
        break;
    }
    return;
}

QRect InkjetStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                  SubControl subControl, const QWidget *widget) const
{
    QRect rect;

    switch (control) {
    default:
        rect = QWindowsStyle::subControlRect(control, option, subControl, widget);
        break;
    case CC_GroupBox:
        if (const QStyleOptionGroupBox *group
                = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            switch (subControl) {
            default:
                rect = QWindowsStyle::subControlRect(control, option, subControl, widget);
                break;
            case SC_GroupBoxContents:
                rect = QWindowsStyle::subControlRect(control, option, subControl, widget);
                rect.adjust(0, -8, 0, 0);
                break;
            case SC_GroupBoxFrame:
                rect = group->rect;
                break;
            case SC_GroupBoxLabel:
                QPixmap titleLeft = cached(":res/images/title_cap_left.png");
                QPixmap titleRight = cached(":res/images/title_cap_right.png");
                QPixmap titleStretch = cached(":res/images/title_stretch.png");
                int txt_width = group->fontMetrics.width(group->text) + 20;
                rect = QRect(group->rect.center().x() - txt_width/2 + titleLeft.width(), 0,
                             txt_width - titleLeft.width() - titleRight.width(),
                             titleStretch.height());
                break;
            }
        }
        break;
    }

    if (control == CC_Slider && subControl == SC_SliderHandle) {
        rect.setWidth(13);
        rect.setHeight(27);
    } else if (control == CC_Slider && subControl == SC_SliderGroove) {
        rect.setHeight(9);
        rect.moveTop(27/2 - 9/2);
    }
    return rect;
}

QSize InkjetStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
                                    const QSize &size, const QWidget *widget) const
{
    QSize newSize = QWindowsStyle::sizeFromContents(type, option, size, widget);


    switch (type) {
    case CT_RadioButton:
        newSize += QSize(20, 0);
        break;

    case CT_PushButton:
        //newSize.setHeight(26);
        //newSize.setHeight(newSize.height());

        //newSize.setHeight(75);
        break;

    case CT_Slider:
        newSize.setHeight(27);
        break;

    default:
        break;
    }

    return newSize;
}

int InkjetStyle::pixelMetric(PixelMetric pm, const QStyleOption *opt, const QWidget *widget) const
{
    if (pm == PM_SliderLength)
        return 13;
    return QWindowsStyle::pixelMetric(pm, opt, widget);
}

void InkjetStyle::polish(QWidget *widget)
{
    if (widget->layout() && qobject_cast<QGroupBox *>(widget)) {
        if (qFindChildren<QGroupBox *>(widget).size() == 0) {
            widget->layout()->setSpacing(0);
            widget->layout()->setMargin(12);
        } else {
            widget->layout()->setMargin(13);
        }
    }

    if (qobject_cast<QPushButton *>(widget)
        || qobject_cast<QRadioButton *>(widget)
        || qobject_cast<QSlider *>(widget)) {
        widget->setAttribute(Qt::WA_Hover);
    }

    QPalette pal = widget->palette();
    if (widget->isWindow()) {
        pal.setColor(QPalette::Background, QColor(225, 225, 225));
        widget->setPalette(pal);
    }

}

void InkjetStyle::unpolish(QWidget *widget)
{
    if (qobject_cast<QPushButton *>(widget)
        || qobject_cast<QRadioButton *>(widget)
        || qobject_cast<QSlider *>(widget)) {
        widget->setAttribute(Qt::WA_Hover, false);
    }
}

void InkjetStyle::polish(QPalette &palette)
{
    palette.setColor(QPalette::Background, qcolorFin);
}

QRect InkjetStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    QRect r;
    switch(element) {
    case SE_RadioButtonClickRect:
        r = widget->rect();
        break;
    case SE_RadioButtonContents:
        r = widget->rect().adjusted(20, 0, 0, 0);
        break;
    default:
        r = QWindowsStyle::subElementRect(element, option, widget);
        break;
    }

    if (qobject_cast<const QRadioButton*>(widget))
        r = r.adjusted(5, 0, -5, 0);

    return r;
}
