/** @addtogroup DbFormatPage
 *  @{*/
/** @file DbFormatPage.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: DbFormatPage.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef DBFORMATPAGE_H
#define DBFORMATPAGE_H

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
#include <QImage>
#include <QPixmap>
#include "ui_EditValoresUI.h"
#include "inkjetcommon.h"

class DbFormatPage : public QWidget
{
	Q_OBJECT

public:
	//Constructor
	DbFormatPage(QWidget *parent);
	//Destructor
	~DbFormatPage();

private:


private slots:

	void UpdateWebWidth();
	void UpdateWidth(QString value);

	void UpdateWebHeight();
	void UpdateHeight(QString value);

	void UpdateNumeroFilas();
	void UpdateFilas(QString value);

	void UpdateNumeroColumnas();
	void UpdateColumnas(QString value);

	void UpdateSeparacionFilas();
	void UpdateSepFilas(QString value);

	void UpdateSeparacionColumnas();
	void UpdateSepColumnas(QString value);

    void UpdateAnguloPlantilla();
    void UpdateAngulo(QString value);

	void UpdatePuntoInicialX();
	void UpdateInicialX(QString value);

	void UpdatePuntoInicialY();
	void UpdateInicialY(QString value);

    void CambiarMacula();
    void UpdateOnOffMacula();

    void UpdateMaculaAncho();
    void UpdateAncho(QString value);

    void UpdateMaculaAlto();
    void UpdateAlto(QString value);

    void UpdateMaculaX();
    void UpdateX(QString value);

    void UpdateMaculaY();
    void UpdateY(QString value);

    void UpdateMaculaFrecuencia();
    void UpdateFrecuencia(QString value);

    void UpdateMaculaRepeticiones();
    void UpdateRepeticiones(QString value);

    void UpdateColorMacula();


    void CambiarMarca();
    void UpdateOnOffMarca();

    void UpdateMarcaX();
    void UpdateXm(QString value);

    void UpdateMarcaY();
    void UpdateYm(QString value);

    void UpdateMarcaFrecuencia();
    void UpdateFrecuencia_m(QString value);

    void UpdateMarcaRepeticiones();
    void UpdateRepeticiones_m(QString value);

    void UpdateColorMarca();

	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

	void AceptarValoresCampos();

    void PasarDeTempAMap();
    void PasarDeMapATemp();

	void ActualizaDataSlot();
    //void ActualizaDataSlotOPC();
	// Si no es root, bloqueo funcionalidad.
	void TestRoot();

	void CreaImagenPagina();
	void MuestraImagen(QPixmap Imagen);
	void LaunchDbImpresion();

	void CargaDatos();
	void GuardaDatos();
    void SetDataCommon();

    void sltEditComentario();
    void sltSaveComentario();
    void sltDeleteComentario();

    void ConfigurarMarca(void);
    void ConfigurarCodigoBarras(void);
    void sltBarcodeConfigured(const m_BarcodeParameters & data);


public slots:


private:

    Ui::EditValoresUI *ui;
    InkjetCommon *DataCommon;
	//Temporizador de eventos
	QTimer *m_Temporizador;
    //QPixmap pixmapPlantilla;

    QPixmap ImagenUnitariaPagina;
    QPixmap rotatedpixmap;
    //QImage Imagen;

    double WebWidth_tmp;
    double WebHeight_tmp;
    double PuntoInicialX_tmp;
    double PuntoInicialY_tmp;
    unsigned int FilasPlantilla_tmp;
    unsigned int ColumnasPlantilla_tmp;
    double SeparacionX_tmp;
    double SeparacionY_tmp;
    int AnguloPlantilla_tmp;
    double MaculaIzqdaX_tmp;
    double MaculaIzqdaY_tmp;
    double MaculaIzqdaAncho_tmp;
    double MaculaIzqdaAlto_tmp;
    double MaculaDchaX_tmp;
    double MaculaDchaY_tmp;
    double MaculaDchaAncho_tmp;
    double MaculaDchaAlto_tmp;
    unsigned char MaculaIzqdaOnOff_tmp;
    unsigned char MaculaDchaOnOff_tmp;
    unsigned int MaculaDchaRep_tmp;
    double MaculaDchaFreq_tmp;
    unsigned int MaculaIzqdaRep_tmp;
    double MaculaIzqdaFreq_tmp;
    unsigned char MaculaIzqdaColor_tmp;
    unsigned char MaculaDchaColor_tmp;
    unsigned int ancho_qlabel_pagina;
    unsigned int alto_qlabel_pagina;
    double MarcaSupX_tmp;
    double MarcaSupY_tmp;
    double MarcaInfX_tmp;
    double MarcaInfY_tmp;
    unsigned char MarcaSupOnOff_tmp;
    unsigned char MarcaInfOnOff_tmp;
    unsigned int MarcaSupRep_tmp;
    double MarcaSupFreq_tmp;
    unsigned int MarcaInfRep_tmp;
    double MarcaInfFreq_tmp;
    unsigned char MarcaSupColor_tmp;
    unsigned char MarcaInfColor_tmp;


signals:
    void SendDebugInfo(unsigned char code, QString msg);

};


#endif // DBFORMATPAGE_H
