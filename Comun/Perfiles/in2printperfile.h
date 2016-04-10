#ifndef IN2PRINTPERFILE_H
#define IN2PRINTPERFILE_H


#include "in2perfile.h"
#include <QString>

class In2PrintPerfile
{

public:
    In2PrintPerfile();
    ~In2PrintPerfile();
    In2Perfile getCyan();
    In2Perfile getYellow();
    In2Perfile getBlack();
    In2Perfile getMagenta();
    In2Perfile getMono();
    void setDescription(QString & txt);
    QString description();
    void setCyan(In2Perfile & x);
    void setMagenta(In2Perfile & x);
    void setYellow(In2Perfile & x);
    void setBlack(In2Perfile & x);
    void setMono(In2Perfile &x);
    float getExposicion();
    void setExposicion(float x);
    void setIconName(QString & x);
    void setName(QString &x);
    QString getName();
    QString getIconName();
    bool isValid();
    void setValid(bool x);
    void setCorreccionBlancos(bool val);
    void setDifAlgoritmo(int val);
    bool getCorreccionBlancos();
    int getDifAlgoritmo();
    void setCMYKMode(bool val);
    bool getCMYKMode();

private:
    QString m_name;
    QString m_iconName;
    float m_exposicion;
    In2Perfile m_cyan;
    In2Perfile m_magenta;
    In2Perfile m_yellow;
    In2Perfile m_black;
    In2Perfile m_mono;
    bool m_valid;
    bool m_bCorrBlancos;
    int m_difAlgo;
    bool m_bCMYKMode;
    QString m_Description;


};

#endif // IN2PRINTPERFILE_H
