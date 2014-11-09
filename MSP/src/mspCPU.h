#ifndef MSP_CPU_H_
#define MSP_CPU_H_

typedef struct{
	t_socket_paquete *paquete;
	t_socket *socket;
}t_nodo_cola;

typedef struct{
	int pid;
	uint32_t direccionVirtual;
	int tamanio;
	char* leido;
}t_envio_leerMSP;

// Funciones propias de la MSP_TO_CPU
void *mspLanzarHiloCPU(void *arg);


#endif /* CPU_H_ */
