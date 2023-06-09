/****************************************************************************************************************************************************************************************************************
Autores: 
    Gabriela Paola Sereniski
    João Victor Salvi Silva
    Ingrid Reupke Sbeguen Moran

Data: 28/04/2023

Descrição:
    Implementação do problema proposto pelo professor e específicado no projeto 1 utilizando semáforos.
    
•••• Alunos:    Podem tentar entrar na sala mas só podem entrar, de fato, se houver um monitor na sala.
                Se a sala estiver aberta, o tempo limite não tiver esgotado ou o limite de vagas não atinjiu o máximo e houver pelo menos um monitor, o aluno pode entrar na sala.
                Saem da sala ao terminar de estudar.
    
•••• Professor: Abre a sala para os monitores.
                Alerta os monitores e alunos quando ele estiver pronto para fechar a sala, assim todos que estavam esperando vão embora.
                Fecha a sala se ela estiver vazia (sem alunos e monitores).
    
•••• Monitor:   Supervisiona o estudo de um grupo de alunos.
                Entra na sala se o professor já a tiver aberto.
                Pode tentar sair da sala a qualquer momento, mas só pode sair se houver outro monitor na sala ou se o professor já tiver avisado que irá fechar a sala e ele for o último a sair.
****************************************************************************************************************************************************************************************************************/


#include "sim_utils.h" // Funções auxiliares/utilitárias
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h> 
#include <time.h> 

#define N_ALUNOS 20 // Número de alunos
#define K_MONITORES 5 // Número de monitores
#define TAM_GRUPO 4 // Tamanho do grupo de alunos
#define VAGAS_SALA 15 // Vagas totais na sala

// Semáforos
sem_t sem_aluno;
sem_t sem_monitor;
sem_t sem_monitor_livre;
sem_t sala_vazia;
sem_t fechar_sala;

// Mutex
pthread_mutex_t mutex_alunos;
pthread_mutex_t mutex_monitores;

// Variáveis gerais para controle e checagem
int num_alunos = 0;
int num_alunos_entraram = 0;
int num_monitores = 0;
int num_alunos_esperando = 0;
int num_monitores_esperando = 0;
int num_grupos = 0;

bool professor_fechou_a_sala = false;

// Função de inicialização (usada para iniciar os semáforos e mutex)
void init()
{
    srand(time(NULL)); // Inicializa a seed aleatória

    // Inicializa os semáforos
    sem_init(&sem_aluno, 0, 0);
    sem_init(&sem_monitor, 0, 0);
    sem_init(&sem_monitor_livre, 0, 0);
    sem_init(&sala_vazia, 0, 0);
    sem_init(&fechar_sala, 0, 0);

    // Inicializa os mutex
    pthread_mutex_init(&mutex_alunos, NULL);
    pthread_mutex_init(&mutex_monitores, NULL);
}

// ----------- Função do professor -----------
void *professor(void *arg)
{

// ----------------------------------------------------------------
// Abrir sala
// ----------------------------------------------------------------
    usleep(time_between(0, 3)); // Espera um tempo aleatório para abrir a sala
    printf("Professor Campiolo abriu a sala.\n");

    // Libera a entrada para monitores
    for (int i = 0; i < K_MONITORES; i++)
    {
        post(&sem_monitor);
    }

// ----------------------------------------------------------------
// Fechar sala
// ----------------------------------------------------------------
    // Espera a sala lotar (ficar sem vagas) ou tempo limite estourar
    struct timespec timeout; // Inicia o tempo limite
    clock_gettime(CLOCK_REALTIME, &timeout); 
    timeout.tv_sec += 20; // Timeout de 20 segundos a partir da chamada da função acima
    
    int result = sem_timedwait(&fechar_sala, &timeout); // Checa se atingiu timeout
    
    if(!result)
    {
        // Se as vagas forem preenchidas
        printf("NUMERO MAXIMO DE ALUNOS\n");
    }
    else 
    {
        // Se o atingir o timeout
        printf("TEMPO MAXIMO DE ESPERA\n");
    }
    printf("Professor avisa que ninguem mais pode entrar.\n");
    professor_fechou_a_sala = true; // Fecha a sala

// ----------------------------------------------------------------
// Avisar monitores e alunos
// ----------------------------------------------------------------
    // Acorda as threads bloqueadas para que elas terminem
    lock(&mutex_alunos);
        while(num_alunos_esperando--)
        {
            post(&sem_aluno);
        }
    unlock(&mutex_alunos);

    lock(&mutex_monitores);
        while(num_monitores_esperando--)
        {
            post(&sem_monitor);
        }
    unlock(&mutex_monitores);

    // Espera os monitores e alunos sairem da sala (completamente vazia)
    wait(&sala_vazia);
    printf("Todos sairam da sala. Professor Campiolo a fechou.\n");

    pthread_exit(NULL); // Sai da thread
}

// ----------- Função do monitor -----------
void *monitor(void *arg)
{

// ----------------------------------------------------------------
// Entrar na sala
// ----------------------------------------------------------------
    int id = *(int *)arg; // Atribui um ID à thread

    usleep(time_between(0, 5)); // Espera um tempo aleatório para tentar entrar na sala

    // Termina a thread caso ela inicie após a porta fechar
    if(professor_fechou_a_sala)
    {
        printf("    Monitor_%d chegou quando a sala ja estava fechada.\n", id);
        pthread_exit(NULL);
    }

    // Atualiza a quantidade de monitores em espera
    lock(&mutex_monitores);
        num_monitores_esperando++;
    unlock(&mutex_monitores);

    printf("    Monitor_%d esta esperando para entrar.\n", id);
    wait(&sem_monitor);

    // Caso a sala feche enquanto o monitor espera, o professor avisa que a sala fechou e a thread termina
    if(professor_fechou_a_sala)
    {
        printf("    Monitor_%d nao pode entrar e foi embora.\n", id);
        pthread_exit(NULL);
    }

// ----------------------------------------------------------------
// Supervisionar
// ----------------------------------------------------------------
    // Atualiza a quantidade de monitores na sala
    lock(&mutex_monitores);
        num_monitores++; // Aumenta a quantidade de monitores 
        num_monitores_esperando--; // Diminui a quantidade de monitores em espera 
        printf("    Monitor_%d entrou na sala e esta supervisionando.\n", id);        
    unlock(&mutex_monitores);    

    // Liberar a entrada de X alunos na sala
    for (int i = 0; i < TAM_GRUPO; i++)
    {
        post(&sem_aluno);
    }

    // Tempo aleatório de supervisão
    usleep(time_between(5, 15));
    
// ----------------------------------------------------------------
// Sair da sala
// ----------------------------------------------------------------
    // Espera os alunos sairem para poder sair (se num_grupos for menor que num_monitores)
    printf("    Monitor_%d quer sair da sala\n", id);
    wait(&sem_monitor_livre);
    
    lock(&mutex_monitores);
        num_monitores--;
        // Bloqueia a entrada de X alunos
        for(int i = 0; i < TAM_GRUPO; i++)
        {
            sem_wait(&sem_aluno);
        }

        printf("    Monitor_%d saiu da sala\n", id);
        
        // Se o monitor for o últomo a sair, avisa que a sala está vazia
        if (num_monitores == 0)
        {
            sem_post(&sala_vazia);
        }
    unlock(&mutex_monitores);

    pthread_exit(NULL); // Sai da thread
}

// ----------- Função do aluno -----------
void *aluno(void *arg)
{
// ----------------------------------------------------------------
// Entrar na sala
// ----------------------------------------------------------------
    int id = *(int *)arg; // Atribui um ID à thread

    usleep(time_between(0, 10)); // Espera um tempo aleatório para tentar entrar na sala

    // Termina a thread caso ela inicie após a porta fechar
    if(professor_fechou_a_sala)
    {
        printf("        Aluno_%d chegou quando a sala ja estava fechada.\n", id);
        pthread_exit(NULL);
    }

    // Atualiza a quantidade de alunos em espera
    lock(&mutex_alunos);
        num_alunos_esperando++;
    unlock(&mutex_alunos);

    printf("        Aluno_%d quer entrar na sala\n", id);
    wait(&sem_aluno);

    // Caso a sala feche enquanto o aluno espera, o professor avisa que a sala fechou e a thread termina
    if(professor_fechou_a_sala)
    {
        printf("        Aluno_%d nao pode entrar e foi embora.\n", id);
        pthread_exit(NULL);
    }

// ----------------------------------------------------------------
// Estudar
// ----------------------------------------------------------------
    // Atualiza a quantidade de alunos e grupos na sala
    lock(&mutex_alunos);
        num_alunos++; // Aumenta a quantidade de alunos 
        num_alunos_entraram++; // Aumenta a quantidade de alunos na sala
        num_alunos_esperando--; // Diminui a quantidade de alunos esperando
        num_grupos = ceil((float)num_alunos / TAM_GRUPO); // Calcula a quantidade de grupos (arredondada para cima)
        printf("        Aluno_%d entrou na sala e esta estudando.\n", id);

        // Se todos os alunos já entraram ou se a quantia máxima de vagas foi atingida
        if(num_alunos_entraram == N_ALUNOS || num_alunos == VAGAS_SALA)
        {
            post(&fechar_sala); // Professor fecha a sala
        }
    unlock(&mutex_alunos);

    usleep(time_between(5, 15)); // Tempo para o aluno ficar estudando
    printf("        Aluno_%d terminou de estudar\n", id);
    sleep(1);

// ----------------------------------------------------------------
// Sair da sala
// ----------------------------------------------------------------
    lock(&mutex_alunos);
        num_alunos--; // Diminiu a quantidade de alunos 
        num_grupos = ceil((float)num_alunos / TAM_GRUPO); // Atualiza a quantidade de grupos (arredondada para cima)
        printf("        Aluno_%d saiu da sala\n", id);
        if (num_grupos < num_monitores) // Libera o monitor se for o último grupo a terminar de estudar
        {
            post(&sem_monitor_livre);
        }
    unlock(&mutex_alunos);
    
    // Quando um aluno sair, outro pode entrar para ser atendido por um mesmo monitor
    post(&sem_aluno); 

    pthread_exit(NULL); // Sai da thread
}

// ----------- Função do main -----------
int main()
{
    init(); // Inicialização

    // Criação das threads
    pthread_t thread_professor;
    pthread_t thread_monitor[K_MONITORES];
    pthread_t thread_aluno[N_ALUNOS];

    // Vetores de IDs
    int id_aluno[N_ALUNOS];
    int id_monitor[K_MONITORES];

    srand(time(NULL)); // Gera seed aleatória

    // ----------- Inicialização de threads -----------
    pthread_create(&thread_professor, NULL, professor, NULL); // Inicialização de 1 professor

    for (int i = 0; i < K_MONITORES; i++) // Inicialização de K monitores
    {
        id_monitor[i] = i + 1;
        pthread_create(&thread_monitor[i], NULL, monitor, &id_monitor[i]);
    }

    for (int i = 0; i < N_ALUNOS; i++) // Inicialização de N alunos
    {
        id_aluno[i] = i + 1;
        pthread_create(&thread_aluno[i], NULL, aluno, &id_aluno[i]);
    }

    // ----------- Join de threads ----------- 
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
