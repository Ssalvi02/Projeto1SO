#include "semaforo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

int post(sem_t *sem) { return sem_post(sem); }
int wait(sem_t *sem) { return sem_wait(sem); }
int lock(pthread_mutex_t *mutex) { return pthread_mutex_lock(mutex); }
int unlock(pthread_mutex_t *mutex) { return pthread_mutex_unlock(mutex); }

int rand_sec(int min, int max)
{
    min *= 1000000, max *= 1000000;
    return (rand() % (max - min + 1) + min);
}

void *professor(void *arg)
{
    usleep(rand_sec(0, 3));
    printf("Professor abre a sala.\n");
    for (int i = 0; i < K_MONITORES; i++)
        post(&sem_monitor);

    wait(&sala_vazia);
    printf("Professor fechou a sala\n");

    pthread_exit(NULL);
}

void *monitor(void *arg)
{
    int id = *(int *)arg;

    // tenta entrar na sala em tempo aleatorio
    usleep(rand_sec(0, 10));
    int monitor_pode_entrar;
    sem_getvalue(&sem_monitor, &monitor_pode_entrar);

    if (monitor_pode_entrar <= 0)
        printf("    Monitor_%d esta esperando para entrar na sala\n", id);

    wait(&sem_monitor);
    printf("    Monitor_%d entrou na sala\n", id);

    lock(&mutex);
    num_monitores++;
    for (int i = 0; i < TAM_GRUPO; i++)
        post(&sem_aluno); // alunos podem entrar
    unlock(&mutex);

    sleep(1);
    printf("    Monitor_%d esta supervisionando\n", id);
    usleep(rand_sec(5, 15));

    int monitor_pode_sair;
    sem_getvalue(&sem_monitor_livre, &monitor_pode_sair);
    if (monitor_pode_sair <= 0)
        printf("    Monitor_%d quer sair da sala\n", id);

    wait(&sem_monitor_livre);
    lock(&mutex);
    num_monitores--;
    for (int i = 0; i < TAM_GRUPO; i++)
        sem_wait(&sem_aluno);
    unlock(&mutex);

    if (num_monitores == 0 && num_alunos == 0)
        sem_post(&sala_vazia);

    printf("    Monitor_%d saiu da sala\n", id);
    pthread_exit(NULL);
}

void *aluno(void *arg)
{
    int id = *(int *)arg;

    // tenta entrar na sala em momento aleatorio
    usleep(rand_sec(0, 10));
    int aluno_pode_entrar;

    sem_getvalue(&sem_aluno, &aluno_pode_entrar);
    if (aluno_pode_entrar <= 0)
        printf("        Aluno_%d esta esperando um monitor para poder entrar\n", id);

    wait(&sem_aluno);
    printf("        Aluno_%d entrou na sala\n", id);

    lock(&mutex);
    num_alunos++;
    num_grupos = ceil((float)num_alunos / TAM_GRUPO);
    unlock(&mutex);
    sleep(1);

    printf("        Aluno_%d esta estudando...\n", id);
    usleep(rand_sec(5, 15));
    printf("        Aluno_%d terminou de estudar e esta saindo da sala...\n", id);

    lock(&mutex);
    num_alunos--;
    num_grupos = ceil((float)num_alunos / TAM_GRUPO);
    printf("        num grupos %d num alunos %d\n", num_grupos, num_alunos);
    unlock(&mutex);

    if (num_grupos < num_monitores)
        post(&sem_monitor_livre);

    pthread_exit(NULL);
}
