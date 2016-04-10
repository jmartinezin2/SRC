/**
 *  @{*/
/** @file In2UdpMessages.h
 *
 *    PROYECTO. Interfaz Inkjet ''
 *
 *    (C) Copyright In2Ps
 *
 *    @brief Definicion de mensajes UDP
 *
 *    @li Autores: Jose Mª Martinez
 *    @li Versión: x.0
 *    @li Fecha: 2/01/2013
 *    @li Lenguaje: C++
 *
 *  Revisiones:
 *
 */

#ifndef IN2UDPMESSAGES_H
#define IN2UDPMESSAGES_H

//Pulso de vida
#define UDP_IS_ALIVE	0x00
//Peticion de estado
#define UDP_REQ_STATE	0x01
//Respuesta de estado
#define UDP_ANS_STATE	0x02
//Peticion de ultimo error
#define UDP_GET_LASTERROR	0x03
//Respuesta de ultimo error
#define UDP_ANS_LASTERROR	0x04
//Peticion de informacion de usuario
#define UDP_GET_USERINFO	0x05
//Respuesta de informacion de usuario
#define UDP_ANS_USERINFO	0x06
//Peticion de informacion de fichero actual
#define UDP_GET_CURRENTFILE	0x07
//Respuesta de fichero actual
#define UDP_ANS_CURRENTFILE	0x08
//Peticion de mapa de memoria
#define UDP_GET_MEMORYMAP 0x09
//Respuesta con mapa de memoria
#define UDP_ANS_MEMORYMAP 0x0A

#endif // IN2UDPMESSAGES_H
