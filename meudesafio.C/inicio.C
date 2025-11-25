#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 10

//------------------------------------------------------------
// ESTRUTURA DA ÁRVORE BINÁRIA DA MANSÃO
//------------------------------------------------------------
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
    char pista[50]; // pista encontrada ao entrar
} Sala;

//------------------------------------------------------------
// ESTRUTURA DA ÁRVORE DE BUSCA (BST) PARA PISTAS
//------------------------------------------------------------
typedef struct NoBST {
    char pista[50];
    struct NoBST *esq, *dir;
} NoBST;

//------------------------------------------------------------
// ESTRUTURA DA TABELA HASH DE SUSPEITOS
//------------------------------------------------------------
typedef struct HashNo {
    char pista[50];
    char suspeito[50];
    struct HashNo *prox;
} HashNo;

HashNo *tabelaHash[TAM_HASH];

//------------------------------------------------------------
// Função hash simples — soma de caracteres da pista
//------------------------------------------------------------
int hash(char *str) {
    int soma = 0;
    for (int i = 0; str[i] != '\0'; i++)
        soma += str[i];
    return soma % TAM_HASH;
}

//------------------------------------------------------------
// Insere pista → suspeito na tabela hash
//------------------------------------------------------------
void inserirNaHash(char *pista, char *suspeito) {
    HashNo *novo = (HashNo*) malloc(sizeof(HashNo));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->prox = NULL;

    int indice = hash(pista);

    novo->prox = tabelaHash[indice];
    tabelaHash[indice] = novo;
}

//------------------------------------------------------------
// Insere pista na BST ordenadamente
//------------------------------------------------------------
NoBST* inserirBST(NoBST *raiz, char *pista) {
    if (raiz == NULL) {
        NoBST *novo = (NoBST*) malloc(sizeof(NoBST));
        strcpy(novo->pista, pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirBST(raiz->esq, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->dir = inserirBST(raiz->dir, pista);

    return raiz;
}

//------------------------------------------------------------
// Exibe pistas coletadas em ordem alfabética
//------------------------------------------------------------
void emOrdem(NoBST *raiz) {
    if (raiz == NULL) return;
    emOrdem(raiz->esq);
    printf("- %s\n", raiz->pista);
    emOrdem(raiz->dir);
}

//------------------------------------------------------------
// Mostra todas as associações pista → suspeito
//------------------------------------------------------------
void exibirHash() {
    printf("\n===== Associações Encontradas =====\n");
    for (int i = 0; i < TAM_HASH; i++) {
        HashNo *aux = tabelaHash[i];
        while (aux) {
            printf("Pista: %-20s → Suspeito: %s\n", aux->pista, aux->suspeito);
            aux = aux->prox;
        }
    }
}

//------------------------------------------------------------
// Retorna o suspeito mais relacionado
//------------------------------------------------------------
char* suspeitoMaisCitado() {
    static char vencedor[50];
    int maior = 0;

    for (int i = 0; i < TAM_HASH; i++) {
        HashNo *aux = tabelaHash[i];

        while (aux) {
            int cont = 0;
            HashNo *busca = tabelaHash[i];

            // Conta quantas vezes aparece
            while (busca) {
                if (strcmp(aux->suspeito, busca->suspeito) == 0)
                    cont++;
                busca = busca->prox;
            }

            if (cont > maior) {
                maior = cont;
                strcpy(vencedor, aux->suspeito);
            }
            aux = aux->prox;
        }
    }
    return vencedor;
}

//------------------------------------------------------------
// Cria dinamicamente uma sala
//------------------------------------------------------------
Sala* criarSala(char nome[], char pista[]) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = nova->direita = NULL;
    return nova;
}

//------------------------------------------------------------
// Exploração da mansão
//------------------------------------------------------------
void explorar(Sala *atual, NoBST **bst) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê entrou em: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("📌 Você encontrou uma pista: %s\n", atual->pista);
            *bst = inserirBST(*bst, atual->pista);

            // Exemplo de suspeitos associados
            if (strcmp(atual->pista, "Luvas sujas") == 0)
                inserirNaHash("Luvas sujas", "Jardineiro");
            if (strcmp(atual->pista, "Taça quebrada") == 0)
                inserirNaHash("Taça quebrada", "Mordomo");
            if (strcmp(atual->pista, "Bilhete rasgado") == 0)
                inserirNaHash("Bilhete rasgado", "Sobrinha");
        }

        if (!atual->esquerda && !atual->direita) {
            printf("Nenhum caminho restante. Fim da exploração!\n");
            return;
        }

        printf("\nEscolha:\n");
        if (atual->esquerda) printf("(e) Ir para esquerda\n");
        if (atual->direita) printf("(d) Ir para direita\n");
        printf("(s) Sair\n> ");

        scanf(" %c", &escolha);

        if (escolha == 'e' && atual->esquerda)
            atual = atual->esquerda;
        else if (escolha == 'd' && atual->direita)
            atual = atual->direita;
        else if (escolha == 's')
            return;
        else
            printf("Opção inválida!\n");
    }
}

//------------------------------------------------------------
// MAIN — Monta a mansão, inicia o jogo
//------------------------------------------------------------
int main() {
    NoBST *pistas = NULL;

    Sala *hall = criarSala("Hall de Entrada", "");
    Sala *biblioteca = criarSala("Biblioteca", "Bilhete rasgado");
    Sala *cozinha = criarSala("Cozinha", "Taça quebrada");
    Sala *jardim = criarSala("Jardim", "Luvas sujas");
    Sala *porão = criarSala("Porão", "");

    hall->esquerda = biblioteca;
    hall->direita = cozinha;
    biblioteca->esquerda = jardim;
    cozinha->direita = porão;

    printf("===== Detective Quest — Missão Avançada =====\n");
    explorar(hall, &pistas);

    printf("\n===== Pistas coletadas =====\n");
    emOrdem(pistas);

    exibirHash();

    printf("\n🔎 Principal suspeito: %s\n", suspeitoMaisCitado());

    printf("\nObrigado por jogar!\n");
    return 0;
}
