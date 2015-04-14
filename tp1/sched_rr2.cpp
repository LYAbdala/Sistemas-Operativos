#include <vector>
#include <queue>
#include "sched_rr2.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR2::SchedRR2(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cores = argn[0];
	quantum = new int[cores];
	contador = new int[cores];
	contDeTareasActivas = new int[cores];
	//indiceDeProcesador = 0;
	colas = new queue<int>[cores];
	for (int i = 0; i < cores; i++){
		quantum[i] = argn[i+1];
		contador[i] = 0;
		contDeTareasActivas[i] =0;
		queue<int> q;
		colas[i] = q;
	}
}

SchedRR2::~SchedRR2() {
	delete[] quantum;
	delete[] contador;
	delete[] contDeTareasActivas;
	delete[] colas;
}


void SchedRR2::load(int pid) {
	int i=0;
	int min=0;
	while(i<cores){
		if(contDeTareasActivas[i]<contDeTareasActivas[min]) min = i;
		i++;
	}
	colas[min].push(pid);
	contDeTareasActivas[min]++;
}

void SchedRR2::unblock(int pid) {
	int cpuTarea=0;
	for (list<pair<int,int> >::iterator it=bloqueadas.begin(); it != bloqueadas.end(); ++it){
		if((*it).first == pid){
			cpuTarea=(*it).second ;
			bloqueadas.remove(*it);
			break;
		}
	}
	colas[cpuTarea].push(pid);
}

int SchedRR2::tick(int cpu, const enum Motivo m) {		
	int sig;
	if (m == EXIT) {
		// Si el pid actual terminó, sigue el próximo.
		if (colas[cpu].empty()){
			return IDLE_TASK;
			contDeTareasActivas[cpu]=0;
		}
		else{
			sig = colas[cpu].front();
			colas[cpu].pop();
			contDeTareasActivas[cpu]--;
		}
	} 
	else if(m == TICK){
		// En TICK, sumar uno al contador
		if (current_pid(cpu) == IDLE_TASK){
			/*Si esta corriendo la IDLE*/
			if (colas[cpu].empty()){
				/*Si no hay ninguna encolada*/
				sig = IDLE_TASK;
			}
			else{
				/*Si hay una encolada, correr esa*/
				contador[cpu] = 0;
				sig = colas[cpu].front(); 
				colas[cpu].pop();
			}
		} 
		else {
			/*Si esta corriendo una que no es la IDLE*/
			contador[cpu]++;
			if (contador[cpu]==quantum[cpu]){
				/*Si se le acabo el quantum a la actual*/
				contador[cpu] = 0;
				colas[cpu].push(current_pid(cpu));
				sig = colas[cpu].front();
				colas[cpu].pop();
			}
			else{
				sig = current_pid(cpu);
			}
		}
	}
	else{
		/*En BLOCK, desalojar inmediatamente y pasar a la siguiente tarea*/
		int pid = current_pid(cpu);
		pair<int,int> nbloq = make_pair(pid,cpu);
		bloqueadas.push_front(nbloq);
		if (colas[cpu].empty()){
			sig = IDLE_TASK;
		}
		else{
			sig = colas[cpu].front();
			colas[cpu].pop();
		}
	}
	return sig;	
}
int SchedRR2::next(int cpu) {
	return 0;
}
