#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int n;
    int folha;
    int *chaves;
    struct No **filhos;
} No;

typedef struct {
    No *raiz;
    int t;
} ArvoreB;

No *criar_no(int t, int folha) {
    No *no = malloc(sizeof(No));
    no->n = 0;
    no->folha = folha;
    no->chaves = malloc((2 * t - 1) * sizeof(int));
    no->filhos = malloc(2 * t * sizeof(No *));
    for (int i = 0; i < 2 * t; i++)
        no->filhos[i] = NULL;
    return no;
}

ArvoreB *criar_arvore(int t) {
    ArvoreB *arv = malloc(sizeof(ArvoreB));
    arv->t = t;
    arv->raiz = criar_no(t, 1);
    return arv;
}

void liberar_no(No *no) {
    if (no == NULL)
        return;
    if (!no->folha) {
        for (int i = 0; i <= no->n; i++)
            liberar_no(no->filhos[i]);
    }
    free(no->chaves);
    free(no->filhos);
    free(no);
}

void liberar_arvore(ArvoreB *arv) {
    liberar_no(arv->raiz);
    free(arv);
}

No *buscar(No *no, int chave, int *pos) {
    int i = 0;
    while (i < no->n && chave > no->chaves[i])
        i++;
    if (i < no->n && chave == no->chaves[i]) {
        *pos = i;
        return no;
    }
    if (no->folha)
        return NULL;
    return buscar(no->filhos[i], chave, pos);
}

void dividir_filho(No *pai, int i, int t) {
    No *cheio = pai->filhos[i];
    No *novo = criar_no(t, cheio->folha);
    novo->n = t - 1;

    for (int j = 0; j < t - 1; j++)
        novo->chaves[j] = cheio->chaves[j + t];

    if (!cheio->folha) {
        for (int j = 0; j < t; j++)
            novo->filhos[j] = cheio->filhos[j + t];
    }
    cheio->n = t - 1;

    for (int j = pai->n; j > i; j--)
        pai->filhos[j + 1] = pai->filhos[j];
    pai->filhos[i + 1] = novo;

    for (int j = pai->n - 1; j >= i; j--)
        pai->chaves[j + 1] = pai->chaves[j];
    pai->chaves[i] = cheio->chaves[t - 1];
    pai->n++;
}

void inserir_nao_cheio(No *no, int chave, int t) {
    int i = no->n - 1;

    if (no->folha) {
        while (i >= 0 && chave < no->chaves[i]) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = chave;
        no->n++;
        return;
    }

    while (i >= 0 && chave < no->chaves[i])
        i--;
    i++;

    if (no->filhos[i]->n == 2 * t - 1) {
        dividir_filho(no, i, t);
        if (chave > no->chaves[i])
            i++;
    }
    inserir_nao_cheio(no->filhos[i], chave, t);
}

int inserir(ArvoreB *arv, int chave) {
    int pos;
    if (buscar(arv->raiz, chave, &pos) != NULL)
        return 0;

    int t = arv->t;
    No *raiz = arv->raiz;

    if (raiz->n == 2 * t - 1) {
        No *nova = criar_no(t, 0);
        nova->filhos[0] = raiz;
        arv->raiz = nova;
        dividir_filho(nova, 0, t);
        inserir_nao_cheio(nova, chave, t);
    } else {
        inserir_nao_cheio(raiz, chave, t);
    }
    return 1;
}

int maior_chave(No *no) {
    while (!no->folha)
        no = no->filhos[no->n];
    return no->chaves[no->n - 1];
}

int menor_chave(No *no) {
    while (!no->folha)
        no = no->filhos[0];
    return no->chaves[0];
}

void juntar(No *pai, int i, int t) {
    No *esq = pai->filhos[i];
    No *dir = pai->filhos[i + 1];

    esq->chaves[t - 1] = pai->chaves[i];

    for (int j = 0; j < dir->n; j++)
        esq->chaves[j + t] = dir->chaves[j];

    if (!esq->folha) {
        for (int j = 0; j <= dir->n; j++)
            esq->filhos[j + t] = dir->filhos[j];
    }

    for (int j = i + 1; j < pai->n; j++)
        pai->chaves[j - 1] = pai->chaves[j];
    for (int j = i + 2; j <= pai->n; j++)
        pai->filhos[j - 1] = pai->filhos[j];

    esq->n += dir->n + 1;
    pai->n--;

    free(dir->chaves);
    free(dir->filhos);
    free(dir);
}

void pegar_da_esquerda(No *pai, int i) {
    No *filho = pai->filhos[i];
    No *irmao = pai->filhos[i - 1];

    for (int j = filho->n - 1; j >= 0; j--)
        filho->chaves[j + 1] = filho->chaves[j];

    if (!filho->folha) {
        for (int j = filho->n; j >= 0; j--)
            filho->filhos[j + 1] = filho->filhos[j];
    }

    filho->chaves[0] = pai->chaves[i - 1];
    if (!filho->folha)
        filho->filhos[0] = irmao->filhos[irmao->n];

    pai->chaves[i - 1] = irmao->chaves[irmao->n - 1];

    filho->n++;
    irmao->n--;
}

void pegar_da_direita(No *pai, int i) {
    No *filho = pai->filhos[i];
    No *irmao = pai->filhos[i + 1];

    filho->chaves[filho->n] = pai->chaves[i];
    if (!filho->folha)
        filho->filhos[filho->n + 1] = irmao->filhos[0];

    pai->chaves[i] = irmao->chaves[0];

    for (int j = 1; j < irmao->n; j++)
        irmao->chaves[j - 1] = irmao->chaves[j];

    if (!irmao->folha) {
        for (int j = 1; j <= irmao->n; j++)
            irmao->filhos[j - 1] = irmao->filhos[j];
    }

    filho->n++;
    irmao->n--;
}

void garantir_minimo(No *pai, int i, int t) {
    if (i > 0 && pai->filhos[i - 1]->n >= t) {
        pegar_da_esquerda(pai, i);
    } else if (i < pai->n && pai->filhos[i + 1]->n >= t) {
        pegar_da_direita(pai, i);
    } else if (i < pai->n) {
        juntar(pai, i, t);
    } else {
        juntar(pai, i - 1, t);
    }
}

void tirar_no(No *no, int chave, int t);

void tirar_de_no_interno(No *no, int i, int t) {
    int chave = no->chaves[i];

    if (no->filhos[i]->n >= t) {
        int antecessor = maior_chave(no->filhos[i]);
        no->chaves[i] = antecessor;
        tirar_no(no->filhos[i], antecessor, t);
    } else if (no->filhos[i + 1]->n >= t) {
        int sucessor = menor_chave(no->filhos[i + 1]);
        no->chaves[i] = sucessor;
        tirar_no(no->filhos[i + 1], sucessor, t);
    } else {
        juntar(no, i, t);
        tirar_no(no->filhos[i], chave, t);
    }
}

void tirar_no(No *no, int chave, int t) {
    int i = 0;
    while (i < no->n && no->chaves[i] < chave)
        i++;

    if (i < no->n && no->chaves[i] == chave) {
        if (no->folha) {
            for (int j = i + 1; j < no->n; j++)
                no->chaves[j - 1] = no->chaves[j];
            no->n--;
        } else {
            tirar_de_no_interno(no, i, t);
        }
        return;
    }

    if (no->folha)
        return;

    int era_o_ultimo = (i == no->n);

    if (no->filhos[i]->n < t)
        garantir_minimo(no, i, t);

    if (era_o_ultimo && i > no->n)
        tirar_no(no->filhos[i - 1], chave, t);
    else
        tirar_no(no->filhos[i], chave, t);
}

int tirar(ArvoreB *arv, int chave) {
    int pos;
    if (buscar(arv->raiz, chave, &pos) == NULL)
        return 0;

    tirar_no(arv->raiz, chave, arv->t);

    if (arv->raiz->n == 0 && !arv->raiz->folha) {
        No *antiga = arv->raiz;
        arv->raiz = antiga->filhos[0];
        free(antiga->chaves);
        free(antiga->filhos);
        free(antiga);
    }
    return 1;
}

int altura(ArvoreB *arv) {
    int h = 0;
    No *no = arv->raiz;
    while (!no->folha) {
        h++;
        no = no->filhos[0];
    }
    return h;
}

void imprimir_nivel(No *no, int nivel) {
    if (nivel == 0) {
        printf("[");
        for (int i = 0; i < no->n; i++) {
            if (i > 0)
                printf(" ");
            printf("%d", no->chaves[i]);
        }
        printf("] ");
        return;
    }
    if (no->folha)
        return;
    for (int i = 0; i <= no->n; i++)
        imprimir_nivel(no->filhos[i], nivel - 1);
}

void imprimir(ArvoreB *arv) {
    if (arv->raiz->n == 0) {
        printf("arvore vazia\n");
        return;
    }
    int h = altura(arv);
    for (int nivel = 0; nivel <= h; nivel++) {
        printf("Nivel %d: ", nivel);
        imprimir_nivel(arv->raiz, nivel);
        printf("\n");
    }
}

void demonstracao(ArvoreB *arv) {
    int valores[] = {10, 20, 5, 6, 12, 30, 7, 17, 3, 25, 40, 1};
    int qtd = sizeof(valores) / sizeof(valores[0]);

    printf("\nInserindo valores (t = %d)\n", arv->t);
    for (int i = 0; i < qtd; i++) {
        printf("\nInserindo %d:\n", valores[i]);
        inserir(arv, valores[i]);
        imprimir(arv);
    }

    int remocoes[] = {6, 13, 7, 20};
    printf("\nRemovendo valores\n");
    for (int i = 0; i < 4; i++) {
        printf("\nRemovendo %d: ", remocoes[i]);
        if (tirar(arv, remocoes[i]))
            printf("ok\n");
        else
            printf("nao encontrado\n");
        imprimir(arv);
    }
}

int main(void) {
    int t;
    printf("Grau minimo t (>= 2): ");
    if (scanf("%d", &t) != 1 || t < 2)
        t = 2;

    ArvoreB *arv = criar_arvore(t);
    int op, valor, pos;

    do {
        printf("\n1 - Inserir\n2 - Remover\n3 - Buscar\n4 - Imprimir\n5 - Demonstracao\n0 - Sair\n> ");
        if (scanf("%d", &op) != 1)
            break;

        if (op == 1) {
            printf("Valor: ");
            scanf("%d", &valor);
            if (!inserir(arv, valor))
                printf("Esse valor ja existe\n");
            imprimir(arv);
        } else if (op == 2) {
            printf("Valor: ");
            scanf("%d", &valor);
            if (!tirar(arv, valor))
                printf("Valor nao encontrado\n");
            imprimir(arv);
        } else if (op == 3) {
            printf("Valor: ");
            scanf("%d", &valor);
            if (buscar(arv->raiz, valor, &pos) != NULL)
                printf("Encontrado\n");
            else
                printf("Nao encontrado\n");
        } else if (op == 4) {
            imprimir(arv);
        } else if (op == 5) {
            demonstracao(arv);
        }
    } while (op != 0);

    liberar_arvore(arv);
    return 0;
}
