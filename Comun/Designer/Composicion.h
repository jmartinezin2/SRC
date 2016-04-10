/** @addtogroup Composicion
 *  @{*/
/** @file Composicion.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Pantalla de parametros de impresion
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: Composicion.h
 *    @li Versión: x.0
 *    @li Fecha: 05/05/14
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef COMPOSICION_H
#define COMPOSICION_H

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
#include "in2graphicspixmapitem.h"
#include "in2graphicsscene.h"
#include "in2graphicsview.h"

class Composicion : /*public InkjetForm*/ public QWidget
{
	Q_OBJECT

public:
	//Constructor
	Composicion(QWidget *parent);
    //Establece el parent
    void setIn2Parent(QWidget *p);
	//Destructor
	~Composicion();

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
    void LaunchEditImagenFija();
    void LaunchEditCampos();
    void LaunchEditComposicion();
    void sltColaImpresion();
    void sltEnableGrid(bool enable);
    void sltConfigApp();
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
	void CargaPlantilla();
	void CargarComposicion();
	void GuardarComposicion();
    void NuevaComposicion();
	int CuadroDialogoGuardarComposicion();
    int CuadroDialogoNuevaComposicion();
    bool Comprobar_Campos_Vacios();
    bool Comprobar_Formatos_Especiales();
    bool Comprobar_OrdenFabricacion_Vacia();
    bool Comprobar_ExisteComposicion();
    void ComprobacionesGenerateImage();

    void GuardarComposicionNow();
    void GuardarComposicionDirectamente();
    void GuardaCompoLocal();

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
    void sltRefreshZoom();
    void sltChangeTab(int index);
    void sltRefreshScreen();
    void sltLaunchInkCalculator();
    void sltChangeSabreIndex();
    void sltTestERP();
    void sltConfigVision();
    void sltUpdateLog(QString txt);
    void sltShowVersion();
    void sltUpdateAlgImage1();
    void sltUpdateAlgImage2();
    void sltUpdateAlgImage3();
    void sltUpdateAlgImage4();

    void sltConfigureDith1();
    void sltConfigureDith2();
    void sltConfigureDith3();
    void sltConfigureDith4();

    void sltRefreshDitherInfo1(struct m_Colores & color);
    void sltRefreshDitherInfo2(struct m_Colores & color);
    void sltRefreshDitherInfo3(struct m_Colores & color);
    void sltRefreshDitherInfo4(struct m_Colores & color);
    //Ponemos una barra de progreso
    void sltUpdatePb(int value, int total);
    void sltResetPb();

public slots:


private:
    Ui::ComposicionUI *ui;
    //InkjetCommon *DataCommon;
    int m_NumCabezales;

	//Temporizador de eventos
	QTimer *m_Temporizador;

    QImage m_genImageToSend;
	QPixmap m_PixmapUnitario;
    QList<QPixmap> qgenpixMapArray;//Lista de pixmaps a generar para el caso de monocromo


    QList<QPixmap> pixMapArray[NUM_COLORES]; //Lista de pixmaps a generar para el caso de colores plenos
    QPixmap PixmapUnitario[NUM_COLORES];
    QImage genImageToSend[NUM_COLORES];

    In2GraphicsScene *m_scene;
    In2GraphicsView *m_view;
    MemoryMap *m_memoryMap;
    static QWidget *m_parent;

signals:
    void SendDebugInfo(unsigned char code, QString msg);
    void SendInformerInfo(unsigned char code, QString msg);
    void finishDesign();
    void sgnImageGenerated();
};

#endif // COMPOSICION_H
