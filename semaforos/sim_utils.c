#include "sim_utils.h"

// Função de post para o semáforo
int post(sem_t *sem) 
{
    return sem_post(sem);
}

// Função de wait para o semáforo
int wait(sem_t *sem)
{
    return sem_wait(sem);
}

// Função de lock para o mutex
int lock(pthread_mutex_t *mutex)
{
    return pthread_mutex_lock(mutex);
}

// Função de unlock para o mutex
int unlock(pthread_mutex_t *mutex)
{
    return pthread_mutex_unlock(mutex);
}

// Sorteia um tempo entre min e max
int time_between(int min, int max)
{
    min *= 1000000, max *= 1000000;
    return (rand() % (max - min + 1) + min);
}
