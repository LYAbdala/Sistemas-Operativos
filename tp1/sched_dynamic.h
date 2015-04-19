#ifndef __SCHED_DYNAMIC_
#define __SCHED_DYNAMIC_

#include <vector>
#include <queue>
#include "basesched.h"

using namespace std;

class SchedDynamic : public SchedBase {
	public:
		SchedDynamic(std::vector<int> argn);
        	~SchedDynamic();
		virtual void initialize();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		/*Valor de retorno de tick*/
		int next(int pid);
		/*Indica el numero total de tareas a correr en la simulacion*/
		int total_tareas;
		/*Indica la cantidad de tareas listas*/
		int tareas_ready();
		/*Deadline de la tarea iesima*/
		int* deadline;
		/*Indica si la tarea iesima esta lista*/
		bool* ready;
		/*Numero de cores*/
		int cores;
};

#endif
