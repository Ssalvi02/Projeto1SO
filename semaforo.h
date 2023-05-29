#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <semaphore.h>
#include <pthread.h>

#define N_ALUNOS 10
#define K_MONITORES 2
#define TAM_GRUPO 5
#define VAGAS_SALA 10

extern sem_t sem_aluno;
extern sem_t sem_monitor;
extern sem_t sem_monitor_livre;
extern sem_t sala_vazia;
extern sem_t sala_lotada;
extern sem_t sala_aberta;

extern pthread_mutex_t mutex;
extern int num_alunos;
extern int num_monitores;
extern int num_grupos;
extern int num_monitor_sair;

int post(sem_t *sem);
int wait(sem_t *sem);
int lock(pthread_mutex_t *mutex);
int unlock(pthread_mutex_t *mutex);
int rand_sec(int min, int max);
void *professor(void *arg);
void *monitor(void *arg);
void *aluno(void *arg);

#endif
