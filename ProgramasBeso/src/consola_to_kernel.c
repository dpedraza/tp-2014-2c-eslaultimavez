#include "consola_to_kernel.h"

void consolaConectarConElLOADER(t_programaBESO* self) {

	self->socket_ProgramaBESO = socket_createClient();

	if (self->socket_ProgramaBESO  == NULL ) {
		log_error(self->loggerProgramaBESO, "Consola: Error al crear socket del Proceso Programa!");
	}
	log_info(self->loggerProgramaBESO, "Programa con el puerto %d  y ip %s se presenta al LOADER!",self->puertoLoader,self->ipLoader);

	if (socket_connect( self->socket_ProgramaBESO,self->ipLoader,self->puertoLoader)==0) {
		log_error(self->loggerProgramaBESO, "Error al conectar el Proceso Programa con el Proceso Kernel!");
	}else{
		log_info(self->loggerProgramaBESO, "ProgramaBeso: Conectado con el Loader (IP:%s/Puerto:%d)!", self->ipLoader, self->puertoLoader);
		consolaRealizarHandshakeConLOADER(self);
	}
}

void consolaRealizarHandshakeConLOADER(t_programaBESO* self) {

	if (socket_sendPaquete(self->socket_ProgramaBESO->socket, HANDSHAKE_PROGRAMA, 0,NULL) > 0)
		log_info(self->loggerProgramaBESO, "Programa: Envio HANDSHAKE_PROGRAMA al LOADER!");
	else
		log_error(self->loggerProgramaBESO, "Programa:Error al enviar HANDSHAKE_PROGRAMA al LOADER!");

	t_socket_paquete *paquete_handshake = (t_socket_paquete *)malloc(sizeof(t_socket_paquete));

	if (socket_recvPaquete(self->socket_ProgramaBESO->socket , paquete_handshake) >= 0) {
		log_info(self->loggerProgramaBESO, "Programe: Recibe paquete del LOADER.");

		if(paquete_handshake->header.type == HANDSHAKE_LOADER){
			log_info(self->loggerProgramaBESO, "Programa: Handshake completo con Loader!");
//			t_info_pid* datos_recibidos = (t_info_pid*) (paquete_handshake->data);
//			log_info(self->loggerProgramaBESO, "Se asigno el ID %d!",datos_recibidos->pid);
//
//			log_info(self->loggerProgramaBESO, "vamos a enviar el codigo.");
//			log_info(self->loggerProgramaBESO, "%s",self->codigo);
//			if (socket_sendPaquete(self->socket_ProgramaBESO , 5000, 50, NULL) > 0) { // ver error!
//			log_info(self->loggerProgramaBESO, "Se envio el programa con exito.");
//			}else{
//				log_error(self->loggerProgramaBESO, "No se pudo enviar el codigo %s",self->codigo);
//			}
		}
	}else {
		log_error(self->loggerProgramaBESO, "Programa: Error al recibir los datos del KERNEL!");
	}

	free(paquete_handshake);

	while(1){
		t_socket_paquete *paquete = (t_socket_paquete *)malloc(sizeof(t_socket_paquete));

		if ((socket_recvPaquete(self->socket_ProgramaBESO, paquete)) >= 0) {
			switch(paquete->header.type){
			case FINALIZAR_PROGRAMA_EXITO:
				log_info(self->loggerProgramaBESO, "Consola: Termino correctamente el programa.");
				close(self->socket_ProgramaBESO->socket->descriptor);
				break;

			case ERROR_POR_SEGMENTATION_FAULT:
				log_info(self->loggerProgramaBESO, "Consola: Error de memoria");
				close(self->socket_ProgramaBESO->socket->descriptor);
				break;

			default:
				log_info(self->loggerProgramaBESO, "Consola: Llego cualquier cosa que no se esperaba.");
				break;

			}//fin switch
		free(paquete);
	}
	}//fin while
}