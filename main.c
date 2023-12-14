#include "trabalho3.h"


int main() {
    srand(time(NULL));
    int numTarefas, i, j;
    Tarefa tarefas[MAX_TAREFAS];

    clock_t inicioTempo = clock();

    // Leitura dos dados de entrada
    FILE *arquivoEntrada = fopen("GUNTHER.IN2", "r");
    if (arquivoEntrada == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        exit(1);
    }

    fscanf(arquivoEntrada, "%d", &numTarefas);
    for (i = 0; i < numTarefas; i++) {
        fscanf(arquivoEntrada, "%d", &tarefas[i].tempo);
        tarefas[i].id = i + 1;
        tarefas[i].completada = 0;
    }

    // Redirecionar a saída para o arquivo "solucao.txt"
    FILE *arquivoSaida = fopen("solucao.txt", "w");
    if (arquivoSaida == NULL) {
        perror("Erro ao criar o arquivo de saída");
        exit(1);
    }

    int matrizDependencias[MAX_TAREFAS][MAX_TAREFAS] = {0};

    // Inicialização das dependências
    for (i = 0; i < numTarefas; i++) {
        for (j = 0; j < numTarefas; j++) {
            matrizDependencias[i][j] = 0;
        }
    }

    // Leitura das dependências
    while (1) {
        int tarefa1, tarefa2;
        fscanf(arquivoEntrada, "%d,%d", &tarefa1, &tarefa2);
        if (tarefa1 == -1 || tarefa2 == -1) {
            break;
        }
        matrizDependencias[tarefa2 - 1][tarefa1 - 1] = 1;
    }
    fclose(arquivoEntrada);

    int tarefasCompletadas = 0;
    int maquina = 1; // Inicie com a Máquina 1

    int tarefasAlocadas[NUM_MAQUINAS][MAX_TAREFAS];
    int tarefasPorMaquina[NUM_MAQUINAS] = {0};

    int temposMaquina[NUM_MAQUINAS] = {0}; // Inicialize os tempos de cada máquina com zero

    int tarefasPorMaquinaEquilibrado = numTarefas / NUM_MAQUINAS;
    int tarefasRestantes = numTarefas % NUM_MAQUINAS;

    int maxTempoMaquina = 0;  // Mova esta linha para fora do loop
    clock_t inicioMelhorSolucao = clock();


    int melhorTempoEncontrado = 1000;  // Inicialize com um valor grande
    for (int repeticao = 0; repeticao < NUM_REPETICOES; repeticao++) { // ----------------------------------------------repetiçao
        printf("Repeticao %d\n", repeticao + 1);

        while (tarefasCompletadas < numTarefas) {
            // Verificação de Prontidão da Tarefa para Alocação/iteração de tarefas disponíveis
            for (i = 0; i < numTarefas; i++) {
                if (!tarefas[i].completada) {
                    int pronta = 1;
                    for (j = 0; j < numTarefas; j++) {
                        if (matrizDependencias[i][j] && !tarefas[j].completada) {
                            pronta = 0;
                            break;
                        }
                    }
                    if (pronta) {
                        // Aloque a próxima tarefa na máquina atual e atualize os tempos
                        alocarTarefa(tarefas, i, maquina, tarefasAlocadas, tarefasPorMaquina);
                        temposMaquina[maquina - 1] += tarefas[i].tempo;
                        tarefasCompletadas++;

                        if (tarefasPorMaquina[maquina - 1] >= tarefasPorMaquinaEquilibrado + (maquina <= tarefasRestantes)) {
                            maquina = (maquina % NUM_MAQUINAS) + 1; // Próxima máquina
                        }
                    }
                }
            }
        }

        printf("-----------------------\n");

        // Impressão da alocação de tarefas em cada máquina (Solucao Inicial)
        for (i = 0; i < NUM_MAQUINAS; i++) {
            printf("Maquina %d: ", i + 1);
            for (j = 0; j < tarefasPorMaquina[i]; j++) {
                printf("%d", tarefasAlocadas[i][j]);
                if (j < tarefasPorMaquina[i] - 1) {
                    printf(",");
                }
            }
            printf("\n");
        }

        // Calcule a função objetivo inicial
        maxTempoMaquina = calcularFuncaoObjetivo(temposMaquina, NUM_MAQUINAS);

        // Impressão da Função Objetiva Inicial (FO Inicial)
        printf("FO Inicial: %d\n", maxTempoMaquina);
        printf("------------------------------------\n");

        clock_t inicioBuscaLocal = clock();
        double tempoLimite = 60.0; // Tempo limite em segundos ------------------------------------------------------TEMPO

        for (int it = 0; ; it++) {
            // Verificar critério de parada baseado no tempo
            clock_t tempoAtual = clock();
            double tempoDecorrido = (double)(tempoAtual - inicioBuscaLocal) / CLOCKS_PER_SEC;
            if (tempoDecorrido >= tempoLimite) {
                printf("Tempo limite atingido. Parando a busca local.\n");
                break;
            }
            // Verificar se ultrapassou o tempo para encontrar a melhor solução
            double tempoMelhorSolucao = (double)(tempoAtual - inicioMelhorSolucao) / CLOCKS_PER_SEC;
                if (tempoMelhorSolucao >= 60.0) {
                printf("Tempo limite para encontrar a melhor solucao atingido. Parando a busca local.\n");
                break;
            }

            // Lógica de perturbação
            for (i = 0; i < NUM_MAQUINAS; i++) {
                // Se houver pelo menos duas tarefas na máquina
                if (tarefasPorMaquina[i] >= 2) {
                    // Escolher duas tarefas aleatórias na máquina
                    int tarefaIndice1 = rand() % tarefasPorMaquina[i];
                    int tarefaIndice2;
                    do {
                        tarefaIndice2 = rand() % tarefasPorMaquina[i];
                    } while (tarefaIndice2 == tarefaIndice1);

                    int tarefaID1 = tarefasAlocadas[i][tarefaIndice1] - 1;
                    int tarefaID2 = tarefasAlocadas[i][tarefaIndice2] - 1;

                    // Simular a troca de tarefas
                    int temp = tarefasAlocadas[i][tarefaIndice1];
                    tarefasAlocadas[i][tarefaIndice1] = tarefasAlocadas[i][tarefaIndice2];
                    tarefasAlocadas[i][tarefaIndice2] = temp;

                    // Recalcular os tempos da máquina
                    temposMaquina[i] = 0;
                    for (j = 0; j < tarefasPorMaquina[i]; j++) {
                        int tarefaAtual = tarefasAlocadas[i][j] - 1;
                        temposMaquina[i] += tarefas[tarefaAtual].tempo;
                    }

                    // Recalcular a função objetivo
                    int novaFuncaoObjetivo = calcularFuncaoObjetivo(temposMaquina, NUM_MAQUINAS);

                    // Verificar se a nova solução é melhor do que a melhor conhecida -----------------------------------------------tempo melhor soluçao
                    if (novaFuncaoObjetivo < melhorTempoEncontrado) {
                        melhorTempoEncontrado = novaFuncaoObjetivo;

                        // Atualize o tempo da melhor solução
                        inicioMelhorSolucao = clock();
                    }
                    // Aceitar a troca se a nova função objetivo for menor
                    if (novaFuncaoObjetivo < maxTempoMaquina) {
                        maxTempoMaquina = novaFuncaoObjetivo;

                         // Atualize o tempo da melhor solução
                        inicioMelhorSolucao = clock();

                        // Se aceitar a troca, não é necessário reverter a perturbação
                    } else {
                        // Reverter a troca se a nova função objetivo não for menor
                        temp = tarefasAlocadas[i][tarefaIndice1];
                        tarefasAlocadas[i][tarefaIndice1] = tarefasAlocadas[i][tarefaIndice2];
                        tarefasAlocadas[i][tarefaIndice2] = temp;
                    }
                }

                // Recalcular os tempos da máquina
                for (int i = 0; i < NUM_MAQUINAS; i++) {
                    temposMaquina[i] = 0;
                    for (int j = 0; j < tarefasPorMaquina[i]; j++) {
                        int tarefaAtual = tarefasAlocadas[i][j] - 1;
                        temposMaquina[i] += tarefas[tarefaAtual].tempo;
                    }
                }
            }
            // Atualize o tempo da melhor solução
            inicioMelhorSolucao = clock();
        }
        // Impressão do tempo da melhor solução
printf("Melhor solucao encontrada em: %.6f segundos\n", (double)(clock() - inicioMelhorSolucao) / CLOCKS_PER_SEC);
fprintf(arquivoSaida, "Melhor solucao encontrada em: %.6f segundos\n", (double)(clock() - inicioMelhorSolucao) / CLOCKS_PER_SEC);


        // Escolha aleatoriamente uma tarefa e uma máquina para realocação
        int maquinaOrigem = rand() % NUM_MAQUINAS;
        int maquinaDestino = (maquinaOrigem + 1) % NUM_MAQUINAS;  // Máquina de destino diferente da de origem

        if (tarefasPorMaquina[maquinaOrigem] > 0) {
            int tarefaIndice = rand() % tarefasPorMaquina[maquinaOrigem];
            int tarefaID = tarefasAlocadas[maquinaOrigem][tarefaIndice] - 1;
            int tarefaTempo = tarefas[tarefaID].tempo;

            // Verificar se as dependências são atendidas
            int dependenciasAtendidas = 1;
            for (int j = 0; j < tarefasPorMaquina[maquinaOrigem]; j++) {
                int tarefaAlocadaAntes = tarefasAlocadas[maquinaOrigem][j] - 1;

                if (matrizDependencias[tarefaAlocadaAntes][tarefaID] == 1) {
                    // Existe uma dependência não atendida
                    dependenciasAtendidas = 0;
                    break;
                }
            }

            // Se as dependências forem atendidas, continue com a realocação
            if (dependenciasAtendidas) {
                // Simule a realocação
                temposMaquina[maquinaOrigem] -= tarefaTempo;
                temposMaquina[maquinaDestino] += tarefaTempo;

                // Calcule a nova função objetivo
                int novaFuncaoObjetivo = calcularFuncaoObjetivo(temposMaquina, NUM_MAQUINAS);

                // Aceite a mudança se a nova função objetivo for menor
                if (novaFuncaoObjetivo < maxTempoMaquina) {
                    // Realize a realocação
                    alocarTarefa(tarefas, tarefaID, maquinaDestino + 1, tarefasAlocadas, tarefasPorMaquina);
                    maxTempoMaquina = novaFuncaoObjetivo;
                    // Atualize o tempo da melhor solução
    inicioMelhorSolucao = clock();
                }
            }
        }
    // Impressão da Função Objetiva após a busca local (FO Após Busca Local)
    printf("FO Apos Busca Local: %d\n", maxTempoMaquina);

    fprintf(arquivoSaida, "FO Apos Busca Local: %d\n", maxTempoMaquina);
}

    printf("Solucao Apos Busca Local:\n");

    // Impressão da alocação de tarefas após a busca local
    for (i = 0; i < NUM_MAQUINAS; i++) {
        printf("Maquina %d: ", i + 1);
        for (j = 0; j < tarefasPorMaquina[i]; j++) {
            printf("%d", tarefasAlocadas[i][j]);
            if (j < tarefasPorMaquina[i] - 1) {
                printf(",");
            }
        }
        printf("\n");
    }

    // Impressão da Função Objetiva após a busca local (FO Após Busca Local)
    printf("FO Apos Busca Local: %d\n", maxTempoMaquina);


    clock_t fimTempo = clock();
    double execucaoTempo = (double)(fimTempo - inicioTempo) / CLOCKS_PER_SEC;

    printf("Tempo de execucao, em segundos: %.5f\n", execucaoTempo);
    printf("------------------------------------\n");
    fprintf(arquivoSaida, "Tempo de execucao, em segundos: %.5f\n", execucaoTempo);

    fclose(arquivoSaida);  // Fechar o arquivo após a escrita
    return 0;
}
