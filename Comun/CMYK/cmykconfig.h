#ifndef CMYKCONFIG_H
#define CMYKCONFIG_H

#include <QObject>
#include <QFlags>

class CMYKConfig : public QObject
{
    Q_OBJECT
public:
    explicit CMYKConfig(QObject *parent = 0);
    void saveConfigFile();
    
signals:
    
private:
    QString m_qstrEfectiveCall;
    QString m_qstrIccFile;
    QString m_qstrInputFile;
    int m_width;
    int m_height;
    QString m_qstrgspath;
    QString m_qstrOutFile;
    int m_CyanIndex;
    int m_MagentaIndex;
    int m_YellowIndex;
    int m_BlackIndex;
    int m_Black2Index;
    int m_WhiteIndex;
    int m_QtConversionFlag;
    int m_CalculationFormat;
    bool m_bAlgMejoraBlancos;
    int m_sabreAngle;

private:
    void setOutPutFile();

public slots:
    void setIccFile(const QString & qstr);
    QString getIccFile();
    void setInputFile(const QString &qstr);
    QString getInputFile();
    void setEfectiveCall();
    void setEfectiveCall(const QString & qstr);
    QString getEfectiveCall();
    void setDpiWidth(int i);
    int getDpiWidth();
    void setDpiHeight(int i);
    int getDpiHeight();
    int execute();
    QString getOutPutFile();
    int getCyanIndex();
    int getMagentaIndex();
    int getYellowIndex();
    int getBlackIndex();
    int getBlack2Index();
    int getWhiteIndex();
    void setWhiteIndex(int index);
    void setCyanIndex(int index);
    void setMagentaIndex(int index);
    void setYellowIndex(int index);
    void setBlackIndex(int index);
    void setBlack2Index(int index);
    int getQtConversionFlag();
    void setQtConversionFlag(int flags);
    int getCalcFormat();
    void setCalcFormat(int flags);
    bool getAlgMejoraBlancos();
    void setAlgMejoraBlancos(bool val);
    void setSabreAngle(int angle);
    int getSabreAngle();



signals:
    void SendDebugInfo(unsigned char code, QString msg);

};

#endif // CMYKCONFIG_H
