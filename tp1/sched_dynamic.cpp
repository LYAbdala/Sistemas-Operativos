#include "sched_dynamic.h"
#include <iostream>

using namespace std;

SchedDynamic::SchedDynamic(vector<int> argn) {
	/*SchedFixed recibe la cantidad de cores?*/
	deadline = NULL;
	ready = NULL;
}

SchedDynamic::~SchedDynamic() {
	delete[] deadline;
	delete[] ready;
}

void SchedDynamic::initialize() {
	total_tareas = total_tasks();
	deadline = new int[total_tareas];
	ready = new bool[total_tareas];
	int i;
	for (i = 0; i < total_tareas; i++){
		ready[i] = false;
		if (period(i) == 0){
			deadline[i] = -1;
		}
		else{
			deadline[i] = period(i);
		}
	}
}

void SchedDynamic::load(int pid) {
	ready[pid] = true;
}

void SchedDynamic::unblock(int pid) {
	/*Desbloquear una tarea. Como las tareas periodicas no hacen
	llamadas bloqueantes, esto solo va a pasar con tareas no
	periodicas*/
	ready[pid] = true;
}


int SchedDynamic::tick(int cpu, const enum Motivo m) {
	int sig;
	int pid_actual = current_pid(cpu);
	
	if (m == EXIT){
		/*Si el pid actual termino, buscar proximo*/
		ready[pid_actual] = false;
		if(period(pid_actual)!=0){
			/*Si termino una periodica, ponerle deadline = -1 */
			deadline[pid_actual] = -1;
		}
		if(tareas_ready() == 0){
			/*Si no hay ninguna lista, pasar a la idle*/
			sig = IDLE_TASK;
		}
		else{
			/*Buscar la siguiente*/
			sig = next(pid_actual);
		}
	}

	else if(m == TICK){
		/*En TICK, checkear que no haya una mas prioritaria lista*/
		if(period(pid_actual) == 0){
			/*Si la tarea es no periodica, seguir con esa*/
			sig = pid_actual;
		}
		else{
			/*Si la tarea es periodica, buscar otra mas prioritaria*/
			sig = next(pid_actual);
		}
	}

	else{
		/*En BLOCK, desalojar y pasar a la siguiente*/
		ready[pid_actual] = false;
		if(tareas_ready() == 0){
			sig = IDLE_TASK;
		}
		else {
			sig = next(pid_actual);
		}
	}
	return sig;
}

int SchedDynamic::next(int pid_actual){
	int i;
	/*Si todo falla (no hay ninguna ready) devolver la idle*/
	int res = IDLE_TASK;
	/*Una pasada para buscar alguna no periodica*/
	for(i=0; i < total_tareas;i++){
		if (period(i)==0 && ready[i]){
			/*Si es no periodica y esta lista, devolverla*/
			res = i;
			return res;
		}
	}
	/*Si estoy aca, la siguiente va a ser una de las periodicas*/
	for(i=0;i<total_tareas;i++){
		if(period(i)==0 || !ready[i])
			/*Saltear no periodicas y no listas*/
			continue;
		if(res == IDLE_TASK){
			/*Si res esta desasignado, asignarlo*/
			res = i;
		}
		else{
			if(deadline[i] < deadline[res])
				/*Si la deadline de la actual es menor que 
				la que tenia, guardarme la actual*/
				res = i;
		}
	}
	return res;
}

int SchedDynamic::tareas_ready(){
	int res = 0;
	for(int i = 0; i < total_tareas;i++){
		if(ready[i])
			res++;
	}
	return res;	
}

/*En cada tick, decrementar lo que falta 
	para las deadlines de las periodicas listas*/
/*	
	for(i=0;i<total_tareas;i++){
		if(period(i)!=0 && ready[i])
			deadline[i]--;
	}
*/