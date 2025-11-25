#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que representa uma sala da mansão (nó da árvore binária)
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

//------------------------------------------------------------
// Função: criarSala
// Cria dinamicamente uma sala, define seu nome e zera os caminhos
//------------------------------------------------------------
Sala* criarSala(char nome[]) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));

    if (nova == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

//------------------------------------------------------------
// Função: explorarSalas
// Permite o jogador navegar pela mansão interativamente
//------------------------------------------------------------
void explorarSalas(Sala *atual) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Se for nó-folha, para o jogo
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Não há mais caminhos. Fim da exploração!\n");
            return;
        }

        printf("Escolha um caminho:\n");
        if (atual->esquerda != NULL) printf("  (e) Ir para a esquerda\n");
        if (atual->direita != NULL) printf("  (d) Ir para a direita\n");
        printf("  (s) Sair do jogo\n> ");

        scanf(" %c", &escolha);

        if (escolha == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (escolha == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (escolha == 's') {
            printf("\nEncerrando exploração...\n");
            return;
        } else {
            printf("\nCaminho inválido! Tente novamente.\n");
        }
    }
}

//------------------------------------------------------------
// Função: main
// Monta a árvore binária da mansão e inicia a exploração
//------------------------------------------------------------
int main() {
    // Criando salas da mansão
    Sala *hall = criarSala("Hall de Entrada");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *cozinha = criarSala("Cozinha");
    Sala *jardim = criarSala("Jardim");
    Sala *porao = criarSala("Porão");
    Sala *escritorio = criarSala("Escritório");

    // Montando estrutura da árvore binária
    hall->esquerda = biblioteca;
    hall->direita = cozinha;

    biblioteca->esquerda = jardim;
    biblioteca->direita = porao;

    cozinha->direita = escritorio;

    printf("===== Detective Quest: A Mansão Misteriosa =====\n");
    explorarSalas(hall);

    printf("\nObrigado por jogar!\n");
    return 0;
}
