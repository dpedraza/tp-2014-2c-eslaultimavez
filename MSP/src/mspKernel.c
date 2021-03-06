#include "Memoria.h"
#include "mspKernel.h"
#include "MSP.h"
#include "mspPrograma.h"
#include "commons/log.h"
#include "commons/socketInBigBang.h"
#include "commons/protocolStructInBigBang.h"
#include <stdlib.h>
#include <unistd.h>

extern t_MSP *self;
//t_sockets *socketsKernel;

void *mspLanzarHiloKernel(t_socket* socketsKernel){

	t_socket_paquete *paquete;
	int i = 1;

	log_info(self->logMSP, "MSP: Hilo Kernel creado correctamente.");

	if (socket_sendPaquete(socketsKernel, HANDSHAKE_MSP, 0, NULL) > 0)
		log_info(self->logMSP, "MSP: Handshake con Kernel!");

	else
		log_error(self->logMSP, "MSP: Error al recibir los datos del Kernel.");

	while(i){

		paquete = (t_socket_paquete *) malloc(sizeof(t_socket_paquete));

		if (socket_recvPaquete(socketsKernel, paquete) >= 0){

			switch(paquete->header.type){

			case CREAR_SEGMENTO:
				crearSegmentoKernel(socketsKernel, paquete);
				break;
			case DESTRUIR_SEGMENTO:
				destruirSegmentoKernel(socketsKernel, paquete);
				break;
			case ESCRIBIR_MEMORIA:
				escribirMemoriaKernel(socketsKernel,paquete);
				break;
			case LEER_MEMORIA:
				leerMemoriaKernel(socketsKernel,paquete);
				break;
			default:
				break;
			}
		}

		else{
			log_debug(self->logMSP, "MSP: El Kernel ha cerrado la conexion.");
			close(socketsKernel->descriptor);
			i = 0;
		}

		socket_freePaquete(paquete);
	}//fin del while(1)

	return NULL;
}


void crearSegmentoKernel(t_socket* socketsKernel,t_socket_paquete *paquete){

	t_datos_aKernelSegmento* datosAKernel = malloc(sizeof(t_datos_aKernelSegmento));
	t_datos_deKernelCrearSegmento* datosDeKernel = (t_datos_deKernelCrearSegmento*) (paquete->data);

	log_info(self->logMSP, "MSP: Kernel esta solicitando la creacion de un segmento...");

	log_info(self->logMSP, "MSP: Abriendo el paquete del Kernel: El paquete contiene PID: %d, Tamaño: %d", datosDeKernel->pid, datosDeKernel->tamanio);

	buscarCrearPrograma(datosDeKernel->pid);

	//Esto envía ERROR_POR_TAMANIO_EXCEDIDO o ERROR_POR_MEMORIA_LLENA o ERROR_POR_NUMERO_NEGATIVO o ERROR_POR_SEGMENTO_INVALIDO
	//o ERROR_POR_SEGMENTATION_FAULT o la base del segmento si no hubo ningún problema
	datosAKernel->recibido = mspCrearSegmento(datosDeKernel->pid, datosDeKernel->tamanio);

	if (socket_sendPaquete(socketsKernel, CREAR_SEGMENTO, sizeof(t_datos_aKernelSegmento), datosAKernel) > 0)
		log_info(self->logMSP, "MSP: Los datos de creacion de Segmento se han enviado al Kernel correctamente");

	free(datosAKernel);
	free(datosDeKernel);
}


void destruirSegmentoKernel(t_socket* socketsKernel,t_socket_paquete *paquete){

	t_datos_aKernelSegmento* datosAKernel = malloc(sizeof(t_datos_aKernelSegmento));
	t_datos_deKernelDestruirSegmento* datosDeKernel = (t_datos_deKernelDestruirSegmento*) (paquete->data);

	log_info(self->logMSP, "MSP: Kernel esta solicitando la destruccion de un segmento...");

	log_info(self->logMSP, "MSP: Abriendo el paquete del Kernel: El paquete contiene PID: %d, Direccion Base: %0.8p", datosDeKernel->pid, datosDeKernel->direccionBase);

	//Esto envía SIN_ERRORES si se destruyó el segmento correctamente
	//o ERROR_POR_SEGMENTO_DESCONOCIDO si el segmento indicado es incorrecto
	datosAKernel->recibido = mspDestruirSegmento(datosDeKernel->pid, datosDeKernel->direccionBase);

	if (socket_sendPaquete(socketsKernel, DESTRUIR_SEGMENTO, sizeof(t_datos_aKernelSegmento), datosAKernel) > 0)
		log_info(self->logMSP, "MSP: Los datos de destruccion de Segmento se han enviado al Kernel correctamente");

	free(datosAKernel);
	free(datosDeKernel);
}


void escribirMemoriaKernel(t_socket* socketsKernel, t_socket_paquete *paquete){

	t_datos_aKernelEscritura* datosAKernel = malloc(sizeof(t_datos_aKernelEscritura));
	t_datos_deKernelEscritura* datosDeKernel = (t_datos_deKernelEscritura*) (paquete->data);

	log_info(self->logMSP, "MSP: Kernel esta solicitando escribir la memoria...");

	log_info(self->logMSP, "MSP: Abriendo el paquete del Kernel: El paquete contiene PID: %d, Direccion Virtual: %0.8p, Tamaño: %d %s", datosDeKernel->pid, datosDeKernel->direccionVirtual, datosDeKernel->tamanio, datosDeKernel->buffer);

	//Esto envía SIN_ERRORES si se escribió la memoria correctamente
	//o ERROR_POR_SEGMENTATION_FAULT si se intentó escribir memoria inválida
	datosAKernel->estado = mspEscribirMemoria(datosDeKernel->pid, datosDeKernel->direccionVirtual, datosDeKernel->buffer, datosDeKernel->tamanio);

	if (socket_sendPaquete(socketsKernel, ESCRIBIR_MEMORIA, sizeof(t_datos_aKernelEscritura), datosAKernel) > 0)
		log_info(self->logMSP, "MSP: Los datos de escritura de memoria se han enviado al Kernel correctamente");

	free(datosAKernel);
	free(datosDeKernel);
}


void leerMemoriaKernel(t_socket* socketsKernel, t_socket_paquete *paquete){

	t_datos_aKernelLectura* datosAKernel = malloc(sizeof(t_datos_aKernelLectura));
	t_datos_deKernelLectura* datosDeKernel = (t_datos_deKernelLectura*) (paquete->data);

	log_info(self->logMSP, "MSP: Kernel esta solicitando leer la memoria...");

	log_info(self->logMSP, "MSP: Abriendo el paquete del Kernel: El paquete contiene PID: %d, Direccion Virtual : %0.8p, Tamaño: %d", datosDeKernel->pid, datosDeKernel->direccionVirtual, datosDeKernel->tamanio);

	//Esto envía SIN_ERRORES si se leyó la memoria correctamente
	//o ERROR_POR_SEGMENTATION_FAULT si se intentó leer memoria inválida
	datosAKernel->estado = mspLeerMemoria(datosDeKernel->pid, datosDeKernel->direccionVirtual, datosDeKernel->tamanio, datosAKernel->lectura);

	if (socket_sendPaquete(socketsKernel, LEER_MEMORIA, sizeof(t_datos_aKernelLectura), datosAKernel) > 0)
		log_info(self->logMSP, "MSP: Los datos de lectura de memoria se han enviado al Kernel correctamente");

	free(datosDeKernel);
	free(datosAKernel);
}
