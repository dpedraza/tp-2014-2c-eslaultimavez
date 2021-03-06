#ifndef KERNEL_H_
#define KERNEL_H_

#include <semaphore.h>
#include <commons/collections/list.h>
#include "commons/log.h"
#include "commons/socketInBigBang.h"
#include "commons/protocolStructInBigBang.h"
#include "commons/kernel.h"
#include "commons/config.h"
#include "commons/panel.h"
#include <pthread.h>

/*----------------------Estructuras del Kernel----------------------------------------*/

typedef struct{
	int pid;
	int32_t tamanio;
	int tipo;
	int idCPU;
} t_entrada_estandarKenel;


typedef struct{
	int numero;
	int idCPU;
} t_entrada_numeroKernel;


typedef struct{
	char texto[1000];
	int idCPU;
} t_entrada_textoKernel;

typedef struct {
	int pid;
	int tid;
	short km;
	uint32_t base_segmento_codigo;
	int tamanio_segmento_codigo;
	uint32_t puntero_instruccion;
	uint32_t base_stack;
	uint32_t cursor_stack;
	int32_t registro_de_programacion[5];
} t_TCB_Kernel;

typedef struct {
	int quantum;
}t_QUANTUM;


typedef struct {
	t_socket_client* socketMSP;
	t_socket* socketCPU;
	t_socket* socketConsola;
	t_log* loggerKernel;
	t_log* loggerPlanificador;
	t_log* loggerLoader;
	int puertoLoader;
	int puertoPlanificador;
	char* ipMsp;
	int puertoMsp;
	int quantum;
	char* systemCalls;
	int tamanioStack;
	t_TCB_Kernel* tcbKernel;
}t_kernel;


typedef struct {
	t_TCB_Kernel* programaTCB;
	t_socket* socketProgramaConsola;
} t_programaEnKernel;


typedef struct {
	int motivo;
	t_TCB_Kernel* tcbKernel;
}t_procesoBloquea;


typedef struct {
	uint32_t id;
	t_TCB_Kernel* TCB;
	t_socket* socketCPU;
}t_cpu;

//Guardan un TCB, Socket, ID
t_list *listaDeProgramasDisponibles;
t_list* cola_new;
t_list* cola_ready;
t_list* cola_exec;
t_list* cola_block; //POR CUATRO, bloqueado por systemCall, por recurso, por esperando un hilo
t_list* cola_exit; 

//Guardan solo TCB para planificador (t_cpu)
t_list* listaDeCPUExec;
t_list* listaDeCPULibres;
t_list* listaSystemCall;
t_list* listaDeRecursos;
t_list *listaBloqueadosPorOtroHilo;

t_tipo_proceso procesoKernel;
t_hilo hiloKernel;
t_list *listaHilosKernel;

// Semaforos
sem_t sem_A;
sem_t sem_B;
sem_t sem_C;
//sem_t sem_interrupcion;
sem_t mutex_new;
sem_t mutex_block;      // Mutex cola Block
sem_t mutex_ready;      // Mutex cola Ready
sem_t mutex_exec;       // Mutex cola Exec
sem_t mutex_exit;       // Mutex cola Exit
sem_t mutex_cpuLibre;   // Mutex cola de CPUs libres
sem_t mutex_cpuExec;    // Mutex cola de CPUs procesando
//sem_t mutex_BloqueoPlanificador;// Mutex cola New


int iretThread;
pthread_t LoaderHilo, PlanificadorHilo;

void kernel_crearColaDeEstados(void);
int  kernel_escuchar_conexiones(void);
void verificar_argumentosKernel(int argc, char* argv[]);

void ponerProgramaEnExit(t_TCB_Kernel* unTCB);
t_programaEnKernel* buscarProgramaEnLista(t_TCB_Kernel* unTCB, t_list* unaLista);

#endif /* KERNEL_H_ */
