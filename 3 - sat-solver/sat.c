#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct No{
   
    int var, valor;
    int *atrib;
    struct No *esq;
    struct No *dir;
} No;


int nvars, tam;
int *formula;
int *solucao;


No *criar_no(int var, int valor, int *pai){
  
    No *n = calloc(1, sizeof(No));
    n->var = var;
    n->valor = valor;
    n->atrib = calloc(nvars + 1, sizeof(int));
    if (pai) memcpy(n->atrib, pai, (nvars + 1) * sizeof(int));
    if(var > 0) n->atrib[var] = valor;
    return n;
}


void liberar (No *n){
    
    if(!n) return;
    liberar(n->esq);
    liberar(n->dir);
    free(n->atrib);
    free(n);
}


void ler_dimacs(const char *nome){
    
    FILE *f = fopen(nome, "r");
    if(!f){
        printf("arquivo nao encontrado\n");
        exit(1);
    }

    char c;
    while(fscanf(f, " %c", &c) == 1 && c == 'c'){
        fscanf(f, "%*[^\n]");
    }

    fscanf(f, " cnf %d %*d", &nvars);

    int cap = 1024;
    formula = malloc(cap * sizeof(int));
    tam = 0;

    int x;
    while (fscanf(f, "%d", &x) == 1){
        if(tam == cap){
            cap *= 2;
            formula = realloc(formula, cap * sizeof(int));
        }
        formula[tam++] = x;
    }
    fclose(f);
}


int valor_lit(int lit, int *a){
    return lit > 0 ? a[lit] : -a[-lit];
}

int propagar(int *a){

    int mudou = 1;
    while (mudou){
        mudou = 0;
        int i = 0;
        while (i < tam){
            int livres = 0, ultimo = 0, satisfeita = 0;
            for (; i < tam &&formula[i] != 0; i++){

                int v = valor_lit(formula[i], a);

                if (v == 1) satisfeita = 1;

                else if(v == 0){
                    livres++;
                    ultimo = formula[i];
                }
            }    
            i++;

            if (satisfeita) continue;
            if (livres == 0) return 0;
            if (livres == 1){
                a[abs(ultimo)] = ultimo > 0 ? 1 : -1;
                mudou = 1;
            }
        
        }
    }
    return 1;
}


int escolher_variavel(int *a){
    
    for (int v = 1; v <= nvars; v++){
        if (a[v] == 0) return v;
    }
    return 0;
}

int dpll(No *no){

    if(!propagar(no->atrib)) return 0;

    int v = escolher_variavel(no->atrib);
    if(v == 0){
        solucao = no->atrib;
        return 1;
    }

    no->esq = criar_no(v, 1, no->atrib);
    if (dpll(no->esq)) return 1;

    no->dir = criar_no(v, -1, no->atrib);
    return dpll(no->dir);
}


int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Uso: %s <arquivo.cnf>\n", argv[0]);
        return 1;
    }

    ler_dimacs(argv[1]);

    No *raiz = criar_no(0, 0, NULL);

    if(dpll(raiz)){
        printf("SAT!\n");
        for(int v = 1; v <= nvars; v++){
            int literal = (solucao[v] == -1) ? -v : v;
            printf("%d ", literal);
        }
        printf("0\n");
    }

    else{
        printf("UNSAT!\n");
    }

    liberar(raiz);
    free(formula);
    return 0;
}
