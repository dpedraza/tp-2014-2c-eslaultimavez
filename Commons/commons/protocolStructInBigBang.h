/*
 * protocolStruct.h
 *
 *  Created on: 26/04/2014
 *      Author: utnso
 */

#ifndef PROTOCOLSTRUCT_H_
#define PROTOCOLSTRUCT_H_

//Handshakes
#define HANDSHAKE_PROGRAMA 1
#define HANDSHAKE_PLANIFICADOR 2
#define HANDSHAKE_LOADER 3
#define HANDSHAKE_MSP 4
#define HANDSHAKE_CPU 5
#define HANDSHAKE_KERNEL 6

//Programa (Para mostrar por consola el mensaje que envia el Kernel)
#define MENSAJE_DE_ERROR 7
#define MENSAJE_SIN_ERROR 8

//Kernel
#define CODIGO_BESO 119
#define FINALIZAR_PROGRAMA_EXITO 120

//MSP
#define CREAR_SEGMENTO 10
#define DESTRUIR_SEGMENTO 11
#define LEER_MEMORIA 12
#define ESCRIBIR_MEMORIA 13

//CPU
#define TCB_NUEVO 20
#define CAMBIO_DE_CONTEXTO 21
#define INTERRUPCION 22
#define CPU_SEGUI_EJECUTANDO 23
#define	KERNEL_FIN_TCB_QUANTUM 24
#define CPU_TERMINE_UNA_LINEA 25
#define ENTRADA_ESTANDAR 26 // PIDE POR CONSOLA DEL PROGRAMA QUE SE INGRESE UN NUMERO
#define SALIDA_ESTANDAR 27 // IMPRIME POR CONSOLA DEL PROGRAMA EL NUMERO INGRESADO
#define SALIDA_ESTANDAR_CHAR 30 // IMPRIME POR CONSOLA DEL PROGRAMA UNA CADENA INGRESADA
#define ENTRADA_ESTANDAR_CHAR 31 // PIDE POR CONSOLA DEL PROGRAMA QUE SE INGRESE UNA CADENA
#define CREAR_HILO 28
#define ACTUALIZAR_STACK 29
#define BLOK_HILO 32
#define JOIN_HILO 33
#define WAKE_HILO 34
#define CREAR_STACK 35 // le pasa al KERNEL el PID y TID del TCB creado y el KERNEL devuelve la Base del Segmento.


//ERRORES POR MSP
#define ERROR_POR_TAMANIO_EXCEDIDO -1
#define ERROR_POR_MEMORIA_LLENA -2
#define ERROR_POR_NUMERO_NEGATIVO -3
#define ERROR_POR_SEGMENTO_INVALIDO -4
#define ERROR_POR_SEGMENTATION_FAULT -5
#define ERROR_POR_SEGMENTO_DESCONOCIDO -6
#define SIN_ERRORES -7


//ERROR DE CPU
#define ERROR_POR_DESCONEXION_DE_CPU 110
#define ERROR_POR_DESCONEXION_DE_CONSOLA 111


#endif /* PROTOCOLSTRUCT_H_ */
