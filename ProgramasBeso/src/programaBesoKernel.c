#include "programaBesoKernel.h"
#include "commons/protocolStructInBigBang.h"
#include <errno.h>
#include <sys/socket.h>
#include <sys/sendfile.h>

void consolaHacerConexionconLoader(t_programaBESO* self){

	self->socketKernel = socket_createClient();

	if (self->socketKernel == NULL )
		log_error(self->loggerProgramaBESO, "Consola: Error al crear socket con el Kernel!");

	if (socket_connect(self->socketKernel, self->ipLoader, self->puertoLoader) == 0)
		log_error(self->loggerProgramaBESO, "Consola: Error al conectarse con el Kernel.");

	else{
		log_info(self->loggerProgramaBESO, "Consola: Conectado con el Kernel (IP: %s/Puerto: %d)!", self->ipLoader, self->puertoLoader);
		realizarHandshakeConLoader(self);
	}

}

void realizarHandshakeConLoader(t_programaBESO* self){

	t_socket_paquete *paquete = (t_socket_paquete*) malloc(sizeof(t_socket_paquete));

	if (socket_sendPaquete(self->socketKernel->socket, HANDSHAKE_PROGRAMA, 0, NULL) > 0)
		log_info(self->loggerProgramaBESO, "Consola: Envia al Kernel: HANDSHAKE_PROGRAMA ");

	if (socket_recvPaquete(self->socketKernel->socket, paquete) >= 0) {
		if(paquete->header.type == HANDSHAKE_LOADER)
			log_info(self->loggerProgramaBESO, "Consola: Recibe del Kernel: HANDSHAKE_LOADER");
	}
	else
		log_error(self->loggerProgramaBESO, "Consola: Error al recibir los datos del Kernel.");

}

void consolaComunicacionLoader(t_programaBESO* self, char *parametro){

	FILE *archivoBeso = fopen(parametro, "r");

	if(archivoBeso == 0){
		log_error(self->loggerProgramaBESO, "Ha ocurrido un problema al abrir el archivo");
		exit(-1);
	}

	fseek(archivoBeso, 0, SEEK_END);	//Me coloco al final del fichero para saber el tamanio
	size_t sizeArchivoBeso = ftell(archivoBeso);
	fseek(archivoBeso, 0, SEEK_SET);	//Me coloco al principio del fichero para leerlo

	off_t offset = 0;
	int i;
	t_socket_header header;
	header.length = sizeof(header) + sizeArchivoBeso;

	if(send(self->socketKernel->socket->descriptor, &header, sizeof(t_socket_header), 0) == sizeof(t_socket_header) && sendfile(self->socketKernel->socket->descriptor, archivoBeso->_fileno, &offset, sizeArchivoBeso) == sizeArchivoBeso)
		log_info(self->loggerProgramaBESO, "Consola: Los datos del Programa Beso se enviaron correctamente al Kernel");

	else{
		log_error(self->loggerProgramaBESO, "Consola: No se pudo enviar el archivo");
		exit(-1);
	}

	t_socket_paquete *paquete = (t_socket_paquete *) malloc(sizeof(t_socket_paquete));
	t_datosKernel* datosAKernel = malloc(sizeof(t_datosKernel));
	t_datosMostrarConsola* datosDeKernel = (t_datosMostrarConsola*) (paquete->data);

	datosAKernel->codigoBeso = self->codigo;

	log_info(self->loggerProgramaBESO, "Consola: Espera respuesta del kernel");

	while(1){
		if (socket_recvPaquete(self->socketKernel->socket, paquete) >= 0){

			switch(datosDeKernel->codigo){

			case ERROR_POR_TAMANIO_EXCEDIDO:
				log_error(self->loggerProgramaBESO,"Consola: Se ha recibido un error por tamaño de segmento excedido");
				break;
			case ERROR_POR_MEMORIA_LLENA:
				log_error(self->loggerProgramaBESO,"Consola: Se ha recibido un error por memoria llena");
				break;
			case ERROR_POR_NUMERO_NEGATIVO:
				log_error(self->loggerProgramaBESO,"Consola: Se ha recibido un error por solicitar un tamaño de segmento negativo");
				break;
			case ERROR_POR_SEGMENTO_INVALIDO:
				log_error(self->loggerProgramaBESO,"Consola: Se ha recibido un error por segmento invalido");
				break;
			case ERROR_POR_SEGMENTATION_FAULT:
				log_error(self->loggerProgramaBESO,"Consola: Se ha recibido un error del tipo Segmentation Fault");
				break;
			default:
				log_info(self->loggerProgramaBESO,"Consola: Recibe OK: %s",datosDeKernel->mensaje);
				break;

			}// fin del switch

		}
		else{
			log_error(self->loggerProgramaBESO, "Consola: El Kernel ha cerrado la conexion.");
			close(self->socketKernel->socket->descriptor);
			exit(-1);
		}
	}

	free(datosAKernel->codigoBeso);
	free(datosAKernel);
	socket_freePaquete(paquete);
}

