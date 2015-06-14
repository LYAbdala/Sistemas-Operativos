#include "RWLock.h"

RWLock::RWLock():cantidadDeSolicitudes(0) {
	(void) pthread_mutex_init(&roomEmpty,NULL);
	(void) pthread_mutex_init(&mutexSolicitudes,NULL);
}

void RWLock::rlock() {
	pthread_mutex_lock(&mutexSolicitudes);
	cantidadDeSolicitudes++;
	if(cantidadDeSolicitudes == 1){
		pthread_mutex_lock(&roomEmpty);
	}
	pthread_mutex_unlock(&mutexSolicitudes);
}

void RWLock::wlock() {
	pthread_mutex_lock(&roomEmpty);
}

void RWLock::runlock() {
	pthread_mutex_lock(&mutexSolicitudes);
	cantidadDeSolicitudes--;
	if (cantidadDeSolicitudes == 0){
		pthread_mutex_unlock(&roomEmpty);
	}
	pthread_mutex_unlock(&mutexSolicitudes);
}

void RWLock::wunlock() {
	pthread_mutex_unlock(&roomEmpty);
}

