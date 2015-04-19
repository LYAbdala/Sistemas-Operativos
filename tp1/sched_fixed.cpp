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
	/*Encolar una dupla de la forma <periodo(pid),pid>*/
	q.push(make_pair(period(pid),pid));
}

void SchedFixed::unblock(int pid) {
	/*Encolar una dupla de la forma <periodo(pid),pid>*/
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
		if (current_pid(cpu) == IDLE_TASK){
			/*Si esta corriendo la IDLE*/
			if (q.empty()){
				/*Si no hay ninguna, seguir con la IDLE*/
				sig = IDLE_TASK;
			}
			else{
				/*Seguir con la siguiente*/
				sig = q.top().second;
				q.pop();
			}
		}
		else{
			/*Si esta corriendo una que no es la IDLE*/
			if (q.empty()){
				/*Si no hay ninguna, seguir con la actual*/
				sig = current_pid(cpu);
			}
			else{
				if(q.top().first < period(current_pid(cpu))){
					/*Si hay otra con periodo menor (mas prioritaria), 
					encolar la actual y pasar a esa*/
					q.push(make_pair(period(current_pid(cpu)),current_pid(cpu)));
					sig = q.top().second;
					q.pop();
				}
				else{
					/*Si la proxima de la cola tenia periodo mayor, seguir
					con la actual*/
					sig = current_pid(cpu);
				}
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