#include "sim_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#define N_ALUNOS 50
#define K_MONITORES 1
#define GROUP_SIZE 1
#define SALA_SIZE 30

sem_t sem_aluno;
sem_t sem_monitor;

int alunos_esperando = 0;
int num_alunos_na_sala = 0;
int monitores_esperando = 0;
int num_monitores_na_sala = 0;

int sala_fechada = 0;

void* alunos(void* id)
{
    int id = *(int* )id;

    usleep(time_between(0, 10)); //Sleep random

    // ----------------------------------------------------------------
    // Entrar na sala
    // ----------------------------------------------------------------
    // 1. Checar se a sala está aberta
    if(sala_fechada == 1)
    {
        printf("        Aluno %d chegou quando a sala já estava fechada...\n", id);
        pthread_exit(NULL);
    }
    // 2. Atualiza alunos em espera
    alunos_esperando++;
    printf("        Aluno %d quer entrar na sala.\n", id);
    sem_wait(&sem_aluno);
    // 3. Realiza a entrada
    num_alunos_na_sala++;
    alunos_esperando--;
    


    // ----------------------------------------------------------------
    // Estudar
    // ----------------------------------------------------------------

    // ----------------------------------------------------------------
    // Sair da sala
    // ----------------------------------------------------------------
}

void* monitores(void* id)
{
    // ----------------------------------------------------------------
    // Entrar na sala
    // ----------------------------------------------------------------

    // ----------------------------------------------------------------
    // Supervisionar
    // ----------------------------------------------------------------

    // ----------------------------------------------------------------
    // Sair da sala
    // ----------------------------------------------------------------
}

void* professor(void*)
{
    // ----------------------------------------------------------------
    // Abrir sala
    // ----------------------------------------------------------------

    // ----------------------------------------------------------------
    // Avisar alunos
    // ----------------------------------------------------------------
    
    // ----------------------------------------------------------------
    // Avisar Monitores
    // ----------------------------------------------------------------
 
    // ----------------------------------------------------------------
    // Fechar sala
    // ----------------------------------------------------------------
}


