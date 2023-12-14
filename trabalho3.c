// trabalho3.c
#include "trabalho3.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Fun��o de compara��o para o qsort (LPT)
int compararTarefas(const void *a, const void *b) {
    return ((Tarefa *)b)->tempo - ((Tarefa *)a)->tempo;
}

void alocarTarefa(Tarefa tarefas[], int indiceTarefa, int maquina, int tarefasAlocadas[][MAX_TAREFAS], int tarefasPorMaquina[]) {
    tarefas[indiceTarefa].completada = 1;

    // Adicionar tarefa � m�quina de destino
    int posicaoInsercao = tarefasPorMaquina[maquina - 1];
    for (int i = 0; i < tarefasPorMaquina[maquina - 1]; i++) {
        if (tarefasAlocadas[maquina - 1][i] > indiceTarefa + 1) {
            posicaoInsercao = i;
            break;
        }
    }

    // Deslocamento e Inser��o da Tarefa
    for (int i = tarefasPorMaquina[maquina - 1]; i > posicaoInsercao; i--) {
        tarefasAlocadas[maquina - 1][i] = tarefasAlocadas[maquina - 1][i - 1];
    }

    tarefasAlocadas[maquina - 1][posicaoInsercao] = indiceTarefa + 1;

    // Atualiza��o do Contador de Tarefas na M�quina
    tarefasPorMaquina[maquina - 1]++;
    // printf("Maquina %d: %d (tarefa alocada)\n", maquina, indiceTarefa + 1);

    // Remover tarefa da m�quina de origem
    for (int i = 0; i < NUM_MAQUINAS; i++) {
        if (i != maquina - 1) {
            for (int j = 0; j < tarefasPorMaquina[i]; j++) {
                if (tarefasAlocadas[i][j] == indiceTarefa + 1) {
                    // Remover a tarefa da m�quina de origem
                    for (int k = j; k < tarefasPorMaquina[i] - 1; k++) {
                        tarefasAlocadas[i][k] = tarefasAlocadas[i][k + 1];
                    }
                    tarefasPorMaquina[i]--;
                    break;
                }
            }
        }
    }
}

int calcularFuncaoObjetivo(int temposMaquina[], int numMaquinas) {
    // Inicialize a fun��o objetivo com o tempo da primeira m�quina
    int maxTempoMaquina = temposMaquina[0];

    // Encontre o tempo m�ximo entre todas as m�quinas
    for (int i = 1; i < numMaquinas; i++) {
        if (temposMaquina[i] > maxTempoMaquina) {
            maxTempoMaquina = temposMaquina[i];
        }
    }

    return maxTempoMaquina;
}


double calcularTempoExecucao(clock_t inicio) {
    clock_t agora = clock();
    return ((double)(agora - inicio)) / CLOCKS_PER_SEC;
}
