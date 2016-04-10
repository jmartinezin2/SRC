/** @addtogroup EditImagenFija
 *  @{*/
/** @file EditImagenFija.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions.
 *
 *    @brief Pantalla de Imagen fija
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: EditImagenFija.h
 *    @li Versión: x.0
 *    @li Fecha: 01/05/14
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef EDITIMAGENFIJA_H
#define EDITIMAGENFIJA_H

#include "InkjetForm.h"
#include "MemoryMap.h"
#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QTimer>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFontComboBox>
#include <QMouseEvent>
#include "ui_EditImagenFijaUI.h"
#include "inkjetcommon.h"
#include "in2graphicsscene.h"
#include "in2graphicsview.h"
#include <QMainWindow>

class EditImagenFija : public QMainWindow
{
	Q_OBJECT

public:
	//Constructor
	EditImagenFija(QWidget *parent);
	//Destructor
	~EditImagenFija();

private:


private slots:

	void UpdateCoordenadaXinicial();
	void UpdateCoordenadaYinicial();
	void UpdateXinicial(QString value);
	void UpdateYinicial(QString value);

    void UpdateAnchoPlantilla();
    void UpdateAltoPlantilla();
    void UpdateAncho(QString value);
    void UpdateAlto(QString value);

    void ActualizarColor(int x, int y);
    void ActualizarColor(const QPointF &p);
    void updatePixelInfo();
    void UpdateColor();
    //void UpdateMemoriaColor(double x_mm, double y_mm);
    void UpdateMemoriaColorClicked(const QPointF &p);

    void UpdateCoordenadaXcolor();
    void UpdateCoordenadaYcolor();
    void UpdateXcolor(QString value);
    void UpdateYcolor(QString value);

    void UpdateNumeroColores();
    void UpdateNumero(QString value);

	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

    void BuscarPlantilla();

    void PasarValoresDeMapATemp();
    void PasarValoresDeTempAMap();




	// Si no es root, bloqueo funcionalidad.
	void TestRoot();

    void CreaImagenMuestra();
    void MuestraImagen(const QPixmap & Imagen, int color);
	void LaunchComposicion();

    void ActualizarControlesColor();

    //virtual void mouseDoubleClickEvent(QMouseEvent * event);
    void SelCorteImagen();
    void SelColorMascara();
    void SelColorPrint();
    void sltConfigColors();
    void sltPrevisualizacion();
    void sltUpdatePlantilla(int d);
    void sltUpdateAnchoPlantilla(int d);
    void sltUpdateAltoPlantilla(int d);
    void sltUpdateDistanciaColumnas(int d);
    void sltUpdateOffsetX(int d);
    void sltUpdateOffsetY(int d);
    void sltRenderPlantilla();
    void sltPlantEnabled();
    void sltAddPlantImage(int x,int y);
    bool eventFilter(QObject *target, QEvent *event);
    void sltAddColor(int i,bool bActualiza);
    void sltSelectColorTable(int x,int y);
    void sltUpdateNumColores();
    void sltPrevColor(int index);
    void sltUpdateSingleColor(int index);
    void sltAlgSelColor();
    void sltChangeDithering(int bits);
    void sltChangeGrayThreshold();
    void sltRotateImage();
    void sltReload();
    void sltCheckTonos(QString color);
    void sltNewColorString(QString & color,QString & id);
    void sltSelectRow(int x,int y);


public slots:
        void AceptarValoresCampos();
private:
    Ui::EditImagenFijaUI *ui;

	//Temporizador de eventos
	QTimer *m_Temporizador;
    //QPixmap pixmapPlantilla;

    double XinicialPlantilla_tmp;
    double YinicialPlantilla_tmp;
    double AnchoPlantilla_tmp;
    double AltoPlantilla_tmp;
    QString RutaPlantilla_tmp;
    m_Colores colores_tmp[NUM_COLORES];
    unsigned char NumColores_tmp;
    struct m_Colores m_colores_tmp[NUM_COLORES];
    int offset_x_plantilla;
    int offset_y_plantilla;
    unsigned char m_escalado;
    In2GraphicsScene *m_Scene;
    In2GraphicsView *m_View;
    In2GraphicsScene *m_SceneColor1;
    In2GraphicsView *m_ViewColor1;
    In2GraphicsScene *m_SceneColor2;
    In2GraphicsView *m_ViewColor2;
    In2GraphicsScene *m_SceneColor3;
    In2GraphicsView *m_ViewColor3;
    In2GraphicsScene *m_SceneColor4;
    In2GraphicsView *m_ViewColor4;
    MemoryMap *memoryMap;






signals:
    void SendDebugInfo(unsigned char code, QString msg,bool save);
};

#endif // EDITIMAGENFIJA_H
