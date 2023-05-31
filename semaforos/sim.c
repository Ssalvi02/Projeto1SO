#include "sim_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

#define N_ALUNOS 20
#define K_MONITORES 5
#define TAM_GRUPO 4
#define VAGAS_SALA 15

sem_t sem_aluno;
sem_t sem_monitor;
sem_t sem_monitor_livre;
sem_t sala_vazia;
sem_t fechar_sala;

pthread_mutex_t mutex_alunos;
pthread_mutex_t mutex_monitores;
int num_alunos = 0;
int num_alunos_entraram = 0;
int num_monitores = 0;
int num_alunos_esperando = 0;
int num_monitores_esperando = 0;
int num_grupos = 0;

bool professor_fechou_a_sala = false;

void init()
{
    srand(time(NULL));

    sem_init(&sem_aluno, 0, 0);
    sem_init(&sem_monitor, 0, 0);
    sem_init(&sem_monitor_livre, 0, 0);
    sem_init(&sala_vazia, 0, 0);
    sem_init(&fechar_sala, 0, 0);

    pthread_mutex_init(&mutex_alunos, NULL);
    pthread_mutex_init(&mutex_monitores, NULL);
}

void *professor(void *arg)
{
    usleep(time_between(0, 3));
    printf("Professor Campiolo abriu a sala.\n");

    // libera entrada de monitores
    for (int i = 0; i < K_MONITORES; i++)
        post(&sem_monitor);

    // espera a sala lotar ou tempo limite passar
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 20;
    int result = sem_timedwait(&fechar_sala, &timeout);
    if(!result) printf("NUMERO MAXIMO DE ALUNOS\n");
    else        printf("TEMPO MAXIMO DE ESPERA\n");
    printf("Professor avisa que ninguem mais pode entrar.\n");
    professor_fechou_a_sala = true;

    // acorda threads bloqueadas para que elas terminem
    lock(&mutex_alunos);
        while(num_alunos_esperando--)
            post(&sem_aluno);
    unlock(&mutex_alunos);

    lock(&mutex_monitores);
        while(num_monitores_esperando--)
            post(&sem_monitor);
    unlock(&mutex_monitores);

    // espera todos sairem
    wait(&sala_vazia);
    printf("Todos sairam da sala. Professor Campiolo a fechou.\n");

    pthread_exit(NULL);
}

void *monitor(void *arg)
{
    int id = *(int *)arg;

    // tenta entrar na sala em tempo aleatorio
    usleep(time_between(0, 5));

    // termina a thread caso ela so inicie depois da porta fechar
    if(professor_fechou_a_sala)
    {
        printf("    Monitor_%d chegou quando a sala ja estava fechada.\n", id);
        pthread_exit(NULL);
    }

    // atualiza numero de monitores em espera
    lock(&mutex_monitores);
        num_monitores_esperando++;
    unlock(&mutex_monitores);

    printf("    Monitor_%d esta esperando para entrar.\n", id);
    wait(&sem_monitor);

    // caso a sala feche enquanto o monitor espera, o professor avisa que a sala fechou e a thread termina
    if(professor_fechou_a_sala)
    {
        printf("    Monitor_%d nao pode entrar e foi embora.\n", id);
        pthread_exit(NULL);
    }

    // atualiza numero de monitores na sala
    lock(&mutex_monitores);
        num_monitores++;
        num_monitores_esperando--;
        printf("    Monitor_%d entrou na sala e esta supervisionando.\n", id);        
    unlock(&mutex_monitores);    

    // monitor libera a entrada de X alunos
    for (int i = 0; i < TAM_GRUPO; i++)
        post(&sem_aluno);

    // tempo de supervisao
    usleep(time_between(5, 15));
    
    // espera alunos sairem para poder sair (num_grupos < num_monitores)
    printf("    Monitor_%d quer sair da sala\n", id);
    wait(&sem_monitor_livre);
    
    lock(&mutex_monitores);
        num_monitores--;
        // bloqueia a entrada de X alunos
        for(int i = 0; i < TAM_GRUPO; i++)
            sem_wait(&sem_aluno);
        
        printf("    Monitor_%d saiu da sala\n", id);
        
        // se o monitor for o ultimo a sair, avisa que a sala esta vazia
        if (num_monitores == 0)
            sem_post(&sala_vazia);
    unlock(&mutex_monitores);

    pthread_exit(NULL);
}

void *aluno(void *arg)
{
    int id = *(int *)arg;

    // tenta entrar na sala em momento aleatorio
    usleep(time_between(0, 10));

    // termina a thread caso ela so inicie depois da porta fechar
    if(professor_fechou_a_sala)
    {
        printf("        Aluno_%d chegou quando a sala ja estava fechada.\n", id);
        pthread_exit(NULL);
    }

    // atualiza numero de alunos em espera
    lock(&mutex_alunos);
        num_alunos_esperando++;
    unlock(&mutex_alunos);

    printf("        Aluno_%d quer entrar na sala\n", id);
    wait(&sem_aluno);

    // caso a sala feche enquanto o aluno espera, o professor avisa que a sala fechou e a thread termina
    if(professor_fechou_a_sala)
    {
        printf("        Aluno_%d nao pode entrar e foi embora.\n", id);
        pthread_exit(NULL);
    }

    // atualiza numero de alunos e numero de grupos na sala
    lock(&mutex_alunos);
        num_alunos++;
        num_alunos_entraram++;
        num_grupos = ceil((float)num_alunos / TAM_GRUPO);
        printf("        Aluno_%d entrou na sala e esta estudando.\n", id);
        // se todos os alunos ja entraram ou se o limite de vagas foi atingido
        if(num_alunos_entraram == N_ALUNOS || num_alunos == VAGAS_SALA) 
            post(&fechar_sala); // professor fecha a sala
    unlock(&mutex_alunos);

    usleep(time_between(5, 15));
    printf("        Aluno_%d terminou de estudar\n", id);
    sleep(1);

    lock(&mutex_alunos);
        num_alunos--;
        num_grupos = ceil((float)num_alunos / TAM_GRUPO);
        printf("        Aluno_%d saiu da sala\n", id);
        // se algum monitor quer sair e o aluno que saiu
        // era o ultimo de um grupo, o monitor pode sair.
        // tambem nao pode ter ninguem esperando pelo atendimento do monitor
        if (num_grupos < num_monitores)
            post(&sem_monitor_livre);
    unlock(&mutex_alunos);
    
    // quando um aluno sai
    // o outro pode entrar pra ser atendido pelo mesmo monitor
    post(&sem_aluno); //DANGER

    pthread_exit(NULL);
}

int main()
{
    init();

    pthread_t thread_professor;
    pthread_t thread_monitor[K_MONITORES];
    pthread_t thread_aluno[N_ALUNOS];

    int id_aluno[N_ALUNOS];
    int id_monitor[K_MONITORES];

    srand(time(NULL));

    pthread_create(&thread_professor, NULL, professor, NULL);

    for (int i = 0; i < K_MONITORES; i++)
    {
        id_monitor[i] = i + 1;
        pthread_create(&thread_monitor[i], NULL, monitor, &id_monitor[i]);
    }

    for (int i = 0; i < N_ALUNOS; i++)
    {
        id_aluno[i] = i + 1;
        pthread_create(&thread_aluno[i], NULL, aluno, &id_aluno[i]);
    }

    for (int i = 0; i < N_ALUNOS; i++)
    {
        pthread_join(thread_aluno[i], NULL);
    }

    for (int i = 0; i < K_MONITORES; i++)
    {
        pthread_join(thread_monitor[i], NULL);
    }

    pthread_join(thread_professor, NULL);

    return 0;
}