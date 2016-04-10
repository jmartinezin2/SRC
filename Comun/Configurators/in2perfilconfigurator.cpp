#include "in2perfilconfigurator.h"
#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include "in2defines.h"


#define ICON    "Icon"
#define DESCRIPTION "Description"
#define CYAN    "CYAN"
#define MAGENTA "MAGENTA"
#define YELLOW  "YELLOW"
#define BLACK   "BLACK"
#define MONO    "MONO"
#define BRILLO  "Brillo"
#define CONTRASTE   "Contraste"
#define EXPOSICION  "Exposicion"
#define PERFIL      "PERFIL"
#define PALETTE     "Palette"
#define ALGORITMO   "Algoritmo"
#define CORRBLANCOS "CorrBlancos"
#define MONOMODE    "MonoMode"
#define CMYKMODE    "CMYKMode"



In2PerfilConfigurator::In2PerfilConfigurator(QObject *parent) : QObject(parent)
{    
    m_file.clear();
}

In2PerfilConfigurator::~In2PerfilConfigurator()
{

}
/** Carga un perfil*/
In2PrintPerfile In2PerfilConfigurator::loadPrintPerfile(QString file){
    In2PrintPerfile perfil;
    if (QFile::exists(file)){
        QSettings FicheroIni(file,QSettings::IniFormat,0);
        QFileInfo info(file);
        QString fString=info.fileName();
        perfil.setName(fString);
        perfil.setValid(true);
        FicheroIni.beginGroup(PERFIL);
        float fVal=FicheroIni.value(EXPOSICION,0).toFloat();
        perfil.setExposicion(fVal);
        fString=FicheroIni.value(ICON,"").toString();
        perfil.setIconName(fString);
        bool bVal=FicheroIni.value(CORRBLANCOS,true).toBool();
        perfil.setCorreccionBlancos(bVal);
        bVal=FicheroIni.value(CMYKMODE,true).toBool();
        perfil.setCMYKMode(bVal);
        fString=FicheroIni.value(ALGORITMO,"Diffuse").toString();
        if (!fString.compare("Ordered")) perfil.setDifAlgoritmo(Qt::OrderedDither);
        else if (!fString.compare("Threshold")) perfil.setDifAlgoritmo(Qt::ThresholdDither);
        else perfil.setDifAlgoritmo(Qt::DiffuseDither);

        fString=FicheroIni.value(DESCRIPTION,"").toString();
        perfil.setDescription(fString);

        FicheroIni.endGroup();

        //CYAN
        FicheroIni.beginGroup(CYAN);
        In2Perfile cyan;
        fString=FicheroIni.value(PALETTE,"").toString();
        QStringList fooList=fString.split("@");
        if (fooList.count()==8){
            QVector<int> fooVector;
            fooVector.resize(8);
            for (int x=0; x<8;x++){
                fooVector.replace(x,fooList.at(x).toInt());
            }
            cyan.setPalette(fooVector);
        }
        int iVal=FicheroIni.value(BRILLO,0).toInt();
        cyan.setBrillo(iVal);
        fVal=FicheroIni.value(CONTRASTE,0).toFloat();
        cyan.setContraste(fVal);
        bVal=FicheroIni.value(CORRBLANCOS,"").toBool();
        cyan.setCorreccionBlancos(bVal);
        fString=FicheroIni.value(ALGORITMO,"Diffuse").toString();
        if (!fString.compare("Ordered")) cyan.setDifAlgorithm(Qt::OrderedDither);
        else if (!fString.compare("Threshold")) cyan.setDifAlgorithm(Qt::ThresholdDither);
        else if (!fString.compare("Floyd")) cyan.setDifAlgorithm(FLOYD_STEINBERG);
        else cyan.setDifAlgorithm(Qt::DiffuseDither);
        bVal=FicheroIni.value(MONOMODE,true).toBool();
        cyan.setMonoMode(bVal);
        perfil.setCyan(cyan);

        FicheroIni.endGroup();

        //MAGENTA
        FicheroIni.beginGroup(MAGENTA);
        In2Perfile magenta;
        fString=FicheroIni.value(PALETTE,"").toString();
        fooList=fString.split("@");
        if (fooList.count()==8){
            QVector<int> fooVector;
            fooVector.resize(8);
            for (int x=0; x<8;x++){
                fooVector.replace(x,fooList.at(x).toInt());
            }
            magenta.setPalette(fooVector);
        }
        iVal=FicheroIni.value(BRILLO,0).toInt();
        magenta.setBrillo(iVal);
        fVal=FicheroIni.value(CONTRASTE,0).toFloat();
        magenta.setContraste(fVal);
        bVal=FicheroIni.value(CORRBLANCOS,"").toBool();
        magenta.setCorreccionBlancos(bVal);
        fString=FicheroIni.value(ALGORITMO,"Diffuse").toString();
        if (!fString.compare("Ordered")) magenta.setDifAlgorithm(Qt::OrderedDither);
        else if (!fString.compare("Threshold")) magenta.setDifAlgorithm(Qt::ThresholdDither);
        else if (!fString.compare("Floyd")) magenta.setDifAlgorithm(FLOYD_STEINBERG);
        else magenta.setDifAlgorithm(Qt::DiffuseDither);
        bVal=FicheroIni.value(MONOMODE,true).toBool();
        magenta.setMonoMode(bVal);
        perfil.setMagenta(magenta);
        FicheroIni.endGroup();

        //YELLOW
        FicheroIni.beginGroup(YELLOW);
        In2Perfile yellow;
        fString=FicheroIni.value(PALETTE,"").toString();
        fooList=fString.split("@");
        if (fooList.count()==8){
            QVector<int> fooVector;
            fooVector.resize(8);
            for (int x=0; x<8;x++){
                fooVector.replace(x,fooList.at(x).toInt());
            }
            yellow.setPalette(fooVector);
        }
        iVal=FicheroIni.value(BRILLO,0).toInt();
        yellow.setBrillo(iVal);
        fVal=FicheroIni.value(CONTRASTE,0).toFloat();
        yellow.setContraste(fVal);
        bVal=FicheroIni.value(CORRBLANCOS,"").toBool();
        yellow.setCorreccionBlancos(bVal);
        fString=FicheroIni.value(ALGORITMO,"Diffuse").toString();
        if (!fString.compare("Ordered")) yellow.setDifAlgorithm(Qt::OrderedDither);
        else if (!fString.compare("Threshold")) yellow.setDifAlgorithm(Qt::ThresholdDither);
        else if (!fString.compare("Floyd")) yellow.setDifAlgorithm(FLOYD_STEINBERG);
        else yellow.setDifAlgorithm(Qt::DiffuseDither);
        bVal=FicheroIni.value(MONOMODE,true).toBool();
        yellow.setMonoMode(bVal);
        perfil.setYellow(yellow);
        FicheroIni.endGroup();

        //BLACK
        FicheroIni.beginGroup(BLACK);
        In2Perfile black;
        fString=FicheroIni.value(PALETTE,"").toString();
        fooList=fString.split("@");
        if (fooList.count()==8){
            QVector<int> fooVector;
            fooVector.resize(8);
            for (int x=0; x<8;x++){
                fooVector.replace(x,fooList.at(x).toInt());
            }
            black.setPalette(fooVector);
        }
        iVal=FicheroIni.value(BRILLO,0).toInt();
        black.setBrillo(iVal);
        fVal=FicheroIni.value(CONTRASTE,0).toFloat();
        black.setContraste(fVal);
        bVal=FicheroIni.value(CORRBLANCOS,"").toBool();
        black.setCorreccionBlancos(bVal);
        fString=FicheroIni.value(ALGORITMO,"Diffuse").toString();
        if (!fString.compare("Ordered")) black.setDifAlgorithm(Qt::OrderedDither);
        else if (!fString.compare("Threshold")) black.setDifAlgorithm(Qt::ThresholdDither);
        else if (!fString.compare("Floyd")) black.setDifAlgorithm(FLOYD_STEINBERG);
        else black.setDifAlgorithm(Qt::DiffuseDither);

        bVal=FicheroIni.value(MONOMODE,true).toBool();
        black.setMonoMode(bVal);
        perfil.setBlack(black);
        FicheroIni.endGroup();

        //MONO
        FicheroIni.beginGroup(MONO);
        In2Perfile mono;
        fString=FicheroIni.value(PALETTE,"").toString();
        fooList=fString.split("@");
        if (fooList.count()==8){
            QVector<int> fooVector;
            fooVector.resize(8);
            for (int x=0; x<8;x++){
                fooVector.replace(x,fooList.at(x).toInt());
            }
            mono.setPalette(fooVector);
        }
        iVal=FicheroIni.value(BRILLO,0).toInt();
        mono.setBrillo(iVal);
        fVal=FicheroIni.value(CONTRASTE,0).toFloat();
        mono.setContraste(fVal);
        bVal=FicheroIni.value(CORRBLANCOS,"").toBool();
        mono.setCorreccionBlancos(bVal);
        fString=FicheroIni.value(ALGORITMO,"Diffuse").toString();
        if (!fString.compare("Ordered")) mono.setDifAlgorithm(Qt::OrderedDither);
        else if (!fString.compare("Threshold")) mono.setDifAlgorithm(Qt::ThresholdDither);
        else if (!fString.compare("Floyd")) mono.setDifAlgorithm(FLOYD_STEINBERG);
        else mono.setDifAlgorithm(Qt::DiffuseDither);

        bVal=FicheroIni.value(MONOMODE,true).toBool();
        mono.setMonoMode(bVal);
        perfil.setMono(mono);
        FicheroIni.endGroup();

    }

    return perfil;

}

/** Guarda el perfil en el fichero que se le diga*/
void In2PerfilConfigurator::savePrintPerfile(QString file, In2PrintPerfile &perfil){

    QSettings FicheroIni(file,QSettings::IniFormat,0);
    FicheroIni.beginGroup(PERFIL);
    FicheroIni.setValue(EXPOSICION,QString("%1").arg(perfil.getExposicion()));
    FicheroIni.setValue(ICON,perfil.getIconName());
    FicheroIni.setValue(CORRBLANCOS,perfil.getCorreccionBlancos());
    FicheroIni.setValue(DESCRIPTION,perfil.description());
    QString fString;
    int foo=perfil.getDifAlgoritmo();
    if (foo==Qt::OrderedDither) FicheroIni.setValue(ALGORITMO,"Ordered");
    else if (foo==Qt::ThresholdDither) FicheroIni.setValue(ALGORITMO,"Threshold");
    else if (foo==Qt::DiffuseDither)  FicheroIni.setValue(ALGORITMO,"Diffuse");
    else FicheroIni.setValue(ALGORITMO,"Diffuse");
    FicheroIni.endGroup();

    //CYAN
    FicheroIni.beginGroup(CYAN);
    In2Perfile cyan=perfil.getCyan();
    QVector<int> pal=cyan.getPalette();
    if (pal.count()==8) fString=QString("%1@%2@%3@%4@%5@%6@%7@%8").arg(QString("%1").arg(pal.at(0)))
                                                                .arg(QString("%1").arg(pal.at(1)))
                                                                .arg(QString("%1").arg(pal.at(2)))
                                                                .arg(QString("%1").arg(pal.at(3)))
                                                                .arg(QString("%1").arg(pal.at(4)))
                                                                .arg(QString("%1").arg(pal.at(5)))
                                                                .arg(QString("%1").arg(pal.at(6)))
                                                                .arg(QString("%1").arg(pal.at(7)));
    else fString=QString("%0@%0@%0@%0@%0@0@%0@0");
    FicheroIni.setValue(PALETTE,fString);
    FicheroIni.setValue(BRILLO,cyan.getBrillo());
    FicheroIni.setValue(CONTRASTE,QString("%1").arg(cyan.getContraste()));
    FicheroIni.setValue(CORRBLANCOS,cyan.getCorreccionBlancos());
    FicheroIni.setValue(MONOMODE,cyan.getMonoMode());
    foo=cyan.getDifAlgorithm();
    if (foo==Qt::OrderedDither) FicheroIni.setValue(ALGORITMO,"Ordered");
    else if (foo==Qt::ThresholdDither) FicheroIni.setValue(ALGORITMO,"Threshold");
    else if (foo==Qt::DiffuseDither)  FicheroIni.setValue(ALGORITMO,"Diffuse");
    else FicheroIni.setValue(ALGORITMO,"Diffuse");
    FicheroIni.endGroup();

    //MAGENTA
    FicheroIni.beginGroup(MAGENTA);
    In2Perfile magenta=perfil.getMagenta();
    pal=magenta.getPalette();
    if (pal.count()==8) fString=QString("%1@%2@%3@%4@%5@%6@%7@%8").arg(QString("%1").arg(pal.at(0)))
                                                                .arg(QString("%1").arg(pal.at(1)))
                                                                .arg(QString("%1").arg(pal.at(2)))
                                                                .arg(QString("%1").arg(pal.at(3)))
                                                                .arg(QString("%1").arg(pal.at(4)))
                                                                .arg(QString("%1").arg(pal.at(5)))
                                                                .arg(QString("%1").arg(pal.at(6)))
                                                                .arg(QString("%1").arg(pal.at(7)));
    else fString=QString("%0@%0@%0@%0@%0@0@%0@0");
    FicheroIni.setValue(PALETTE,fString);
    FicheroIni.setValue(BRILLO,magenta.getBrillo());
    FicheroIni.setValue(CONTRASTE,QString("%1").arg(magenta.getContraste()));
    FicheroIni.setValue(CORRBLANCOS,magenta.getCorreccionBlancos());
    FicheroIni.setValue(MONOMODE,magenta.getMonoMode());
    foo=magenta.getDifAlgorithm();
    if (foo==Qt::OrderedDither) FicheroIni.setValue(ALGORITMO,"Ordered");
    else if (foo==Qt::ThresholdDither) FicheroIni.setValue(ALGORITMO,"Threshold");
    else if (foo==Qt::DiffuseDither)  FicheroIni.setValue(ALGORITMO,"Diffuse");
    else FicheroIni.setValue(ALGORITMO,"Diffuse");
    FicheroIni.endGroup();

    //YELLOW
    FicheroIni.beginGroup(YELLOW);
    In2Perfile yellow=perfil.getYellow();
    pal=yellow.getPalette();
    if (pal.count()==8) fString=QString("%1@%2@%3@%4@%5@%6@%7@%8").arg(QString("%1").arg(pal.at(0)))
                                                                .arg(QString("%1").arg(pal.at(1)))
                                                                .arg(QString("%1").arg(pal.at(2)))
                                                                .arg(QString("%1").arg(pal.at(3)))
                                                                .arg(QString("%1").arg(pal.at(4)))
                                                                .arg(QString("%1").arg(pal.at(5)))
                                                                .arg(QString("%1").arg(pal.at(6)))
                                                                .arg(QString("%1").arg(pal.at(7)));
    else fString=QString("%0@%0@%0@%0@%0@0@%0@0");
    FicheroIni.setValue(PALETTE,fString);
    FicheroIni.setValue(BRILLO,yellow.getBrillo());
    FicheroIni.setValue(CONTRASTE,QString("%1").arg(yellow.getContraste()));
    FicheroIni.setValue(CORRBLANCOS,yellow.getCorreccionBlancos());
    FicheroIni.setValue(MONOMODE,yellow.getMonoMode());
    foo=yellow.getDifAlgorithm();
    if (foo==Qt::OrderedDither) FicheroIni.setValue(ALGORITMO,"Ordered");
    else if (foo==Qt::ThresholdDither) FicheroIni.setValue(ALGORITMO,"Threshold");
    else if (foo==Qt::DiffuseDither)  FicheroIni.setValue(ALGORITMO,"Diffuse");
    else FicheroIni.setValue(ALGORITMO,"Diffuse");
    FicheroIni.endGroup();

    //BLACK
    FicheroIni.beginGroup(BLACK);
    In2Perfile black=perfil.getBlack();
    pal=black.getPalette();
    if (pal.count()==8) fString=QString("%1@%2@%3@%4@%5@%6@%7@%8").arg(QString("%1").arg(pal.at(0)))
                                                                .arg(QString("%1").arg(pal.at(1)))
                                                                .arg(QString("%1").arg(pal.at(2)))
                                                                .arg(QString("%1").arg(pal.at(3)))
                                                                .arg(QString("%1").arg(pal.at(4)))
                                                                .arg(QString("%1").arg(pal.at(5)))
                                                                .arg(QString("%1").arg(pal.at(6)))
                                                                .arg(QString("%1").arg(pal.at(7)));
    else fString=QString("%0@%0@%0@%0@%0@0@%0@0");
    FicheroIni.setValue(PALETTE,fString);
    FicheroIni.setValue(BRILLO,black.getBrillo());
    FicheroIni.setValue(CONTRASTE,QString("%1").arg(black.getContraste()));
    FicheroIni.setValue(CORRBLANCOS,black.getCorreccionBlancos());
    FicheroIni.setValue(MONOMODE,black.getMonoMode());
    foo=black.getDifAlgorithm();
    if (foo==Qt::OrderedDither) FicheroIni.setValue(ALGORITMO,"Ordered");
    else if (foo==Qt::ThresholdDither) FicheroIni.setValue(ALGORITMO,"Threshold");
    else if (foo==Qt::DiffuseDither)  FicheroIni.setValue(ALGORITMO,"Diffuse");
    else FicheroIni.setValue(ALGORITMO,"Diffuse");
    FicheroIni.endGroup();

    FicheroIni.sync();


}



