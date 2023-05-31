#ifndef SIM_UTILS_H
#define SIM_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

int post(sem_t *sem);
int wait(sem_t *sem);
int lock(pthread_mutex_t *mutex);
int unlock(pthread_mutex_t *mutex);

int time_between(int min, int max);

#endif