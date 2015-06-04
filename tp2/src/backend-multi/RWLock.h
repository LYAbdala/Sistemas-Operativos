#ifndef RWLock_h
#define RWLock_h
#include <iostream>

class RWLock {
    public:
        RWLock();
        void rlock();
        void wlock();
        void runlock();
        void wunlock();

    private:
        //pthread_rwlock_t rwlock;
        pthread_mutex_t roomEmpty;
        int cantidadDeSolicitudes;
};

#endif
