#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long contador = 0;

int comparar_int(const void *a, const void *b) {
    contador++;
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

typedef struct No {
    void *dado;
    struct No *prox;
} No;

No *lista_inserir(No *ini, void *dado) {
    No *novo = malloc(sizeof(No));
    if (!novo) exit(1);
    novo->dado = dado;
    novo->prox = ini;
    return novo;
}

int lista_buscar(No *ini, void *chave, int (*cmp)(const void *, const void *)) {
    for (No *p = ini; p; p = p->prox)
        if (cmp(p->dado, chave) == 0) return 1;
    return 0;
}

void lista_liberar(No *ini) {
    while (ini) {
        No *prox = ini->prox;
        free(ini);
        ini = prox;
    }
}

typedef struct Arv {
    void *dado;
    struct Arv *esq, *dir;
} Arv;

Arv *abb_inserir(Arv *raiz, void *dado, int (*cmp)(const void *, const void *)) {
    if (!raiz) {
        Arv *n = malloc(sizeof(Arv));
        if (!n) exit(1);
        n->dado = dado;
        n->esq = n->dir = NULL;
        return n;
    }
    if (cmp(dado, raiz->dado) < 0) raiz->esq = abb_inserir(raiz->esq, dado, cmp);
    else                           raiz->dir = abb_inserir(raiz->dir, dado, cmp);
    return raiz;
}

int abb_buscar(Arv *raiz, void *chave, int (*cmp)(const void *, const void *)) {
    while (raiz) {
        int c = cmp(chave, raiz->dado);
        if (c == 0) return 1;
        raiz = (c < 0) ? raiz->esq : raiz->dir;
    }
    return 0;
}

void abb_liberar(Arv *raiz) {
    if (!raiz) return;
    abb_liberar(raiz->esq);
    abb_liberar(raiz->dir);
    free(raiz);
}
int main(void) {
    int N, BUSCAS;

    printf("Quantos dados? ");
    if (scanf("%d", &N) != 1 || N <= 0) { printf("Valor invalido.\n"); return 1; }
    printf("Quantos sorteios? ");
    if (scanf("%d", &BUSCAS) != 1 || BUSCAS <= 0) { printf("Valor invalido.\n"); return 1; }
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    int *valores = malloc(N * sizeof(int));
    if (!valores) return 1;
    No *lista = NULL;
    Arv *abb = NULL;

    srand((unsigned)time(NULL));
    for (int i = 0; i < N; i++) {
        valores[i] = rand();
        lista = lista_inserir(lista, &valores[i]);
        abb = abb_inserir(abb, &valores[i], comparar_int);
    }
    printf("%d dados armazenados na lista e na ABB.\n", N);

    FILE *f = fopen("comparacoes.csv", "w");
    if (!f) { perror("comparacoes.csv"); return 1; }
    fprintf(f, "busca,lista,abb\n");

    for (int i = 0; i < BUSCAS; i++) {
        
        getchar();

        int sorteado = valores[rand() % N];

        contador = 0;
        lista_buscar(lista, &sorteado, comparar_int);
        long c_lista = contador;

        contador = 0;
        abb_buscar(abb, &sorteado, comparar_int);
        long c_abb = contador;

        printf("Numero sorteado: %d\n", sorteado);
        printf("Comparacoes na lista = %ld\n", c_lista);
        printf("Comparacoes na ABB   = %ld\n", c_abb);
        fprintf(f, "%d,%ld,%ld\n", i + 1, c_lista, c_abb);
    }
    fclose(f);

    lista_liberar(lista);
    abb_liberar(abb);
    free(valores);
    return 0;
}
