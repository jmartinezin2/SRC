/** @addtogroup DbImpresion
 *  @{*/
/** @file DbImpresion.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: DbImpresion.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/14
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef DBIMPRESION_H
#define DBIMPRESION_H

#include "InkjetForm.h"
#include "MemoryMap.h"
#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QTimer>
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include "ui_ComposicionUI.h"
#include "inkjetcommon.h"

class DbImpresion : /*public InkjetForm*/ public QWidget
{
	Q_OBJECT

public:
	//Constructor
	DbImpresion(QWidget *parent);
	//Destructor
	~DbImpresion();

private:
	//Carga los datos que hay en memoria
	void CargaDatosCampos();
	//Guarda los datos que hay en memoria
	void GuardaDatosCampos();
    //Inicializa estructuras comunes
    void setDataCommon();
    //Recupera el numero de cabezales
    void UpdateNumCabezales();
    //Algoritmo In2 para eliminar blancos
    void PreprocessImagesIn2();
    //Muestra propiedades de la composicion
    void ShowCompositionProps();
    //Crea una imagen completa
    QPixmap CreaImagenCompleta();
private slots:
	//Acepta la seleccion de parametros
	void AceptarSeleccion();
	//Cancela la selección de parámetros
	void CancelarSeleccion();
	//Imprime
	void FormatImagen();
	void FormatPagina();
	void FormatPlantilla();
	//actualizar datos
	void ActualizaDataSlot();
    //void ActualizaDataSlotOPC();
	// Si no es root, bloqueo funcionalidad.
	void TestRoot();

	void UpdateOnOff1();
	void UpdateOnOff2();
    void UpdateOnOff3();
    void UpdateOnOff4();

    void UpdateOrdenFabricacion();
    void Update_OrdenFabricacion(QString value);

	void UpdateNombreCampo1();
	void Update_NombreCampo1(QString value);
	void UpdateNombreCampo2();
	void Update_NombreCampo2(QString value);
    void UpdateNombreCampo3();
    void Update_NombreCampo3(QString value);
    void UpdateNombreCampo4();
    void Update_NombreCampo4(QString value);

	void UpdateValorCampo1();
	void Update_ValorCampo1(QString value);
	void UpdateValorCampo2();
	void Update_ValorCampo2(QString value);
    void UpdateValorCampo3();
    void Update_ValorCampo3(QString value);
    void UpdateValorCampo4();
    void Update_ValorCampo4(QString value);

    void UpdateOnOffVar1();
    void UpdateOnOffVar2();
    void UpdateOnOffVar3();
    void UpdateOnOffVar4();
    void UpdateOnOffVar5();
    void UpdateOnOffVar6();


    void UpdateNombreCampoVar1();
    void Update_NombreCampoVar1(QString value);
    void UpdateNombreCampoVar2();
    void Update_NombreCampoVar2(QString value);
    void UpdateNombreCampoVar3();
    void Update_NombreCampoVar3(QString value);
    void UpdateNombreCampoVar4();
    void Update_NombreCampoVar4(QString value);
    void UpdateNombreCampoVar5();
    void Update_NombreCampoVar5(QString value);
    void UpdateNombreCampoVar6();
    void Update_NombreCampoVar6(QString value);


    void UpdateValorCampoVar1();
    void Update_ValorCampoVar1(QString value);
    void UpdateValorCampoVar2();
    void Update_ValorCampoVar2(QString value);
    void UpdateValorCampoVar3();
    void Update_ValorCampoVar3(QString value);
    void UpdateValorCampoVar4();
    void Update_ValorCampoVar4(QString value);
    void UpdateValorCampoVar5();
    void Update_ValorCampoVar5(QString value);
    void UpdateValorCampoVar6();
    void Update_ValorCampoVar6(QString value);


	void LaunchPrincipal();
	void GenerateImage();
    void GeneratePrev();
	void CargaPlantilla();
	void CargarComposicion();
	void GuardarComposicion();
    void NuevaComposicion();
	int CuadroDialogoGuardarComposicion();
    int CuadroDialogoNuevaComposicion();
    bool Comprobar_Campos_Vacios();
    bool Comprobar_OrdenFabricacion_Vacia();
    bool Comprobar_ExisteComposicion();
    void ComprobacionesGenerateImage();

    void GuardarComposicionNow();
    void GuardarComposicionDirectamente();

    void UpdateTablaImprimir();
    void Update_TablaImprimir(QString value);
    void AbrirTablaImprimir();

    void Comprobar_Campos_Variables();
    void CargaDatosCamposVariables();
    void GuardaDatosCamposVariables();

    void InsertarEnColaImpresion();
    void CheckConfigCampos();
    void LoadBBDD(QString rutadb);
    void CargaBBDD();
    void AbrirBBDD();
    void LoadNombresBBDD();
    void CopiarBBDD();
    void sltPrevisualizar();

    void EnableWidgetsCampo1();
    void DisableWidgetsCampo1();
    void EnableWidgetsCampo2();
    void DisableWidgetsCampo2();
    void EnableWidgetsCampo3();
    void DisableWidgetsCampo3();
    void EnableWidgetsCampo4();
    void DisableWidgetsCampo4();

    void EnableWidgetsCampoVar1();
    void DisableWidgetsCampoVar1();
    void EnableWidgetsCampoVar2();
    void DisableWidgetsCampoVar2();
    void EnableWidgetsCampoVar3();
    void DisableWidgetsCampoVar3();
    void EnableWidgetsCampoVar4();
    void DisableWidgetsCampoVar4();
    void EnableWidgetsCampoVar5();
    void DisableWidgetsCampoVar5();
    void EnableWidgetsCampoVar6();
    void DisableWidgetsCampoVar6();
    void visualizarIni();
    void sltPrevisualizarZoom();
    void sltChangeTab(int index);

public slots:


private:
    Ui::ComposicionUI *ui;
    InkjetCommon *DataCommon;
    int m_NumCabezales;

	//Temporizador de eventos
	QTimer *m_Temporizador;

    QImage m_genImageToSend;
	QPixmap m_PixmapUnitario;
    QList<QPixmap> qgenpixMapArray;//Lista de pixmaps a generar para el caso de monocromo


    QList<QPixmap> pixMapArray[NUM_COLORES]; //Lista de pixmaps a generar para el caso de colores plenos
    QPixmap PixmapUnitario[NUM_COLORES];
    QImage genImageToSend[NUM_COLORES];

    /*QPalette dummyPaletteDisabled;
    QPalette dummyPaletteEnabled;*/

signals:
    void SendDebugInfo(unsigned char code, QString msg);
    void SendInformerInfo(unsigned char code, QString msg);
};

#endif // DBIMPRESION_H
