/** @addtogroup in2CryptoHash
 *  @{*/
/** @file in2CryptoHash.cpp
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright Cemitec. Línea Tecnológica: Diseño Electrónico
 *
 *    @brief Dialogo principal para la aplicación inkjet
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Fichero: in2CryptoHash.cpp
 *    @li Versión: x.0
 *    @li Fecha: 01/04/08
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */
#include <QStringList>
#include <string.h>
#include <QPalette>
#include <windows.h>
#include <QDebug>

#include <QCryptographicHash>

#include "in2CryptoHash.h"

//Constructor 
in2CryptoHash::in2CryptoHash(QWidget *parent)
{
    resetHash();
}

in2CryptoHash::~in2CryptoHash(){

}


void in2CryptoHash::resetHash(){

    tmp.clear();
    numeroHash.clear();
    hexHash.clear();
}



QString in2CryptoHash::getHash(){
    QString tmpHash;
    QCryptographicHash m_Hash(QCryptographicHash::Md4);
    numeroHash.clear();
    hexHash.clear();
    m_Hash.addData(tmp);
    numeroHash=m_Hash.result();
    tmpHash=numeroHash.toHex();
    hexHash=QString("%1").arg(tmpHash);
    return tmpHash;

    //MD4
    //c42139f062322c75a41f007b9b5d8e1f
    //c42139f062322c75a41f007b9b5d8e1f

    //MD5
    //d4a56880a0fe64c244dfa1310ef37ec3
    //d4a56880a0fe64c244dfa1310ef37ec3
}


QString in2CryptoHash::getHash(unsigned char algoritmo){

    QString tmpHash;
    QCryptographicHash m_Hash(QCryptographicHash::Md4);

    numeroHash.clear();
    hexHash.clear();

    switch (algoritmo){
        case CRYPTO_ALGORITMO_MD4:
            numeroHash=m_Hash.hash(tmp,QCryptographicHash::Md4);
            break;
        case CRYPTO_ALGORITMO_MD5:
            numeroHash=m_Hash.hash(tmp,QCryptographicHash::Md5);
            break;
        case CRYPTO_ALGORITMO_SHA1:
            numeroHash=m_Hash.hash(tmp,QCryptographicHash::Sha1);
            break;
        default:
            numeroHash=m_Hash.hash(tmp,QCryptographicHash::Md4);
            break;
    }

    tmpHash=numeroHash.toHex();
    hexHash=QString("%1").arg(tmpHash);
    return tmpHash;
}


void in2CryptoHash::putIntoHash(QByteArray inputData){
   tmp.append(inputData);
}
