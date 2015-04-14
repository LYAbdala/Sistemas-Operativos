#ifndef __SCHED_RR2__
#define __SCHED_RR2__

#include <vector>
#include <queue>
#include <list>
#include "basesched.h"

using namespace std;

class SchedRR2 : public SchedBase {
	public:
		SchedRR2(std::vector<int> argn);
        	~SchedRR2();
		virtual void initialize() {};
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		int next(int cpu);
		int cores;
		int* contador;
		int* quantum;
		int* contDeTareasActivas;
		//int indiceDeProcesador;
		queue<int>* colas;
		//list<pair<pid, cpu>>
		list<pair<int, int> > bloqueadas;
};

#endif
