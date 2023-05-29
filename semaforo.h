#ifndef SEMAFORO_H
#define SEMAFORO_H

#define N_ALUNOS 10
#define K_MONITORES 2
#define TAM_GRUPO 5
#define VAGAS_SALA 10

void init();
void *professor(void *arg);
void *monitor(void *arg);
void *aluno(void *arg);

#endif
