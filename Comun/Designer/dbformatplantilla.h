/** @addtogroup DbFormat
 *  @{*/
/** @file DbFormatPlantilla.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: DbFormatPlantilla.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef DBFORMATPLANTILLA_H
#define DBFORMATPLANTILLA_H

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

class DbFormatPlantilla : public QWidget
{
	Q_OBJECT

public:
	//Constructor
	DbFormatPlantilla(QWidget *parent);
	//Destructor
	~DbFormatPlantilla();

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
    void UpdateColor();
    //void UpdateMemoriaColor(double x_mm, double y_mm);
    void UpdateMemoriaColorClicked();

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



	void ActualizaDataSlot();
    //void ActualizaDataSlotOPC();

	// Si no es root, bloqueo funcionalidad.
	void TestRoot();

    void CreaImagenMuestra();
	void MuestraImagen(QPixmap Imagen);
	void LaunchDbImpresion();

    void ActualizarControlesColor();

    //virtual void mouseDoubleClickEvent(QMouseEvent * event);
    void setDataCommon();
    void SelCorteImagen();
    void SelColorMascara();
    void SelColorPrint();
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
    QImage sltPlantGetImage(int x,int y);
    bool eventFilter(QObject *target, QEvent *event);

public slots:
        void AceptarValoresCampos();
private:
    Ui::EditImagenFijaUI *ui;
    InkjetCommon *DataCommon;

	//Temporizador de eventos
	QTimer *m_Temporizador;
	QPixmap pixmapPlantilla;

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



signals:
    void SendDebugInfo(unsigned char code, QString msg);
};

#endif // DBFORMATPLANTILLA_H
