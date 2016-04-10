#ifndef IN2HYDRACONTROL_H
#define IN2HYDRACONTROL_H

#include <QObject>
#include <QColor>
#include "XCL_Library.h"

#define FLAG_ERROR_INICIANDO 1

class In2HydraControl : public QObject
{
    Q_OBJECT
public:
    explicit In2HydraControl(QObject *parent = 0,int id=0);
    /** Lo inicializa*/
    void init(int id);
    /** Arranca el hydra*/
    int Start();
    /** Para el hydra*/
    int Stop();
    /** Cierra el hydra*/
    int Close();
    /** Reinicia el hydra*/
    int Restart();
    /** Envía comando*/
    int SendCommand(XCL_Commands command);
    /** Lee el PID*/
    int ReadPid(XCL_Controls Select, XCL_PID *Details);
    /** Lee modo*/
    int ReadMode(XCL_Modes *mode);
    /** Lee coficientes F*/
    int ReadFcoeff(float *coefArray);
    /** Lee coficientes I*/
    int ReadIcoeff(short *coefArray);
    /** Lee coficientes F*/
    int WriteFcoeff(float *coefArray);
    /** Lee coficientes I*/
    int WriteIcoeff(short *coefArray);
    /** Lee el control*/
    int ReadControl(XCL_Controls control,XCL_PID *output);
    /** Escribe el control*/
    int WriteControl(XCL_Controls control,XCL_PID *output);
    /** Lee el sensor*/
    int ReadSensor(XCL_Sensors sensor, XCL_ADC *output);
    /** Escribe el sensor*/
    int WriteSensor(XCL_Sensors sensor, XCL_ADC *output);
    /** Lee los switches*/
    int ReadSwitches(short *Bits);
    /** Lee los LEDs*/
    int ReadLEDS(short *Bits, short *Fault);
    /** Lee el Filtro*/
    int ReadFilter(XCL_Sensors Select, XCL_Filters *TimeConstant);
    /** Escribe el Filtro*/
    int WriteFilter(XCL_Sensors Select, XCL_Filters *TimeConstant);
    /** Escribe la desripcion*/
    int WriteDescription(const QString & desc);
    /** Lee la desripcion*/
    QString ReadDescription();
    /** Lee el Drive*/
    int ReadDrive(XCL_Drives Select, XCL_DAC *Details);
    /** Escribe el Drive*/
    int WriteDrive(XCL_Drives Select, XCL_DAC *Details);
    /** Recoge informacion*/
    int GetInfo(char SerialNum [], char Length, short *CurrentBootBlock);
    /** Recoge descripcion de bloque*/
    int GetBlockDesc(char Descrip [],short Length, short DescBlock);
    /** Lee log*/
    int ReadLog(float *Farray);
    /** Guarda en la Flash*/
    int SaveToFlash(short BlockToWrite, short NewBootBlock, char *NewDescrip,short DescSize);
    /** Guarda en disco*/
    int SaveToDisk(LPCWSTR Filename,LPCWSTR Description,short DescSize);
    /** Carga de disco*/
    int LoadFromDisk(LPCWSTR Filename,LPCWSTR Description,short DescSize);
    /** Devuelve el supplyOfsset*/
    float getSupplyOffset();
    /** Devuelve el returnOfsset*/
    float getReturnOffset();
    /** Devuelve el color del Hydra*/
    QColor getColor();
    /** Establece el autostart*/
    void setAutoStart(bool value);
    /** Establece el used*/
    void setUsed(bool value);
    /** devuelve el autostart*/
    bool getAutoStart();
    /** devuelve el used*/
    bool getUsed();
    /** devuelve el mode*/
    XCL_Modes getMode();
    /** Devuelve la descripcion*/
    QString getDescription();
    /** Activa un flag*/
    void setFlag(char flag,char value);
    /** Devuelve un flag*/
    char getFlag(char flag);
    /** devuelve si puede imprimir o no*/



private:
    int m_ID;
    float m_SupplyOffset;
    float m_ReturnOffset;
    QColor m_ColorHydra;
    XCL_Modes m_mode;
    bool m_used;
    bool m_autostart;
    QString m_Descripcion;
    char m_FlagErrorIniciando;




signals:
    
public slots:

};

#endif // IN2HYDRACONTROL_H
