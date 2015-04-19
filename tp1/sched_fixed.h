#ifndef __SCHED_FIXED_
#define __SCHED_FIXED_

#include <vector>
#include <queue>
#include "basesched.h"

using namespace std;

class SchedFixed : public SchedBase {
	public:
		SchedFixed(std::vector<int> argn);
        ~SchedFixed();
		virtual void initialize();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		/*Cantidad de cores (por si acaso fuera util tenerla)*/
		int cores;
		/*Cola de prioridad para las tareas:
		En la estructura va a haber pares de la forma <periodo(pid),pid>,
		y la más prioritaria (o sea, la primera que salga con top() ) 
		va a ser la que tenga menor periodo
		*/
		priority_queue< pair<int,int>, 
			vector< pair<int,int> >,
		   	greater< pair<int,int> > > q;
};

#endif
