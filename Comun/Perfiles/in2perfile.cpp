#include "in2perfile.h"

In2Perfile::In2Perfile()
{
    m_brillo=0;
    m_contraste=1;
    m_palette.fill(1,7);
    m_palette.replace(6,0);
    m_CorrBlancos=false;
    m_monoMode=false;
    m_difAlgo=0;
}

In2Perfile::~In2Perfile()
{

}

/** Devuelve la paleta*/
QVector<int> In2Perfile::getPalette(){
    return m_palette;
}
/** Devuelve el brillo*/
int In2Perfile::getBrillo(){
    return m_brillo;
}
/** Devuelve el contraste*/
float In2Perfile::getContraste(){
    return m_contraste;
}
/** Carga el brillo*/
void In2Perfile::setBrillo(int x){
    m_brillo=x;
}
/** Carga el contraste*/
void In2Perfile::setContraste(float x){
    m_contraste=x;
}
/** Carga la paleta*/
void In2Perfile::setPalette(QVector<int> & pal){
    m_palette=pal;
}
/** Carga correccion de blancos*/
void In2Perfile::setCorreccionBlancos(bool val){
    m_CorrBlancos=val;
}
/** Devuelve correccion de blancos*/
bool In2Perfile::getCorreccionBlancos(){
    return m_CorrBlancos;
}
/** Carga correccion de blancos*/
void In2Perfile::setDifAlgorithm(int val){
    m_difAlgo=val;
}
/** Carga correccion de blancos*/
int In2Perfile::getDifAlgorithm(){
    return m_difAlgo;
}
/** Carga MonoMode*/
void In2Perfile::setMonoMode(bool val){
    m_monoMode=val;
}
/** Devuelve el monomode*/
bool In2Perfile::getMonoMode(){
    return m_monoMode;
}
