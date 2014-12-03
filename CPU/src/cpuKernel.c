#include "cpuKernel.h"
#include "CPU_Proceso.h"
#include <stdlib.h>


void cpuConectarConKernel(t_CPU *self){

	log_info(self->loggerCPU, "CPU: Conectando con el Planificador...");

	self->socketPlanificador = socket_createClient();

	if (self->socketPlanificador  == NULL )
		log_error(self->loggerCPU, "Error al crear socket del Proceso CPU!");

	if (socket_connect(self->socketPlanificador, self->ipPlanificador, self->puertoPlanificador) == 0)
		log_error(self->loggerCPU, "CPU: Error al hacer el connect con el Planificador!");

	else
		cpuRealizarHandshakeConKernel(self);

}


void cpuRealizarHandshakeConKernel(t_CPU *self){

	t_socket_paquete *paquete = (t_socket_paquete *) malloc(sizeof(t_socket_paquete));

	log_info(self->loggerCPU, "La CPU solicita HANDSHAKE con KERNEL.");

	if (socket_sendPaquete(self->socketPlanificador->socket, HANDSHAKE_CPU, 0, NULL) > 0)
		log_info(self->loggerCPU, "CPU le envia HANDSHAKE al KERNEL!");

	if (socket_recvPaquete(self->socketPlanificador->socket, paquete) >= 0) {

		if(paquete->header.type == HANDSHAKE_PLANIFICADOR)
			log_info(self->loggerCPU, "CPU: Recibe del Planificador HANDSHAKE_PLANIFICADOR");

		else
			log_error(self->loggerCPU, "CPU: Error al recibir HANDSHAKE_PLANIFICADOR del Planificador");
	}

	else
		log_error(self->loggerCPU, "CPU: Error al recibir paquete del Planificador.");

}


int cpuRecibirTCB(t_CPU *self){

	t_socket_paquete *paquetePlanificadorTCB = (t_socket_paquete*) malloc(sizeof(t_socket_paquete));
	t_TCB_CPU* unTCBNuevo = (t_TCB_CPU*) malloc(sizeof(t_TCB_CPU));

	if(socket_recvPaquete(self->socketPlanificador->socket, paquetePlanificadorTCB) >= 0){

		//printf("TCB_NUEVO %d \n", paquetePlanificadorTCB->header.type);   //no Borrar sirve para como debug Jorge
		if(paquetePlanificadorTCB->header.type == TCB_NUEVO){

			unTCBNuevo = (t_TCB_CPU*) paquetePlanificadorTCB->data;
			self->tcb = unTCBNuevo;
			printTCBCPU(self->tcb);
			//log_debug(self->loggerCPU, "CPU: recibio un TCB_NUEVO con PID: %d TID:%d KM:%d", self->tcb->pid, self->tcb->tid, self->tcb->km);
		}

		else{
			log_error(self->loggerCPU, "CPU: error al recibir de planificador TCB_NUEVO");
			return 1;
		}
	}

	else{
		log_error(self->loggerCPU, "CPU: Error al recibir un paquete del planificador");
		return 1;
	}

	free(paquetePlanificadorTCB);
	return 0;
}


int cpuRecibirQuantum(t_CPU *self){

	t_socket_paquete *paquetePlanificadorQuantum = (t_socket_paquete *) malloc(sizeof(t_socket_paquete));
	t_quantumCPU* unQuantum =  (t_quantumCPU*) malloc(sizeof(t_quantumCPU));

	if(socket_recvPaquete(self->socketPlanificador->socket, paquetePlanificadorQuantum) >= 0){

		t_quantumCPU* unQuantum = (t_quantumCPU*) paquetePlanificadorQuantum->data;
		self->quantum = unQuantum->quantumCPU;

		log_debug(self->loggerCPU, "CPU: recibe un quantum: %d",self->quantum);
	}

	else{
		log_error(self->loggerCPU, "CPU: error al recibir un quantum");
		return 1;
	}

	free(paquetePlanificadorQuantum);
	return 0;
}


void cpuEnviarPaqueteAPlanificador(t_CPU *self, int paquete){

	if (socket_sendPaquete(self->socketPlanificador->socket, paquete, 0, NULL) <= 0)
		log_info(self->loggerCPU, "CPU: Fallo envio de CPU_TERMINE_UNA_LINEA, PID: %d", self->tcb->pid);
	else
		log_info(self->loggerCPU, "CPU: envia al Planificador un paquete N°: %d", paquete);

}


void cpuCambioContexto(t_CPU *self){

	//antes de hacer el send, deberia actualizarce el STACK
	//el stack sufre modificaciones cuando se ejecutan las instrucciones ESO, no se actualiza, lo que se actualiza es el TCB.
	if (socket_sendPaquete(self->socketPlanificador->socket, CAMBIO_DE_CONTEXTO, sizeof(t_TCB_CPU), self->tcb) <= 0)
		log_error(self->loggerCPU, "CPU: Falló cambio de conexto");

	else
		log_info(self->loggerCPU, "CPU: Envia al Planificador: CAMBIO_DE_CONTEXTO");

}


void cpuEnviaInterrupcion(t_CPU *self){

	t_interrupcion *interrupcion = malloc(sizeof(t_interrupcion));
	interrupcion->tcb = self->tcb;
	interrupcion->direccion = self->unaDireccion;

	//socket_sendPaquete(self->socketPlanificador->socket, INTERRUPCION, 0, NULL);
	log_info(self->loggerCPU, "CPU: Solicitud de Interrupcion al Kernel");

	if (socket_sendPaquete(self->socketPlanificador->socket, INTERRUPCION, sizeof(t_interrupcion), interrupcion) <= 0)
		log_error(self->loggerCPU, "CPU: Error de Interrupcion");

	else
		printTCBCPU(interrupcion->tcb);
		log_info(self->loggerCPU, "CPU: envia INTERRUPCION");

}


int cpuFinalizarProgramaExitoso(t_CPU *self, t_TCB_CPU* algo){

	log_info(self->loggerCPU, "CPU: Envia FINALIZAR_PROGRAMA_EXITO al Kernel!!!!");

	socket_sendPaquete(self->socketPlanificador->socket, FINALIZAR_PROGRAMA_EXITO, 0, NULL);

	if (socket_sendPaquete(self->socketPlanificador->socket, FINALIZAR_PROGRAMA_EXITO, sizeof(t_TCB_CPU), algo) <= 0){
		log_error(self->loggerCPU, "CPU: Error de finalizacion de proceso %d", self->tcb->pid);
		return -1;
	}

	else{
		log_info(self->loggerCPU, "CPU: Finalizacion de proceso correctamente %d", self->tcb->pid);
		return SIN_ERRORES;

	}
}
