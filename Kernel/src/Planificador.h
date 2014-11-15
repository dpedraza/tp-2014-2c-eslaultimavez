#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_


#include "Kernel.h"

t_list* listaCpu;
void kernel_comenzar_Planificador(t_kernel* self);

void planificadorEscuchaCPU(t_kernel* self);
t_cpu* obtenerCPUSegunDescriptor(t_kernel* self,int descriptor);
void atenderNuevaConexionCPU(t_kernel* self,t_socket* socketNuevoCliente, fd_set* master, int* fdmax);
void atienderCPU(t_kernel* self,t_cpu* programa, fd_set* master);


#endif /* BOOT_H_ */
