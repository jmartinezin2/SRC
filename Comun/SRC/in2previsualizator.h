#ifndef IN2PREVISUALIZATOR_H
#define IN2PREVISUALIZATOR_H

#include <QWidget>
#include "ui_in2visualizator.h"
#include "in2graphicsview.h"
#include "in2graphicspixmapitem.h"
#include "in2graphicsscene.h"

class In2Previsualizator : public QWidget
{
    Q_OBJECT
public:
    explicit In2Previsualizator(QWidget *parent = 0,QString  filename="");
    ~In2Previsualizator();

private slots:
    void sltPrint();
    void sltShowImage();
    void sltGeneraMuestra();
signals:

private:
    In2GraphicsScene *m_scene;
    In2GraphicsView *m_view;
    Ui::In2Visualizator *ui;
    QString m_fileName;

public slots:
};

#endif // IN2PREVISUALIZATOR_H
