#ifndef IN2QLABEL_H
#define IN2QLABEL_H

#include <QLabel>
#include <QRubberBand>
#include <QMouseEvent>
#include <QPixmap>
#include <QList>
#include <QRgb>

//Para hacer zoom
struct Pulsacion{
    int x1;
    int x2;
    int y1;
    int y2;
    bool zooming;
};


class In2QLabel : public QLabel
{
Q_OBJECT

public:
    In2QLabel(QWidget * parent = 0);
    ~In2QLabel();
    void setPixmap ( const QPixmap &, bool save );
    void setPixmap(const QPixmap &);
    void setOriginalPixmap(const QPixmap &);
    QRgb SelectedPixelColor(); //Devuelve el color del pixel seleccionado
    QPoint GetFirstColorOcurrence(QRgb color); //Devuelve primera ocurrencia de color
    QPixmap OriginalPixmap();
    void setNewPixmap(const QPixmap &p);
    void SetAcceptDrops(bool accept);
    void setNewMimePixmap(const QPixmap & tmpLabel);
signals:
    void DobleClick(int x, int y);
    void LabelPos(const QPoint &p);
    void SetItemPos(const QPoint &p);
private:
    Pulsacion pulsacion;
    QRubberBand *rubberBand;
    QPoint origin;
    QList<QPixmap> prevPixmap;
    QPixmap m_PictureOriginal;
    bool m_pressed;

protected slots:
    virtual void enterEvent ( QEvent * event );
    virtual void leaveEvent ( QEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void mouseDoubleClickEvent(QMouseEvent * event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
};

#endif // IN2QLABEL_H
