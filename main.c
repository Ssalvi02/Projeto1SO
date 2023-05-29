#include "semaforo.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

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
