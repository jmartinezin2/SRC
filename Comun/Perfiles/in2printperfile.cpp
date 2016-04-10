#include "in2printperfile.h"

In2PrintPerfile::In2PrintPerfile()
{
    m_exposicion=0;
    m_name.clear();
    m_iconName.clear();
    m_valid=false;
    m_difAlgo=0;
    m_bCorrBlancos=1;
    m_bCMYKMode=true;
    m_Description.clear();
}

In2PrintPerfile::~In2PrintPerfile()
{

}
/** Crga la descripcion*/
void In2PrintPerfile::setDescription(QString &txt){
    m_Description=txt;
}
/** Devulelve la descripcion*/
QString In2PrintPerfile::description(){
    return m_Description;
}

/** establece si el perfil es valido*/
void In2PrintPerfile::setValid(bool x){
    m_valid=x;
}
/** Devuelve si es valido*/
bool In2PrintPerfile::isValid(){
    return m_valid;
}
/** Devuelve el perfil mono*/
In2Perfile In2PrintPerfile::getMono(){
    return m_mono;
}

/** Devuelve el perfil cyan*/
In2Perfile In2PrintPerfile::getCyan(){
    return m_cyan;
}
/** Devuelve el perfil yellow*/
In2Perfile In2PrintPerfile::getYellow(){
    return m_yellow;
}
/** Devuelve el perfil black*/
In2Perfile In2PrintPerfile::getBlack(){
    return m_black;
}
/** Devuelve el perfil magenta*/
In2Perfile In2PrintPerfile::getMagenta(){
    return m_magenta;
}
/** Carga el perfil cyan**/
void In2PrintPerfile::setCyan(In2Perfile & x){
    m_cyan=x;
}
/** Carga el perfil magenta*/
void In2PrintPerfile::setMagenta(In2Perfile & x){
    m_magenta=x;
}
/** Carga el perfil yellow*/
void In2PrintPerfile::setYellow(In2Perfile & x){
    m_yellow=x;
}
/** Carga el perfil black*/
void In2PrintPerfile::setBlack(In2Perfile & x){
    m_black=x;
}
/** Carga el perfil mono*/
void In2PrintPerfile::setMono(In2Perfile & x){
    m_mono=x;
}

/** Devuelve la exposicion*/
float In2PrintPerfile::getExposicion(){
    return m_exposicion;
}
/** Carga la exposicion*/
void In2PrintPerfile::setExposicion(float x){
    m_exposicion=x;
}
/** Guarda el nombre del icono*/
void In2PrintPerfile::setIconName(QString & x){
    m_iconName=x;
}
/** Guarda el nombre del perfil*/
void In2PrintPerfile::setName(QString &x){
    m_name=x;
}
/** Devuelve el nombre del perfil*/
QString In2PrintPerfile::getName(){
    return m_name;
}
/** Devuelve el nombre del icono*/
QString In2PrintPerfile::getIconName(){
    return m_iconName;
}
/** Carga correccion de blancos*/
void In2PrintPerfile::setCorreccionBlancos(bool val){
    m_bCorrBlancos=val;
}
/** Devuelve correccion de blancos*/
bool In2PrintPerfile::getCorreccionBlancos(){
    return m_bCorrBlancos;
}
/** Carga algoritmo de difusion*/
void In2PrintPerfile::setDifAlgoritmo(int val){
    m_difAlgo=val;
}
/** Devuelve algorito de difusion*/
int In2PrintPerfile::getDifAlgoritmo(){
    return m_difAlgo;
}
/** Devuelve si es CMYKMode o Mono Mode*/
bool In2PrintPerfile::getCMYKMode(){
    return m_bCMYKMode;
}
/** Devuelve si es CMYKMode o Mono Mode*/
void In2PrintPerfile::setCMYKMode(bool val){
    m_bCMYKMode=val;
}

