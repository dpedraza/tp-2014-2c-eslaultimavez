/*
 * protocolStruct.h
 *
 *  Created on: 26/04/2014
 *      Author: utnso
 */

#ifndef PROTOCOLSTRUCT_H_
#define PROTOCOLSTRUCT_H_


	//Código operación:       	Payload:
	#define HANDSHAKE_PROGRAMA     1
	#define HANDSHAKE_PLANIFICADOR 2
	#define HANDSHAKE_LOADER 3
	#define HANDSHAKE_MSP 4
	#define HANDSHAKE_CPU 5
	#define HANDSHAKE_KERNEL 6



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
	#define ENTRADA_ESTANDAR 26
	#define SALIDA_ESTANDAR 27
	#define CREAR_HILO 28


	//ERRORES POR MSP
	#define ERROR_POR_TAMANIO_EXCEDIDO  100
	#define ERROR_POR_MEMORIA_LLENA     101
	#define ERROR_POR_NUMERO_NEGATIVO   102
	#define ERROR_POR_SEGMENTO_INVALIDO 103
	#define ERROR_POR_SEGMENTO_FAULT 104


	//ERROR DE CPU
	#define ERROR_POR_DESCONEXION_DE_CPU 110
	#define ERROR_POR_DESCONEXION_DE_CONSOLA 111


	//Estructura para lo que quieran. Poner las structs necesarias.
	typedef struct { //HANDSHAKE_PROGRAMACONSOLA
		char* programaConsola;
		int puertoProgramaConsola;
	} t_info_programa_handshake;



#endif /* PROTOCOLSTRUCT_H_ */
