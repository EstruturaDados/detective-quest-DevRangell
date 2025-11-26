#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --------------------------------------------
// Estruturas de dados
// --------------------------------------------

// Nó da árvore de salas (mapa da mansão)
typedef struct Sala {
    char nome[50];
    char pista[100];      // pista estática associada à sala (pode ser string vazia)
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// Nó da BST que armazena pistas coletadas
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

// Nó para encadeamento na tabela hash (associação pista -> suspeito)
typedef struct HashNode {
    char pista[100];
    char suspeito[50];
    struct HashNode *proximo;
} HashNode;

// Tabela hash simples com encadeamento
#define HASH_SIZE 53
typedef struct HashTable {
    HashNode *tabela[HASH_SIZE];
} HashTable;

// --------------------------------------------
// Prototipação de funções (documentadas abaixo)
// --------------------------------------------

// criarSala() – cria dinamicamente um cômodo.
Sala* criarSala(const char *nome, const char *pista);

// explorarSalas() – navega pela árvore e ativa o sistema de pistas.
void explorarSalas(Sala *atual, PistaNode **arvorePistas);

// inserirPista() / adicionarPista() – insere a pista coletada na árvore de pistas.
PistaNode* inserirPista(PistaNode *raiz, const char *pista);

// inserirNaHash() – insere associação pista/suspeito na tabela hash.
void inserirNaHash(HashTable *h, const char *pista, const char *suspeito);

// encontrarSuspeito() – consulta o suspeito correspondente a uma pista.
const char* encontrarSuspeito(HashTable *h, const char *pista);

// verificarSuspeitoFinal() – conduz à fase de julgamento final.
void verificarSuspeitoFinal(PistaNode *pistasColetadas, HashTable *h);

// utilitárias
void exibirPistas(PistaNode *raiz);
void liberarSalas(Sala *raiz);
void liberarPistas(PistaNode *raiz);
void liberarHash(HashTable *h);

// --------------------------------------------
// Implementação
// --------------------------------------------

// Função utilitária: compara strings ignorando case
int igualIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

// -------------------------------------------------
// criarSala: cria dinamicamente um cômodo
// -------------------------------------------------
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        fprintf(stderr, "Erro ao alocar memória para sala!\n");
        exit(EXIT_FAILURE);
    }
    strncpy(nova->nome, nome, sizeof(nova->nome)-1);
    nova->nome[sizeof(nova->nome)-1] = '\0';

    if (pista && strlen(pista) > 0) {
        strncpy(nova->pista, pista, sizeof(nova->pista)-1);
        nova->pista[sizeof(nova->pista)-1] = '\0';
    } else {
        nova->pista[0] = '\0';
    }

    nova->esq = nova->dir = NULL;
    return nova;
}

// -------------------------------------------------
// inserirPista: insere a pista coletada na BST
// Retorna a raiz atualizada. Pistas duplicadas não são inseridas.
// -------------------------------------------------
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novo) { fprintf(stderr, "Erro alocando nó de pista\n"); exit(EXIT_FAILURE); }
        strncpy(novo->pista, pista, sizeof(novo->pista)-1);
        novo->pista[sizeof(novo->pista)-1] = '\0';
        novo->esq = novo->dir = NULL;
        return novo;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else if (cmp > 0)
        raiz->dir = inserirPista(raiz->dir, pista);
    // se igual, não insere (evita duplicatas)
    return raiz;
}

// -------------------------------------------------
// exibirPistas: imprime a BST em ordem alfabética
// -------------------------------------------------
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esq);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

// -------------------------------------------------
// Hash: função djb2 simples para strings
// -------------------------------------------------
unsigned long hashString(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

// -------------------------------------------------
// inserirNaHash: insere associação pista -> suspeito
// -------------------------------------------------
void inserirNaHash(HashTable *h, const char *pista, const char *suspeito) {
    unsigned long idx = hashString(pista) % HASH_SIZE;
    // verifica se já existe; se existir, sobrescreve o suspeito
    for (HashNode *n = h->tabela[idx]; n != NULL; n = n->proximo) {
        if (strcmp(n->pista, pista) == 0) {
            strncpy(n->suspeito, suspeito, sizeof(n->suspeito)-1);
            n->suspeito[sizeof(n->suspeito)-1] = '\0';
            return;
        }
    }
    // cria novo nó
    HashNode *novo = (HashNode*) malloc(sizeof(HashNode));
    if (!novo) { fprintf(stderr, "Erro alocar HashNode\n"); exit(EXIT_FAILURE); }
    strncpy(novo->pista, pista, sizeof(novo->pista)-1);
    novo->pista[sizeof(novo->pista)-1] = '\0';
    strncpy(novo->suspeito, suspeito, sizeof(novo->suspeito)-1);
    novo->suspeito[sizeof(novo->suspeito)-1] = '\0';
    novo->proximo = h->tabela[idx];
    h->tabela[idx] = novo;
}

// -------------------------------------------------
// encontrarSuspeito: retorna o suspeito associado a uma pista
// Retorna NULL se não encontrar.
// -------------------------------------------------
const char* encontrarSuspeito(HashTable *h, const char *pista) {
    unsigned long idx = hashString(pista) % HASH_SIZE;
    for (HashNode *n = h->tabela[idx]; n != NULL; n = n->proximo) {
        if (strcmp(n->pista, pista) == 0) return n->suspeito;
    }
    return NULL;
}

// -------------------------------------------------
// explorarSalas: navega pela árvore e ativa recolha de pistas
// Recebe ponteiro para raiz da BST de pistas (passado por referência)
// -------------------------------------------------
void explorarSalas(Sala *atual, PistaNode **arvorePistas) {
    char escolha;
    while (atual != NULL) {
        printf("\nVocê entrou em: %s\n", atual->nome);
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: %s\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        } else {
            printf("Nenhuma pista visível nesta sala.\n");
        }

        printf("Escolha (e) esquerda | (d) direita | (s) sair\n> ");
        scanf(" %c", &escolha);
        // limpa buffer restante na linha
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) ;

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esq) atual = atual->esq;
            else printf("Não existe caminho à esquerda.\n");
        } else if (escolha == 'd' || escolha == 'D') {
            if (atual->dir) atual = atual->dir;
            else printf("Não existe caminho à direita.\n");
        } else if (escolha == 's' || escolha == 'S') {
            printf("Você optou por encerrar a exploração.\n");
            return;
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}

// -------------------------------------------------
// Função auxiliar que conta quantas pistas da BST apontam para 'acusado'
// usa a tabela hash para mapear cada pista ao suspeito e compara
// -------------------------------------------------
int contarPistasParaSuspeito(PistaNode *raiz, HashTable *h, const char *acusado) {
    if (raiz == NULL) return 0;
    int count = 0;
    // conta à esquerda
    count += contarPistasParaSuspeito(raiz->esq, h, acusado);
    // verifica o nó atual
    const char *sus = encontrarSuspeito(h, raiz->pista);
    if (sus != NULL && igualIgnoreCase(sus, acusado)) count++;
    // conta à direita
    count += contarPistasParaSuspeito(raiz->dir, h, acusado);
    return count;
}

// -------------------------------------------------
// verificarSuspeitoFinal: solicita acusação e verifica evidências
// Regras: sucesso se ao menos 2 pistas coletadas apontarem para o suspeito
// -------------------------------------------------
void verificarSuspeitoFinal(PistaNode *pistasColetadas, HashTable *h) {
    if (pistasColetadas == NULL) {
        printf("Nenhuma pista coletada. Não é possível acusar ninguém.\n");
        return;
    }

    printf("\nPistas coletadas (ordem alfabética):\n");
    exibirPistas(pistasColetadas);

    char acusado[50];
    printf("\nDigite o nome do suspeito que deseja acusar: \n> ");
    if (!fgets(acusado, sizeof(acusado), stdin)) return;
    // remove newline
    acusado[strcspn(acusado, "\n")] = '\0';

    if (strlen(acusado) == 0) {
        printf("Nenhum nome informado. Acusação cancelada.\n");
        return;
    }

    int total = contarPistasParaSuspeito(pistasColetadas, h, acusado);
    printf("Pistas que apontam para %s: %d\n", acusado, total);
    if (total >= 2)
        printf("Resultado: Há evidências suficientes. %s é provavelmente o culpado!\n", acusado);
    else
        printf("Resultado: Evidências insuficientes para condenar %s. Continue investigando.\n", acusado);
}

// -------------------------------------------------
// Funções de liberação de memória
// -------------------------------------------------
void liberarSalas(Sala *raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esq);
    liberarSalas(raiz->dir);
    free(raiz);
}

void liberarPistas(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz);
}

void liberarHash(HashTable *h) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashNode *n = h->tabela[i];
        while (n) {
            HashNode *t = n->proximo;
            free(n);
            n = t;
        }
        h->tabela[i] = NULL;
    }
}

// --------------------------------------------
// main: monta o mapa fixo, popula a tabela hash
// e inicia exploração -> julgamento final
// --------------------------------------------
int main(void) {
    // Montagem do mapa (árvore binária fixa)
    Sala *hall = criarSala("Hall de Entrada", "Pegadas molhadas na entrada");
    Sala *salaEstar = criarSala("Sala de Estar", "Copo quebrado com impressões");
    Sala *cozinha = criarSala("Cozinha", "Faca ausente do conjunto");
    Sala *biblioteca = criarSala("Biblioteca", "Livro rasgado sobre o tapete");
    Sala *jardim = criarSala("Jardim", "Terra revirada perto do portão");
    Sala *adega = criarSala("Adega", "Garrafa com etiqueta de vinho rasgada");

    hall->esq = salaEstar;
    hall->dir = cozinha;
    salaEstar->esq = biblioteca;
    salaEstar->dir = jardim;
    cozinha->dir = adega;

    // Inicializa tabela hash e insere associações pista -> suspeito
    HashTable mapaSuspeitos = { {0} };

    inserirNaHash(&mapaSuspeitos, "Pegadas molhadas na entrada", "Sr. Verde");
    inserirNaHash(&mapaSuspeitos, "Copo quebrado com impressões", "Sra. Rosa");
    inserirNaHash(&mapaSuspeitos, "Faca ausente do conjunto", "Sr. Preto");
    inserirNaHash(&mapaSuspeitos, "Livro rasgado sobre o tapete", "Sra. Rosa");
    inserirNaHash(&mapaSuspeitos, "Terra revirada perto do portão", "Sr. Verde");
    inserirNaHash(&mapaSuspeitos, "Garrafa com etiqueta de vinho rasgada", "Sr. Preto");

    printf("=============================================\n");
    printf("  Detective Quest - Capítulo Final: Julgamento\n");
    printf("=============================================\n");

    PistaNode *pistasColetadas = NULL;
    explorarSalas(hall, &pistasColetadas);

    verificarSuspeitoFinal(pistasColetadas, &mapaSuspeitos);

    // Limpeza
    liberarSalas(hall);
    liberarPistas(pistasColetadas);
    liberarHash(&mapaSuspeitos);

    return 0;
}

