#ifndef IN2COMPOFILE_H
#define IN2COMPOFILE_H

#include <QString>
#include <QStringList>
#include "MemoryMap.h"

class In2CompoFile
{
public:
    In2CompoFile();
    void SetHashCompoFile();
    QString CalculateHashCompoFile();
    QString GetHashCompoFile();
    void AbrirFicheroIni();
    void GuardarFicheroIni();
    bool isFixedDataActive(QString val);


private:
    QStringList txt_CAMPO;
    QStringList txt_CAMPO_VARIABLE;
    MemoryMap *memoryMap;

};

#endif // IN2COMPOFILE_H
