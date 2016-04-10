/** @addtogroup DbFormat
 *  @{*/
/** @file DbFormat.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: DbFormat.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef DBFORMAT_H
#define DBFORMAT_H

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
#include "ui_EditCamposUI.h"
#include "inkjetcommon.h"
#include "counterStyle.h"

class DbFormat : public QWidget
{
	Q_OBJECT

public:
	//Constructor
	DbFormat(QWidget *parent);
	//Destructor
	~DbFormat();

private:


private slots:

    void UpdateNumeroCampo();
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

	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

    void PasarDeMapATemp();
    void PasarDeTempAMap();
    void PasarDeMapATempVariables();
    void PasarDeTempAMapVariables();
	void AceptarValoresCampos();

	void ActualizaDataSlot();
    //void ActualizaDataSlotOPC();

	// Si no es root, bloqueo funcionalidad.
	void TestRoot();

    void CreaImagenMuestra();
    void CreaImagenMuestraVariable();
	void MuestraImagen(QPixmap Imagen);
	void LaunchDbImpresion();
    void SetDataCommon();
    QPoint UpdateMousePos(const QPoint &p );
    void sltMoveItem();
    QPixmap DrawFixedItem(int i);
    QPixmap DrawVariableItem(int i);
    void UpdateComposition(const QPoint &point);
    void ActualizaDatosItem();
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

public slots:


private:
    Ui::EditCamposUI *ui;
    InkjetCommon *DataCommon;
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

public:

signals:
    void SendDebugInfo(unsigned char code, QString msg);

};

#endif // DBFORMAT_H
