// trabalho3.h

#ifndef TRABALHO3_H
#define TRABALHO3_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_MAQUINAS 6
#define MAX_TAREFAS 100
#define NUM_REPETICOES 5
#define NUM_IT_PERTURBACAO 50


typedef struct Tarefa {
    int id;
    int tempo;
    int completada;
} Tarefa;

int compararTarefas(const void *a, const void *b);
void alocarTarefa(Tarefa tarefas[], int indiceTarefa, int maquina, int tarefasAlocadas[][MAX_TAREFAS], int tarefasPorMaquina[]);
int calcularFuncaoObjetivo(int tempos[], int numMaquinas);
double calcularTempoExecucao(clock_t inicio);
#endif

