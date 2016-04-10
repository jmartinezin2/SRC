/** @addtogroup EditCampos
 *  @{*/
/** @file EditCampos.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions.
 *
 *    @brief Pantalla de campos de composicion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: EditCampos.h
 *    @li Versión: x.0
 *    @li Fecha: 01/05/14
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef EDITCAMPOS_H
#define EDITCAMPOS_H

#include "InkjetForm.h"
#include "MemoryMap.h"
#include <QComboBox>
#include <QTextEdit>
#include <QVector>
#include <QCheckBox>
#include <QTimer>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFontComboBox>
#include "ui_EditCamposUI.h"
#include "inkjetcommon.h"
#include "counterStyle.h"
#include "in2graphicsscene.h"
#include "in2graphicsview.h"
#include "In2FieldPixmapItem.h"

class EditCampos : public QWidget
{
	Q_OBJECT

public:
	//Constructor
	EditCampos(QWidget *parent);
	//Destructor
	~EditCampos();

private:


private slots:

    void UpdateNumeroCampo();
    void UpdateNumeroCampo(int index);
    void UpdateNumeroMatriz();
	//void UpdateNombreCampo();
	void UpdateCampoHorizontal();
	void UpdateCampoVertical();
	void UpdateVertical(QString value);
	void UpdateHorizontal(QString value);

	void UpdateTamanoFuente();
	void UpdateTamano(QString value);

	//void UpdateTipoFuente();
	//void UpdateTipo(QString value);

	void UpdateOrientacionFuente();
	void UpdateOrientacion(QString value);

    void UpdateFilasMatriz();
	void UpdateFilas(QString value);
	
    void UpdateValorCampo();
    void sltUpdateValorCampo(QString value);

    void UpdateColumnasMatriz();
	void UpdateColumnas(QString value);
	
    void UpdateFrecuenciaFilasMatriz();
	void UpdateFrecuenciaFilas(QString value);
	
    void UpdateFrecuenciaColumnasMatriz();
	void UpdateFrecuenciaColumnas(QString value);
	
    void UpdateExcepcionesMatriz();
    void UpdateExcepciones(QString value);

    void UpdateOnOffMatriz();

    void UpdateColorCampo(void);


    //Acepta la seleccion de parametros y sale
    void AceptarSeleccionAndExit();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

    void PasarDeMapATemp();
    void PasarDeTempAMap();
    void PasarDeMapATempVariables();
    void PasarDeTempAMapVariables();

    void PasarDeMapATemp(int index);
    void PasarDeTempAMap(int index);
    void PasarDeMapATempVariables(int index);
    void PasarDeTempAMapVariables(int index);

    void AceptarValoresCampos();


	// Si no es root, bloqueo funcionalidad.
	void TestRoot();

    void MuestraImagen();
    void LaunchComposicion();
    QPoint UpdateMousePos(const QPointF &p );
    void sltMoveItem();
    void UpdateComposition(const QPoint &point);
    void ActualizaDatosItem();
    void sltItemSelected();
    void ActualizaDatosItem(int id);
    void sltItemSelected(int id);

    void sltMoveLeft();
    void sltMoveRight();
    void sltMoveUp();
    void sltMoveDown();

    void ConfigurarOrigen(void);
    void ConfigurarElemento(void);

    void sltBarcodeConfigured(const m_BarcodeParameters & data);
    void ConfigurarCodigoBarras(void);
    void UpdateTipoCampo(void);
    void UpdateOrigenCampo(void);

    void sltCounterConfigured(const m_CounterParameters & data);
    void ConfigurarContador(void);

    void sltDateTimeConfigured(const m_DateTimeParameters & data);
    void ConfigurarFechaHora(void);

    void sltRandomConfigured(const m_RandomNumberParameters & data);
    void ConfigurarRandom(void);
    void drawItem(int i,int matriz);
    void sltShowGrid();
    void UpdateGridInfo();
    void sltGridMas();
    void sltGridMenos();
    void sltTrackItem();

public slots:


private:
    Ui::EditCamposUI *ui;
    //Temporizador de eventos
    QTimer *m_Temporizador;
    //QPixmap pixmapPlantilla;

    //QPixmap ImagenUnitaria;
    QPixmap rotatedpixmap;
    //Variables de Escalado
    //unsigned char escalado;
    int offset_x;
    int offset_y;
    counterStyle* miEstiloContador;
    In2GraphicsScene *m_Scene;
    In2GraphicsView *m_View;


    QVector<In2FieldPixmapItem *> m_OrdenCapas;
    int m_margin;
    MemoryMap *m_memoryMap;




public:

signals:
    void SendDebugInfo(unsigned char code, QString msg,bool save);
    void repaintIntem(int i);

};

#endif // EDITCAMPOS_H
