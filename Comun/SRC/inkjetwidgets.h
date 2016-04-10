/** @addtogroup inkjetwidgets
 *  @{*/
/** @file inkjetwidgets.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Widgets de la aplicación
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: inkjetwidgets.h
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef INKJETWIDGETS_H
#define INKJETWIDGETS_H

#include "inkjetstyle.h"
#include <QBitmap>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>

#if defined(QT_OPENGL_SUPPORT)
#include <QGLWidget>
class GLWidget : public QGLWidget
{
public:
    GLWidget(QWidget *parent)
        : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {}
    void disableAutoBufferSwap() { setAutoBufferSwap(false); }
    void paintEvent(QPaintEvent *) { parentWidget()->update(); }
};
#endif

class QTextDocument;
class QTextEdit;
class QVBoxLayout;

class InkjetFrame : public QWidget
{
    Q_OBJECT
public:
    InkjetFrame(QWidget *parent);
    virtual void paint(QPainter *) {}


    void paintDescription(QPainter *p);

    void loadDescription(const QString &filename);
    void setDescription(const QString &htmlDesc);

    void loadSourceFile(const QString &fileName);

    bool preferImage() const { return m_prefer_image; }

#if defined(QT_OPENGL_SUPPORT)
    QGLWidget *glWidget() const { return glw; }
#endif

public slots:
    void setPreferImage(bool pi) { m_prefer_image = pi; }
    void setDescriptionEnabled(bool enabled);
    void showSource();

#if defined(QT_OPENGL_SUPPORT)
    void enableOpenGL(bool use_opengl);
    bool usesOpenGL() { return m_use_opengl; }
#endif

signals:
    void descriptionEnabledChanged(bool);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

#if defined(QT_OPENGL_SUPPORT)
    GLWidget *glw;
    bool m_use_opengl;
#endif
    QPixmap m_tile;

    bool m_show_doc;
    bool m_prefer_image;
    QTextDocument *m_document;

    QString m_sourceFileName;

};

#endif
