#include "sim_utils.h"


int post(sem_t *sem) 
{
    return sem_post(sem);
}

int wait(sem_t *sem)
{
    return sem_wait(sem);
}

int lock(pthread_mutex_t *mutex)
{
    return pthread_mutex_lock(mutex);
}

int unlock(pthread_mutex_t *mutex)
{
    return pthread_mutex_unlock(mutex);
}

int time_between(int min, int max)
{
    min *= 1000000, max *= 1000000;
    return (rand() % (max - min + 1) + min);
}
