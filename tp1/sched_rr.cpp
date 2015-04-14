#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR::SchedRR(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cores = argn[0];
	quantum = new int[cores];
	contador = new int[cores];
	for (int i = 0; i < cores; i++){
		quantum[i] = argn[i+1];
		contador[i] = 0;
	}
}

SchedRR::~SchedRR() {
	delete[] quantum;
	delete[] contador;
}

void SchedRR::load(int pid) {
	q.push(pid);	
}

void SchedRR::unblock(int pid) {
	q.push(pid);
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	int sig;
	if (m == EXIT) {
		// Si el pid actual terminó, sigue el próximo.
		if (q.empty()){
			return IDLE_TASK;
		}
		else{
			sig = q.front();
			q.pop();
		}
	} 
	else if(m == TICK){
		// En TICK, sumar uno al contador
		if (current_pid(cpu) == IDLE_TASK){
			/*Si esta corriendo la IDLE*/
			if (q.empty()){
				/*Si no hay ninguna encolada*/
				sig = IDLE_TASK;
			}
			else{
				/*Si hay una encolada, correr esa*/
				contador[cpu] = 0;
				sig = q.front(); 
				q.pop();
			}
		} 
		else {
			/*Si esta corriendo una que no es la IDLE*/
			contador[cpu]++;
			if (contador[cpu]==quantum[cpu]){
				/*Si se le acabo el quantum a la actual*/
				contador[cpu] = 0;
				q.push(current_pid(cpu));
				sig = q.front();
				q.pop();
			}
			else{
				sig = current_pid(cpu);
			}
		}
	}
	else{
		/*En BLOCK, desalojar inmediatamente y pasar a la siguiente tarea*/
		if (q.empty()){
			sig = IDLE_TASK;
		}
		else{
			sig = q.front();
			q.pop();
		}
	}
	return sig;
}