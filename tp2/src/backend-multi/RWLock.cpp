#include "RWLock.h"

RWLock::RWLock():cantidadDeSolicitudes(0) {
	(void) pthread_mutex_init(&roomEmpty,NULL);
}

void RWLock::rlock() {
	if(cantidadDeSolicitudes == 0){
		pthread_mutex_lock(&roomEmpty);
	}
	cantidadDeSolicitudes++;
}

void RWLock::wlock() {
	pthread_mutex_lock(&roomEmpty);
}

void RWLock::runlock() {
	cantidadDeSolicitudes--;
	if (!cantidadDeSolicitudes)
		pthread_mutex_unlock(&roomEmpty);
}

void RWLock::wunlock() {
	pthread_mutex_unlock(&roomEmpty);
}

