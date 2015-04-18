#include "sched_fixed.h"
#include <iostream>

using namespace std;

SchedFixed::SchedFixed(vector<int> argn) {
	/*SchedFixed recibe la cantidad de cores*/
	cores = argn[0];
}

SchedFixed::~SchedFixed() {
}

void SchedFixed::initialize() {
}

void SchedFixed::load(int pid) {
	/*Encolar una dupla de la forma (periodo(pid),pid)*/
	q.push(make_pair(period(pid),pid));
}

void SchedFixed::unblock(int pid) {
	q.push(make_pair(period(pid),pid));
}

int SchedFixed::tick(int cpu, const enum Motivo m) {
	int sig;
	if (m == EXIT){
		/* Si el pid actual termino, buscar proximo*/
		if (q.empty()){
			sig = IDLE_TASK;
		}
		else{
			sig = q.top().second; 
			q.pop();
		}
	}
	else if (m == TICK){
		/* En TICK, checkear que no haya una más prioritaria lista*/
		if (q.empty()){
			/*Si no hay ninguna encolada*/
			sig = current_pid(cpu);
		}
		else{
			/*Si hay alguna encolada, checkear la prioridad*/
			if ( (q.top().first) < period(current_pid(cpu))){
				sig = q.top().second;
				q.pop();
			}
			else{
				sig = current_pid(cpu);
			}
		}
	}
	else{
		/*En BLOCK, desalojar y pasar a la siguiente*/
		if (q.empty()){
			sig = IDLE_TASK;
		}
		else{
			sig = q.top().second;
			q.pop();
		}
	}
	return sig;	
}


/*
Las lıneas de la forma “&Ar,p,t” crean r tareas de tipo TaskCPU de duracion t y familia
A cada p unidades de tiempo (es decir la primera en 0p la segunda en 1p la tercera en
2p y ası hasta completar r tareas).
*/

/*
  current pid(cpu): Devuelve el proceso que esta usando el CPU.
  total_tasks(): Devuelve la cantidad total de tareas que se van a simular (solo para prioridades fijas y dinamicas)
  period(int pid): Devuelve el perıodo de una tarea.
  type(int pid): Devuelve la familia de una tarea.
  declared_cputime(int pid): Devuelve el tiempo de CPU que la tarea va a utilizar.
*/

/*
  TICK: la tarea consumio todo el ciclo utilizando el CPU.
  BLOCK: la tarea ejecuto una llamada bloqueante o permanecio bloqueada durante el ultimo ciclo.
  EXIT: la tarea termino(ejecuto return).
*/ 

/*
  


*/ 