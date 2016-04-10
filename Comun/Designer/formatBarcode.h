/** @addtogroup formatBarcode
 *  @{*/
/** @file formatBarcode.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions.
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: formatBarcode.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/14
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef FORMATBARCODE_H
#define FORMATBARCODE_H

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
#include "ui_EditBarcodeUI.h"
#include "inkjetcommon.h"
#include "In2BarcodeClass.h"

class formatBarcode : public QWidget
{
	Q_OBJECT

public:
	//Constructor
    formatBarcode(QWidget *parent,struct m_BarcodeParameters data);
	//Destructor
	~formatBarcode();

private:


private slots:

	void UpdateAnchoBarcode();
	void UpdateAltoBarcode();
	void UpdateAncho(QString value);
	void UpdateAlto(QString value);


	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();

	void AceptarValoresCampos();

	// Si no es root, bloqueo funcionalidad.
	void TestRoot();

    void CreaImagenMuestra();
	void MuestraImagen(QPixmap Imagen);
    void UpdateMousePos(const QPoint &p );


public slots:


private:
    Ui::EditBarcodeUI *ui;
    InkjetCommon *DataCommon;
    //Temporizador de eventos
    QTimer *m_Temporizador;
    //QPixmap pixmapPlantilla;

    //QPixmap ImagenUnitaria;
    QPixmap rotatedpixmap;
    //Variables de Escalado
    unsigned char escalado;
    int offset_x;
    int offset_y;

    In2BarcodeClass *dummybc;
    struct m_BarcodeParameters m_Barcode;


signals:
    void SendDebugInfo(unsigned char code, QString msg);
    void sglBarcodeConfigured(const m_BarcodeParameters & data);

};

#endif // FORMATBARCODE_H
