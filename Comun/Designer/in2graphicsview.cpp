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

#include "in2graphicsview.h"
#include "In2GraphicsView.h"
#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <qmath.h>

In2GraphicsView::In2GraphicsView(QGraphicsView* view)
  : QObject(view), m_view(view)
{
  m_view->viewport()->installEventFilter(this);
  m_view->setMouseTracking(true);
  m_modifiers = Qt::ControlModifier;
  m_ZoomFactorBase = 1.0015;
  m_view->setAcceptDrops(true);
}
/** Carga un zoom*/
void In2GraphicsView::NewZoom(double factor) {
  m_view->scale(factor, factor);
  m_view->centerOn(m_ScenePos);
  QPointF delta_viewport_pos = m_ViewPortPos - QPointF(m_view->viewport()->width() / 2.0,                                                             m_view->viewport()->height() / 2.0);
  QPointF viewport_center = m_view->mapFromScene(m_ScenePos) - delta_viewport_pos;
  m_view->centerOn(m_view->mapToScene(viewport_center.toPoint()));
  //emit zoomed();
}

/** Modificadores*/
void In2GraphicsView::SetModifiers(Qt::KeyboardModifiers modifiers) {
  m_modifiers = modifiers;

}
/** Establece el factor inicial*/
void In2GraphicsView::SetZoomFactorBase(double value) {
  m_ZoomFactorBase = value;
}
/** Filtro de eventos*/
bool In2GraphicsView::eventFilter(QObject *object, QEvent *event) {
    if (event->type()==QEvent::MouseButtonPress){
        m_view->setDragMode(QGraphicsView::ScrollHandDrag);
        emit gvMousePress();
    }
    else if (event->type()==QEvent::MouseButtonRelease){
        m_view->setDragMode(QGraphicsView::NoDrag);
        emit gvMouseRelease();
    }
    else if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        QPointF delta = m_ViewPortPos - mouse_event->pos();
        if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
            m_ViewPortPos = mouse_event->pos();
            m_ScenePos = m_view->mapToScene(mouse_event->pos());
            //if (m_view->scene())
           /* QGraphicsPixmapItem *child = dynamic_cast<QGraphicsPixmapItem*>(childAt(m_ScenePos));
            if (child){
                emit trackPoint(m_ScenePos);
            }*/
            emit trackPoint(m_ScenePos);

        }
    }
    else if (event->type() == QEvent::Wheel) {
        QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);
        if (wheel_event->orientation() == Qt::Vertical) {
            double angle = wheel_event->delta();
            double factor = qPow(m_ZoomFactorBase, angle);
            NewZoom(factor);
            return true;
        }
    }
    else if (event->type()==QEvent::MouseButtonDblClick){
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
        QPointF selPoint = m_view->mapToScene(mouse_event->pos());
        emit selectedPoint(selPoint);
    }

  Q_UNUSED(object)
  return false;
}
