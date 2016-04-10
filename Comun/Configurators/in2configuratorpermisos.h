#ifndef IN2CONFIGURATORPERMISOS_H
#define IN2CONFIGURATORPERMISOS_H

#include <QString>
#include <QVector>
#include <QHash>
#include <QSettings>

//Para la pantalla Principal
#define PRMS_GESTION   "Gestion"
#define PRMS_MAQUINA   "Maquina"
#define PRMS_CHILLER   "Chiller"
#define PRMS_FLUIDICA  "Fluidica"
#define PRMS_COMPOSICION   "Composicion"
#define PRMS_LOG       "Log"
#define PRMS_USUARIO   "Usuario"
#define PRMS_SALIR     "Salir"

//Para la pantalla Maquina
#define PRMS_JOGMAS          "JogMas"
#define PRMS_JOGMENOS        "JogMenos"
#define PRMS_CORTE           "Corte"
#define PRMS_TENSIONMAS      "TensionMas"
#define PRMS_TENSIONMENOS    "TensionMenos"
#define PRMS_AUTOTENSION     "AutoTension"
#define PRMS_BUSQMACULA      "BusqMacula"
#define PRMS_CAMBIOBOBINA    "CambioBobina"
#define PRMS_TRABAJONUEVO    "TrabajoNuevo"
#define PRMS_AVANZADA        "Avanzada"
#define PRMS_COLAIMPRESION   "ColaImpresion"
#define PRMS_ENVIARCOMPO     "EnviarCompo"
#define PRMS_INSPECCION      "Inspeccion"
#define PRMS_FLUIDICA        "Fluidica"
#define PRMS_CHILLER         "Chiller"
#define PRMS_LOG             "Log"
#define CERRARTRABAJO   "CerrarTrabajo"
#define VISIBLECERRARTRABAJO    "VisibleCerrarTrabajo"
#define PRMS_EXIT            "Exit"
#define PRMS_LOADFILE        "LoadFile"
#define PRMS_CAMBIOCAMPOS    "CambioCampos"
#define PRMS_CAMBIOTINTAS    "CambioTintas"
#define PRMS_ABRIRINI        "AbrirIni"
#define PRMS_CONFIGPRINT     "ConfigPrint"
#define PRMS_CONFIGVISION    "ConfigVision"
#define PRMS_CONFIGLAMPARA   "ConfigLampara"
#define PRMS_CONFIGCORTE     "ConfigCorte"
#define PRMS_CHANGEUSER      "ChangeUser"
#define PRMS_INFO            "Info"


class In2ConfiguratorPermisos
{
public:
    In2ConfiguratorPermisos();
    bool permiso(QString key);
    bool loadPermisos(QString pantalla);

private:
    unsigned char _searchPermiso(QString key);

private:
    QHash<QString,unsigned char> m_listaPermisos;
   // QHash<QString,unsigned char> m_listaMaquina;


};

#endif // IN2CONFIGURATORPERMISOS_H
