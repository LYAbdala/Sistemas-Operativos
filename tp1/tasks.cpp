#include "tasks.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

void TaskCPU(int pid, vector<int> params) { // params: n
	uso_CPU(pid, params[0]); // Uso el CPU n milisegundos.
}

void TaskIO(int pid, vector<int> params) { // params: ms_pid, ms_io,
	uso_CPU(pid, params[0]); // Uso el CPU ms_pid milisegundos.
	uso_IO(pid, params[1]); // Uso IO ms_io milisegundos.
}

void TaskConsola(int pid, vector<int>params){
	int n, b_min, b_max;
	int random;
	n = params[0];
	b_min = params[1];
	b_max = params[2];
	for(int i = 0;i<n;i++){
		random = b_min + (rand() % (int)(b_max-b_min+1));
		uso_IO(pid, random);	
	}	
}

void TaskAlterno(int pid, vector<int> params) { // params: ms_pid, ms_io, ms_pid, ...
	for(int i = 0; i < (int)params.size(); i++) {
		if (i % 2 == 0) uso_CPU(pid, params[i]);
		else uso_IO(pid, params[i]);
	}
}

void TaskConBloqueo(int pid, vector<int> params) {
	int tiempo = params[0];
	int inicioBloq = params[1];
	int finBloq = params[2];

	if(inicioBloq > tiempo){
		uso_CPU(pid, tiempo);
	}else{
		uso_CPU(pid,inicioBloq-2);
		uso_IO(pid,finBloq-inicioBloq+1);
		uso_CPU(pid,tiempo-finBloq);
	}
}

void TaskBatch(int total_cpu, int cant_bloqueos){
	int indexTicks = 0;
	int cantBloq = 0;
	float random = 0;
	int total = total_cpu - cant_bloqueos;
	while (indexTicks < total){
		random = rand();
		if(random < 0.5){
			uso_IO(0,1);
			cantBloq++;
		}
		if(cantBloq == cant_bloqueos){
			uso_CPU(total-indexTicks, 0);
			indexTicks = total+1;
		}
		indexTicks++;
	}
	while(cantBloq < cant_bloqueos){
		uso_CPU(total-indexTicks, 0);
		cantBloq++;
	}
}

void tasks_init(void) {
	/* Todos los tipos de tareas se deben registrar acá para poder ser usadas.
	 * El segundo parámetro indica la cantidad de parámetros que recibe la tarea
	 * como un vector de enteros, o -1 para una cantidad de parámetros variable. */
	register_task(TaskCPU, 1);
	register_task(TaskIO, 2);
	register_task(TaskAlterno, -1);
	register_task(TaskConsola,3);
	register_task(TaskConBloqueo,3);
}
