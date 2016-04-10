/** @addtogroup MemoryMap
 *  @{*/
/** @file MemoryMap.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2 Printing Solutions
 *
 *    @brief Mapa de memoria para la aplicación inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: MemoryMap.h
 *    @li Versión: x.0
 *    @li Fecha: 30/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#pragma once

#include <QDate>
#include <QPixmap>
#include <QString>
#include <QRgb>
#include <QMap>
#include <QMetaType>
#include <QLocale>

//#include "XCL_Library.h"
#include "ScorpionDLL-ext.h"
#include "qzint.h"

#define NUM_CAMPOS                  4
#define NUM_CAMPOS_VARIABLES        6
#define NUM_MATRIZ_POR_CAMPO        2

//#define NUM_ALARMAS                 1
#define NUM_COLORES                 4

//#define NUM_XUSB                    12
//#define NUM_PH_PER_XUSB             8
//#define NUM_PH_TOTAL                NUM_XUSB*NUM_PH_PER_XUSB

#define MAXHYDRAS                   6

#define FLUIDICA_HYDRA              0
#define FLUIDICA_EIK                1

#define CAMPOS_LEN			25
#define RUTA_LEN			512
#define DEFAULT_FONT_SIZE 8

//Modos de acceso
#define ACCESS_STRANGE 0
#define ACCESS_PRINTER 1
#define ACCESS_ROOT 2
#define ACCESS_MAINTENANCE 3
#define ACCESS_GOD 4



#define STR_PRINTER "Printer"
#define STR_STRANGE "Intruso"
#define STR_ROOT    "Administrador"
#define STR_MAINTENANCE "Mantenimiento"
#define STR_GOD "SuperUser"

#define TEMP_PATH "tmp"

#define DEFAULT_NOMASK_PIXEL 0xFFFFFFFF
//Tipos de corte
#define CUT_LINEAL 0
#define CUT_MEDIAN 1
#define CUT_IN2    2

//Estados de la política de passwords
#define  PASSWORD_POLICY_OK 0
#define  PASSWORD_POLICY_LEN 8
#define  PASSWORD_NUMBERS_AND_CHARS -1

//Algoritmos de seleccion de color
#define SELECCION_MANUAL 0
#define SELECCION_AUTOMATICA 1

//Proceso que se da mientras no se imprime
#define OFFLINE_PROC "OFFLINE"

//Estados del sistema
enum ESTADOS{
	ST_DESCONECTADA=0,
	ST_CONECTADA,
	ST_TRANSMITIENDO,
    ST_REPOSO,
    ST_IMPRIMIENDO,
	ST_ERROR
};
struct UserLog{
    QString m_user;
    QString m_Password;
    int m_derechos;
    QString m_cliente;
};
struct FiltroMacula{
    bool bMonitorMacula;
    double fDistancia;
    double fError;
    double fMax;
    double fMin;
    double fMaxFiltered;
    double fMinFiltered;
};

struct Impresion{
    bool bEscalaGrises;
    bool bReverse;
    bool bHighSpeed;
    bool bMonitorHead;    
    bool bMonitorVision;
    struct FiltroMacula m_fltrMacula;
    int iVelocidad;
    QString qstrPal1;
    QString qstrPal2;
    QString qstrPal3;
    QString qstrPal4;

};
struct m_BarcodeParameters{
    int codificacion;
    int inputMode;
    int option2;
    int width;
    int height;
    double width_mm;
    double height_mm;
    int borderWidth;
    int borderType;
    int whitespace;
    bool hideText;
    QString fuente;
    QString texto;
    int fontSizeSmall;
    int fontSizeLarge;
    QColor FgColor;
    int indiceFgColor;
    QColor BgColor;
    int indiceBgColor;
    QString lastError;
    int escala;
    unsigned char CampoFijo;
    unsigned char NumeroCampo;
    Zint::QZint::AspectRatioMode ar;
    int textAlignement;
    QString fontType;
    int nivelSeguridad;
};


struct strcMarca{
    double MarcaSupX;
    double MarcaSupY;
    double MarcaInfX;
    double MarcaInfY;
    bool MarcaSupOnOff;
    bool MarcaInfOnOff;
    unsigned int MarcaSupRepeticiones;
    double MarcaSupFrecuencia;
    unsigned int MarcaInfRepeticiones;
    double MarcaInfFrecuencia;
    unsigned char MarcaSupColor;
    unsigned char MarcaInfColor;
    unsigned char MarcaSupTipo;
    unsigned char MarcaInfTipo;
    int anguloSup;
    int anguloInf;
    QString MarcaSupValor;
    QString MarcaInfValor;
    QString MarcaSupFuente;
    QString MarcaIInfFuente;
    QString MarcaSupTipoLetraCampo;
    QString MarcaInfTipoLetraCampo;
    QString MarcaSupEstiloLetraCampo;
    QString MarcaInfEstiloLetraCampo;
    int MarcaSupLetterSpace;
    int MarcaInfLetterSpace;
    unsigned int MarcaSupTamanoFuente;
    unsigned int MarcaInfTamanoFuente;
    struct m_BarcodeParameters barcodeSup;
    struct m_BarcodeParameters barcodeInf;
};

//Variables globales de la inkjet
struct m_Variables{
    unsigned char SalvarFicheroConfiguracion;
    unsigned char DisablePrinting;
    unsigned int ImpresionesPorEnvioFilas;
	unsigned int ImpresionesPorEnvioColumnas;
    bool bUseLocalCopy;
    bool bReloadCompo;
    //LAS RUTAS DEL BMP Y DEL INI SON LAS MISMAS
    //LA RUTA DEL PDF ES DISTINTA
    //Guardo datos
    //Datos del ini
    QString defaultImagePath;
    QString savedNombreIni;
    QString savedRutaIni;
    QString savedNombreRutaIni;
	//Datos del ini
    QString CreadoPor;
    QString CreadoCuando;
    QString ModificadoPor;
    QString ModificadoCuando;
    //Rutas
    QString NombreIni;
    QString RutaIni;
    QString NombreRutaIni;
	//Datos del pdf
    QString NombrePdf;
    QString RutaPdf;
    QString NombreRutaPdf;
    //Datos del bmp generado
    QString NombreBmp;
    QString RutaBmp;
    QString NombreRutaBmp;
    //Datos del bmp enviado desde Trabajos Impresion
    QString NombreBmpEnviado;
    QString RutaBmpEnviado;
    QString NombreRutaBmpEnviado;
    QString NombreArchivoPost;
    QString VolumenGota[MAXSCORPIONS];
    unsigned char version_campos;
    bool bDilateAlgorithm;
    int iSabreAlgorithm;
    //Esta es la zona de plantilla
    bool bPlantEnabled;
    double fPlantAncho;
    double fPlantAlto;
    double fPlantOffsetX;
    double fPlantOffsetY;
    double fPlantDistCols;
    int iPlantCols;
    QStringList strPlantImages;
    double WebWidth;
	double WebHeight;
    QString Comentario;
    int AnguloCorte;
    float AnguloPlantilla;
	unsigned int ImpresionesPorEnvio;
	unsigned int FilasPlantilla;
	unsigned int ColumnasPlantilla;
	double XinicialPlantilla;
	double YinicialPlantilla;
    double AnchoPlantilla;
    double AltoPlantilla;
    bool AlgdobleRes;
    int iBitsDithering;
	double SeparacionX;
	double SeparacionY;
	float FactorEscala;
	double PuntoInicialX;
	double PuntoInicialY;
	double MaculaIzqdaX;
	double MaculaIzqdaY;
	double MaculaIzqdaAncho;
	double MaculaIzqdaAlto;
	double MaculaDchaX;
	double MaculaDchaY;
	double MaculaDchaAncho;
	double MaculaDchaAlto;
	unsigned char MaculaIzqdaOnOff;
	unsigned char MaculaDchaOnOff;
    unsigned int MaculaDchaRepeticiones;
    double MaculaDchaFrecuencia;
    unsigned int MaculaIzqdaRepeticiones;
    double MaculaIzqdaFrecuencia;
    unsigned char MaculaIzqdaColor;
    unsigned char MaculaDchaColor;
    unsigned char NumColores;
    int AlgSelColor;
    int iGrayTh;
    QString TipoColor; //Si es cuatricomía o colores plenos
    QString OrdenFabricacion;
    QString Proceso;
    QString Producto;
    int DefaultNumHeads;
    bool algoritmoSuperposicion;

};

struct m_SAI{
    bool testSai;
    QString tokenFile;
};

//Estructura de la informacion de las matricees los campos a imprimir
struct m_Matrices{
    unsigned char OnOffMatriz;
    double PosicionXCampo;
    double PosicionYCampo;
    int OrientacionCampo;
    unsigned int FilasMatrizCampo;
    unsigned int ColumnasMatrizCampo;
    double FreqFilasMatrizCampo;
    double FreqColumnasMatrizCampo;
    double incFilas;
    double incColumnas;
    QString ExcepcionesMatrizCampo;
};

struct FormatoNumero{
    bool zeroPadding;
    bool bFormatEnabled;
    QLocale::Language FormatSeparador;
    int numeroCifras;
};

struct m_CounterParameters{
    unsigned char numeroCampo;
    qint64 valorInicial;
    qint64 valorInicialFila;
    qint64 valorActual;
    qint64 limiteSuperior;
    qint64 limiteInferior;
    qint64 incremento;
    qint64 offset;
    QString configuracion;
    qint64 indexConfiguracion;
    int numeroCifras;
    struct FormatoNumero m_format;

};


struct m_RandomNumberParameters{
    unsigned char numeroCampo;
    int valorActual;
    int limiteSuperior;
    int limiteInferior;
    int semilla;
    int numeroCifras;
    QString formato;
};

struct m_DateTimeParameters{
    QString formato;
    QDateTime datetime;
    QDate date;
    QTime time;
    int numeroCampo;
};

//Estructura de la informacion de los campos a imprimir
struct m_Campos{
    QString NombreCampo;
    QString ValorActualCampo;
    QString TipoCampo;
	unsigned char OnOffCampo;
    //unsigned char OrdenCampo;
    unsigned int TamanoFuenteCampo;
    int letterSpace;
    QString TipoLetraCampo;
    QString EstiloLetraCampo;
    unsigned char NumCaracteresCampo;
    unsigned char color;
    struct m_Matrices m_matrices[NUM_MATRIZ_POR_CAMPO];
    struct m_BarcodeParameters m_barcodeParameters;
    struct m_DateTimeParameters m_dateTimeParameters;
};


//Estructura de la informacion de los campos a imprimir
struct m_CamposVariables{
    QString NombreCampo;
    QString ValorActualCampo;
    unsigned char OnOffCampo;
    QString TipoCampo; //Si es registro de db, contador
    QString OrigenValorCampo; //Si es registro de db, contador
    unsigned char indiceOrigenCampo;
    unsigned char OrdenCampo;
    unsigned int TamanoFuenteCampo;
    int letterSpace;
    QString TipoLetraCampo;
    QString EstiloLetraCampo;
    unsigned char NumCaracteresCampo;
    unsigned char color;
    unsigned char IndiceCampoEnDB;
    struct m_Matrices m_matrices[NUM_MATRIZ_POR_CAMPO];
    //En caso de que sea tipo contador;
    //int LimiteInferior;
    //int LimiteSuperior;
    //int ValorInicial;
    //int Incremento;
    struct m_BarcodeParameters m_barcodeParameters;
    struct m_CounterParameters m_counterParameters;
    struct m_DateTimeParameters m_dateTimeParameters;
    struct m_RandomNumberParameters m_randomNumberParameters;
};


//Estructurade la información de los colores seleccionados
struct m_Colores{
    int index;
    QRgb valor;
    unsigned int x_pix;
    unsigned int y_pix;
    QString cadena;
    bool enabled;
    QString alg;
    bool bWhiteCorr;
    bool bBlackCorr;
    bool bMonoMode;
    bool bMonoCorr;
    bool bCorrDinamica;
    int m_diherCut;
    QString m_ditherPal;
    bool m_customPal;

};

//Estructura de la informacion de las variables de máquina
//struct m_Alarmas{
//    char Fecha[CAMPOS_LEN];
//    QDate Fechaq;
//};

//Estructura de la informacion de los XUSB conectados
struct m_XUSBs{
    QString SN;
    unsigned char estado;
};

struct m_OPC{
    QString Nombre;
    unsigned char estado;
};

//Estructura de fluidica
struct m_Fluidica{
    unsigned char Conectado;
    unsigned char Estado;
    unsigned char Tipo;
    unsigned char NumEquipos;
    int NumConfigured;
    QStringList Descripcion;
    int ID[MAXHYDRAS];
    int Modo[MAXHYDRAS];
    int error;
    int errormode[MAXHYDRAS];
    float Supply_Temp[MAXHYDRAS];
    float Level_Res[MAXHYDRAS];
    float Supply_Pr[MAXHYDRAS];
    float Vaccuum_Pr[MAXHYDRAS];
    float Pressure_Pr[MAXHYDRAS];
    float Return_Temp[MAXHYDRAS];
    float Res_Temp[MAXHYDRAS];
    short Buffer_Nivel[MAXHYDRAS][5];
    //unsigned char index_nivel_Hydra;
    short Nivel_filtrado[MAXHYDRAS];
    float Nivel_filtrado_f[MAXHYDRAS];
    float supply_offset[MAXHYDRAS];
    float return_offset[MAXHYDRAS];
    QColor color[MAXHYDRAS];
    short switches[MAXHYDRAS];
    bool Used[MAXHYDRAS];
    short bits[MAXHYDRAS];
    short fault[MAXHYDRAS];
    bool bAutostart[MAXHYDRAS];
    bool bAutoShutdown[MAXHYDRAS];
};

struct m_Maquina{
    unsigned char EstadoMaquina;
    unsigned char VersionMaquina;
    unsigned char MaquinaEnAutomatico;
    unsigned char InicioSecuenciaAutomatico;
    unsigned char EstadoSecuenciaAutomatico;
    unsigned char InicioSecuenciaEmpalme;
    unsigned char EstadoSecuenciaEmpalme;
    unsigned char InicioSecuenciaBobinaNueva;
    unsigned char EstadoSecuenciaBobinaNueva;
    bool EmpalmePerformed;
    bool ParadaDiametro;
    signed short prev_ref_velocidad;
    signed short ref_velocidad;
    signed short ref_velocidad_actual;
    signed short velocidad;
    signed short contador_velocidad;
    signed short metros_Offset;
    signed short ref_metros_trabajo;

    signed short metros_trabajo;
    int iRefCounter;
    int iCounter;
    int iTCounter;
    int iOffsetCounter;
    signed short ref_diametro_bobina_entrada;
    signed short ref_diametro_bobina_salida;
    signed short diametro_bobina_entrada;
    signed short diametro_bobina_salida;
    signed short diametro_bobina_salida_inst;
    signed short diametro_bobina_entrada_inst;
    signed short diametro_parar;
    signed short iContadorParada;
    unsigned int iMaxMacula;
    unsigned int iMinMacula;
    unsigned int iContadorMaculas;

    signed short temperatura_lampara;
    signed short intensidad_lampara;
    int modoParo;
    bool balluf_centrado;
    bool secuencia_on;
    bool lampara_on;
    signed short ContadorTempLampara;
    bool condiciones_impresion;
    bool impresion;
    bool condiciones_movimiento;
    unsigned int codigo_parada;
    bool softstop_realizado;
    long seg_lampara_on;
    bool softstopear;
    bool stopear;
    bool stop_realizado;
    bool jogmas_conlampara;
    bool jogmenos_conlampara;
    int HorasMaximasUsoLampara;
    int TemperaturaMaximaLampara;
    int DiasMaximosLimpiezaLampara;
    int DiasMaximosUsoFiltro;
    int AnchoBobina;
    struct m_XUSBs m_xusbs[MAXSCORPIONS];
    struct m_OPC m_opc;
    struct m_Fluidica m_fluidica;
    QMap<int,int> qmDiamPresion;
};

struct strcVision{
    unsigned int cErrores;
    unsigned int cErroresConsecutivos;
    unsigned int iPrograma;
};

//Estructura de la configuracion general de la máquina
struct m_Config{
    QString MotorImpresion;
    QString MotorFluidica;
    bool OPCEnable;
    QString OPCLibreria;
    unsigned char NumColores;
    unsigned char BitDepth;
    QString TipoColor; //si es cuatricomía o colores plenos
    bool TecladoVirtualEnable;
    unsigned char RegistroUsuario;
    bool m_LDAPActive;

};

//Estructura de la base de dator que contienen los datos variables
struct m_DataBase{
    QString NombreDb;
    QString RutaDb;
    QString NombreRutaDb;
    QString NombreRutaDbIntermedia;
    QString NombreTablaDatosImprimir;
    QString NombreColaImpresion;
    QString RutaColaImpresion;
    unsigned int FilasTotalesDB;
    unsigned int ImpresionesEnUltimoEnvio;
    unsigned int ImpresionesPorEnvio;
    unsigned int ResultadosTotalesDB;
    unsigned int NumCamposTotalesDB;
    unsigned int EnviosNecesariosDB;
    unsigned char PermitirRepeticiones;
};
//Aqui se cargaran los valores por defecto de la aplicación si se necesitasen
struct m_MachineDefaults{
    double WebWidth;
    double WebHeight;
    bool MaculaTrasPrint;
    int DistanciaStop;
    int VelArranque;
    int TiempoArranque;
    int JointTime;
    int ControlHead;
    bool filtroMaculas;
    int delayedMonitorTime;
    int DiametroMinimoEmpalme;
    int DiametroMinimoBobinaEntrada;
    int DiametroMaximoBobinaEntrada;
    int DiametroMaximoBobinaSalida;
    int DiametroMinimoBobinaSalida;
    int DiametroMinimoBultoEntrada;
    int DiametroMinimoBultoSalida;
};

//Unico mapa de memoria de la aplicacion
struct Mapa{
    struct m_Config m_config;
	struct m_Variables m_variables;
	struct m_Campos m_campos[NUM_CAMPOS];
    struct m_CamposVariables m_camposvariables[NUM_CAMPOS_VARIABLES];
    struct m_DataBase m_database;
	struct m_Maquina m_maquina;
    struct m_Colores m_colores[NUM_COLORES];
    struct m_Colores m_ColorMascara;
    struct m_SAI m_sai;
    struct m_MachineDefaults m_machinedefaults;
    struct Impresion m_impresion;
    struct strcVision m_Vision;
    struct strcMarca m_Marca;

};

class MemoryMap:public QObject
{
    Q_OBJECT
public:
    static MemoryMap *request(QObject *parent);
	//Inicializa Mapa de memoria
    void Init();
    void InitComposicion();
	//Carga una imagen a partir de file
	bool setImagen(QString file);
	//Actualiza la variable del mapa que le digamos
	void SetVar(int x, QString Valor);
	//Devuelve el valor de la variable que le digamos
	QString GetVar(int index);
	//Guarda en un fichero el mapa de memoria actual
	int SaveMemoryMap();
	//Carga el fichero de memoria
	int LoadMemoryMap();
    //Devuelve el numero de campos variables
    int getNumCamposVariables();
    //Devuelve el numero de matrices por campo
    int getNumMatricesCampoVariable(int index);
    //Devuelve el cliente
    QString getClient();
    //Devuelve el user
    QString getUser();
    //Devuelve el Password
    QString getPassword();
    //Almacena User y Password
    void setUserAndPassword(const QString & user, const QString &password);
    //Solo user
    void setClient(const QString & client);
    //Solo user
    void setUser(const QString & user);
    //Establece los derechos de usuario
    void setUserRights(int derechos);
    //Devuelve los derechos de usuario
    int getUserRights();
    // Devuelve los derechos de usuario en texto*/
    QString getUserRightsText();
    //Devuelve la orden de fabricacion+proiducto
    QString getOrdenAndProducto();
    //Salta a la zona tempral
    void JumpToTempWorld();
    //Salta a la zona original
    void JumpToOriginalWorld();
    //Cambia los nombres de la zona temporal
    void ReturnToTempWorld();
    //Cambia los nombres de la zona original
    void ReturnToOriginalWorld();
    //Indica si se usa copia local en diseño
    bool useLocalCopy();
    //Devuelve el path a buscar las imagenes
    QString getImagePath();
    //Guarda quien crea la compo
    void SetCredentialsCreated();
    //Guarda quien modifica la compo
    void SetCredentialsModified();
    //Testea la política de passwords
    int TestPasswordPolicy();
    //Establece offline mode
    void setOfflineMode();
    //Devueleve el indice del campo x
    int getIndexFixedData(QString val);
    //Devuelve el indice del campo fijo más cercano
    int getClosestFixedField(float xPos, float yPos);

private:
    MemoryMap(QObject *parent = 0);




public:
    QString m_DebugLine;
    //Path de la ultima imagen cargada
	QString m_FilePath;
	//Indica si se ha cargado con exito un fichero de configuracion
	bool m_ConfiguracionCargada;
	//Indica si se ha generado una composicion
	bool m_ComposicionGenerada;
    //Indica si se ha enviado una imagen
    bool m_ImagenEnviada;
    //Indica si lo ultimo que hemos hecho ha sido crear composicion o generar imagen
    unsigned char m_UltimaAccion;
    //Imagen a imprimir
	QPixmap m_Imagen;
    //QImage m_ImagenFijaEnviar;
    //Para saber si la imagen a enviar contiene información variable
    unsigned char ImagenVariable;
    //Unico mapa accesible
	struct Mapa m_mapa;

signals:
    void SendDebugInfo(unsigned char code, QString msg);

private:
    struct UserLog m_log;
    static MemoryMap *mInstance;
public:
	~MemoryMap(void);
};

#endif
