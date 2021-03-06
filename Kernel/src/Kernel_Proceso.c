/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Kernel_Proceso.h"
#include "Planificador.h"
#include "boot.h"
#include "kernelMSP.h"
#include "Loader.h"

pthread_mutex_t blockMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t execMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t exitMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t newMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cpuMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t programasBesoDisponibleMutex = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char** argv) {

	verificar_argumentosKernel(argc, argv);
	t_kernel *self = malloc(sizeof(t_kernel));
	t_config *configKernel;

	if(!cargarConfiguracionKernel(argv[1], self, configKernel)){
		printf("Archivo de configuracion invalido\n");
		return EXIT_SUCCESS;
	}

	inicializar_panel(procesoKernel, argv[1]);

	cola_new = list_create();
	cola_ready = list_create();
	cola_exit = list_create();
	cola_exec = list_create();
	cola_block = list_create();
	listaDeCPUExec = list_create();
	listaDeCPULibres = list_create();
	listaDeProgramasDisponibles = list_create();
	listaDeRecursos = list_create();
	listaSystemCall = list_create();
	listaBloqueadosPorOtroHilo = list_create();


	//se inician los semaforos
	sem_init(&mutex_cpuLibre, 1, 1);
	sem_init(&mutex_cpuExec, 1, 1);
	sem_init(&mutex_exit, 1,1);
	sem_init(&mutex_exec, 1, 1);
	sem_init(&mutex_ready, 1, 0);
	sem_init(&mutex_new, 1, 0);
	sem_init(&mutex_block, 1, 1);
	//sem_init(&mutex_BloqueoPlanificador, 1, 0);
	sem_init(&sem_A, 0, 0);
	sem_init(&sem_B, 0, 0);
	sem_init(&sem_C, 0, 0);
	//sem_init(&sem_interrupcion, 1, 1);


	//hace el boot y le manda a la msp el archivo de SystemCall
	hacer_conexion_con_msp(self);

	//El codigo se levanta de las system calls
	crearTCBKERNEL(self);
	//
	//	//Esto lo hace despues de Bootear
	//
	iretThread = pthread_create( &LoaderHilo, NULL, (void*) kernel_comenzar_Loader, self);
	if(iretThread) {
		printf("Error - pthread_create() LoaderHilo, return code: %d\n", iretThread);
		exit(EXIT_FAILURE);
	}

	iretThread = pthread_create( &PlanificadorHilo, NULL, (void*) kernel_comenzar_Planificador, self);
	if(iretThread) {
		printf("Error - pthread_create() PlanificadorHilo, return code: %d\n",iretThread);
		exit(EXIT_FAILURE);
	}

	pthread_join(LoaderHilo, NULL);
	pthread_join(PlanificadorHilo, NULL);

	destruirConfiguracionKernel(self, configKernel);
	return EXIT_SUCCESS;
}

void verificar_argumentosKernel(int argc, char* argv[]){
	if( argc != 2 ){
		printf("Modo de empleo: ./Kernel kernelConfig.cfg\n");
		exit (EXIT_FAILURE);
	}
}


void ponerProgramaEnExit(t_TCB_Kernel* unTCB){

	t_programaEnKernel* programaInterrupcion = buscarProgramaEnLista(unTCB, cola_exec);

	pthread_mutex_lock(&exitMutex);
	list_add(cola_exit,programaInterrupcion);
	pthread_mutex_unlock(&exitMutex);
}

t_programaEnKernel* buscarProgramaEnLista(t_TCB_Kernel* unTCB, t_list* unaLista){
	bool esProgramaInterrupcion(t_programaEnKernel* programaEnLista){
		return ((programaEnLista->programaTCB->pid == unTCB->pid) && (programaEnLista->programaTCB->tid == unTCB->tid));
	}

	pthread_mutex_lock(&blockMutex);
	t_programaEnKernel* programaInterrupcion = list_find(unaLista, (void*)esProgramaInterrupcion);
	pthread_mutex_unlock(&blockMutex);

	return programaInterrupcion;
}
