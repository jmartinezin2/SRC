#ifndef QLABELEXT_H
#define QLABELEXT_H

#include <QLabel>
#include <QRubberBand>
#include <QMouseEvent>


//Para hacer zoom
struct Recuadro{
    int x1;
    int x2;
    int y1;
    int y2;
    bool pulso;
};


class QLabelExt : public QLabel
{
    Q_OBJECT
public:
     QLabelExt(QWidget *parent = 0);
    ~QLabelExt();
     void SetBlocked(bool block);
     bool isBlocked();
     void setRubber(const QRect & rect);

private:
     void paintRubber(const QRect & r);
     void lookRubberCorner(const QPoint &p);
     void modifyRubberBand(const QPoint &p);
private:
    Recuadro pulsacion;
    QRubberBand *rubberBand;
    QRubberBand *cornerRubberBand;
    QPoint origin;
    bool m_blocked;
    int m_CornerSelected;
signals:
    void press(const QPoint &p);
    void release(const QPoint &p);
    void moving(const QPoint &p);
    void newSelection(const QRect &r);
protected slots:
    virtual void enterEvent ( QEvent * event );
    virtual void leaveEvent ( QEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );

public slots:
    
};

#endif // QLABELEXT_H
