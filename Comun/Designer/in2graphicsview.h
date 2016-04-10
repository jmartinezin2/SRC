#ifndef IN2GRAPHICSVIEW_H
#define IN2GRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QRubberBand>



class In2GraphicsView : public QObject
{
    Q_OBJECT
public:
    In2GraphicsView(QGraphicsView *view);
    void NewZoom(double factor);
    void SetModifiers(Qt::KeyboardModifiers modifiers);
    void SetZoomFactorBase(double value);
    
private:
  QGraphicsView* m_view;
  Qt::KeyboardModifiers m_modifiers;
  double m_ZoomFactorBase;
  QPointF m_ScenePos, m_ViewPortPos;
  bool eventFilter(QObject* object, QEvent* event);

signals:
  void zoomed();
  void selectedPoint(const QPointF & p);
  void trackPoint(const QPointF & p);
  void gvMouseRelease();
  void gvMousePress();

    
};

#endif // IN2GRAPHICSVIEW_H
