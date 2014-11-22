#ifndef CPUKERNEL_H_
#define CPUKERNEL_H_

#include "CPU.h"

typedef struct {
	t_TCB_CPU* tcb;
	int quantum;
} t_TCB_nuevo;

/*
 * Nombre: t_entrada_estandar
 * Descripcion: envia al Kernel un pid, un tamanio maximo de entrada y un identificador de tipo de entrada de datos por consola.
 * Mensaje: ENTRADA_ESTANDAR
 *
 * 1 = numero (-2.147.483.648 a -2.147.483.648)
 * 2 = string (el tamanio de la cadena ingresada es indicado por el tamanio)
 *
 * */

typedef struct{
	int pid;
	int32_t tamanio;
	int tipo;
} t_entrada_estandar;

/*
 * Nombre: t_salida_estandar
 * Descripcion: envia al Kernel un pid y una cadena para que se muestre por pantalla.
 * Mensaje: SALIDA_ESTANDAR
 * */

typedef struct{
	int pid;
	char* cadena;
} t_salida_estandar;

/*
 * Nombre: t_interrupcion
 * Descripcion: envia al Kernel un tcb y una direccion virtual (desplazamiento dentro del systemcall.bc)
 * 				el CPU queda bloqueado esperando que reciba el tcbkernel, con todos los campos iguales al tcb enviado, excepto
 * 				por el puntero_instruccion, donde debe ir la direccion virtual enviada.
 * Mensaje: INTERRUPCION
 * */

typedef struct {
	t_TCB_CPU* tcb;
	uint32_t direccionVirtual;
} t_interrupcion;
t_socket_client* conectarCPUConKernel(t_CPU* self);

/*
 * Nombre: t_join
 * Descripcion: envia al Kernel un tid_llamador, que se bloquea hasta que el tid_esperar termine su ejecucion.
 * Mensaje: JOIN_HILO
 * */

typedef struct {
	int tid_llamador;
	int tid_esperar;
} t_join;


/*
 * Nombre: t_bloquear
 * Descripcion: envia al Kernel un tcb y un id de recurso, indicando que el tcb debe bloquearse en la cola del recurso
 * 				asociado al id de recurso.
 * Mensaje: BLOK_HILO
 * */

typedef struct {
	t_TCB_CPU* tcb;
	int32_t id_recurso;
} t_bloquear;

/*
 * Nombre: t_despertar
 * Descripcion: envia al Kernel un tcb y un id de recurso, indicando que el tcb debe bloquearse en la cola del recurso
 * 				asociado al id de recurso.
 * Mensaje: WAKE_HILO
 * */

typedef struct {
	int32_t id_recurso;
} t_despertar;

/*
 * Nombre: t_crear_hilo
 * Descripcion: envia al Kernel un tcb indicando que se debe crear un hilo hijo y luego planificarlo.
 * Mensaje: CREA_HILO
 *
 * */

typedef struct {
	t_TCB_CPU* tcb;
} t_crea_hilo;



t_socket_client* conectarCPUConKernel(t_CPU* self);
void cpuRealizarHandshakeConKernel(t_CPU* self);


#endif /* CPUKERNEL_H_ */
