#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE_VALUES    256
#define MAX_TREE_BYTES 8191
#define MAX_PATH_DEPTH 300
#define SERIAL_BUFFER  1100

typedef int (*CompareFn)(const void *a, const void *b);

/**
 * TAD Heap: fila de prioridade (heap minimo) de ponteiros genericos.
 * Sempre entrega o menor elemento segundo a funcao de comparacao.
 */
typedef struct {
    void   **items;
    size_t   size;
    size_t   capacity;
    CompareFn compare;
} Heap;

/**
 * Cria um heap vazio.
 * @param capacity capacidade inicial (cresce sozinha se necessario)
 * @param compare  funcao de comparacao dos elementos
 * @return novo heap, ou NULL se faltar memoria
 */
static Heap *heap_create(size_t capacity, CompareFn compare) {
    Heap *h = malloc(sizeof(Heap));
    if (!h) return NULL;
    h->items = malloc(capacity * sizeof(void *));
    if (!h->items) { free(h); return NULL; }
    h->size = 0;
    h->capacity = capacity;
    h->compare = compare;
    return h;
}

/**
 * Libera o heap. Os elementos apontados nao sao liberados.
 * @param h heap a ser liberado (pode ser NULL)
 */
static void heap_destroy(Heap *h) {
    if (!h) return;
    free(h->items);
    free(h);
}

/**
 * Insere um elemento no heap.
 * @param h    heap onde inserir
 * @param item ponteiro generico do elemento
 * @return 1 em caso de sucesso, 0 se faltar memoria
 */
static int heap_push(Heap *h, void *item) {
    if (h->size == h->capacity) {
        size_t novaCap = h->capacity * 2;
        void **novo = realloc(h->items, novaCap * sizeof(void *));
        if (!novo) return 0;
        h->items = novo;
        h->capacity = novaCap;
    }

    size_t i = h->size++;
    while (i > 0) {
        size_t pai = (i - 1) / 2;
        if (h->compare(item, h->items[pai]) >= 0) break;
        h->items[i] = h->items[pai];
        i = pai;
    }
    h->items[i] = item;
    return 1;
}

/**
 * Remove e devolve o menor elemento do heap.
 * @param h heap de onde remover
 * @return o elemento removido, ou NULL se o heap estiver vazio
 */
static void *heap_pop(Heap *h) {
    if (h->size == 0) return NULL;

    void *menor = h->items[0];
    void *ultimo = h->items[--h->size];

    size_t i = 0;
    while (1) {
        size_t filho = 2 * i + 1;
        if (filho >= h->size) break;
        if (filho + 1 < h->size && h->compare(h->items[filho + 1], h->items[filho]) < 0)
            filho++;
        if (h->compare(ultimo, h->items[filho]) <= 0) break;
        h->items[i] = h->items[filho];
        i = filho;
    }
    if (h->size > 0) h->items[i] = ultimo;
    return menor;
}

/**
 * TAD Arvore binaria: no que guarda um dado generico e aponta para
 * ate dois filhos.
 */
typedef struct TreeNode {
    void *data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/**
 * Cria um no de arvore.
 * @param data  dado generico guardado no no
 * @param left  filho esquerdo (NULL se nao houver)
 * @param right filho direito (NULL se nao houver)
 * @return novo no, ou NULL se faltar memoria
 */
static TreeNode *tree_create(void *data, TreeNode *left, TreeNode *right) {
    TreeNode *n = malloc(sizeof(TreeNode));
    if (!n) return NULL;
    n->data = data;
    n->left = left;
    n->right = right;
    return n;
}

/**
 * Informa se o no e folha.
 * @param n no a ser verificado
 * @return 1 se o no nao tem filhos, 0 caso contrario
 */
static int tree_is_leaf(const TreeNode *n) {
    return n->left == NULL && n->right == NULL;
}

/**
 * Libera a arvore inteira (em pos-ordem).
 * @param n        raiz da arvore (pode ser NULL)
 * @param freeData funcao que libera o dado de cada no (pode ser NULL)
 */
static void tree_destroy(TreeNode *n, void (*freeData)(void *)) {
    if (!n) return;
    tree_destroy(n->left, freeData);
    tree_destroy(n->right, freeData);
    if (freeData) freeData(n->data);
    free(n);
}

/**
 * TAD Tabela: associa cada valor de byte (0 a 255) a um ponteiro generico.
 */
typedef struct {
    void *entries[BYTE_VALUES];
} Table;

/**
 * Cria uma tabela vazia (todas as posicoes NULL).
 * @return nova tabela, ou NULL se faltar memoria
 */
static Table *table_create(void) {
    return calloc(1, sizeof(Table));
}

/**
 * Guarda um valor na posicao de um byte.
 * @param t     tabela
 * @param key   byte usado como indice
 * @param value ponteiro generico a ser guardado
 */
static void table_put(Table *t, unsigned char key, void *value) {
    t->entries[key] = value;
}

/**
 * Le o valor guardado na posicao de um byte.
 * @param t   tabela
 * @param key byte usado como indice
 * @return o valor guardado, ou NULL se a posicao estiver vazia
 */
static void *table_get(const Table *t, unsigned char key) {
    return t->entries[key];
}

/**
 * Libera a tabela.
 * @param t         tabela a ser liberada (pode ser NULL)
 * @param freeValue funcao que libera cada valor (pode ser NULL)
 */
static void table_destroy(Table *t, void (*freeValue)(void *)) {
    if (!t) return;
    if (freeValue) {
        for (int i = 0; i < BYTE_VALUES; i++)
            if (t->entries[i]) freeValue(t->entries[i]);
    }
    free(t);
}

typedef struct {
    unsigned char byte;
    unsigned long long freq;
} HuffItem;

typedef struct {
    unsigned char *bits;
    size_t length;
} Code;

static void free_item(void *p) { free(p); }

static void free_code(void *p) {
    Code *c = p;
    free(c->bits);
    free(c);
}

static int compare_nodes(const void *a, const void *b) {
    const HuffItem *x = ((const TreeNode *)a)->data;
    const HuffItem *y = ((const TreeNode *)b)->data;
    if (x->freq < y->freq) return -1;
    if (x->freq > y->freq) return 1;
    return 0;
}

static TreeNode *new_huff_node(unsigned char byte, unsigned long long freq,
                               TreeNode *left, TreeNode *right) {
    HuffItem *item = malloc(sizeof(HuffItem));
    if (!item) return NULL;
    item->byte = byte;
    item->freq = freq;

    TreeNode *n = tree_create(item, left, right);
    if (!n) free(item);
    return n;
}

static TreeNode *build_tree(const unsigned long long freq[BYTE_VALUES]) {
    Heap *heap = heap_create(BYTE_VALUES + 1, compare_nodes);
    if (!heap) return NULL;

    int distintos = 0;
    int ultimo = 0;
    for (int b = 0; b < BYTE_VALUES; b++) {
        if (freq[b] > 0) {
            TreeNode *folha = new_huff_node((unsigned char)b, freq[b], NULL, NULL);
            if (!folha || !heap_push(heap, folha)) { heap_destroy(heap); return NULL; }
            distintos++;
            ultimo = b;
        }
    }

    if (distintos == 1) {
        TreeNode *falso = new_huff_node((unsigned char)(ultimo + 1), 0, NULL, NULL);
        if (!falso || !heap_push(heap, falso)) { heap_destroy(heap); return NULL; }
    }

    while (heap->size > 1) {
        TreeNode *a = heap_pop(heap);
        TreeNode *b = heap_pop(heap);
        unsigned long long soma = ((HuffItem *)a->data)->freq + ((HuffItem *)b->data)->freq;
        TreeNode *pai = new_huff_node('*', soma, a, b);
        if (!pai || !heap_push(heap, pai)) { heap_destroy(heap); return NULL; }
    }

    TreeNode *raiz = heap_pop(heap);
    heap_destroy(heap);
    return raiz;
}

static int build_codes(const TreeNode *n, unsigned char *path, size_t depth, Table *table) {
    if (tree_is_leaf(n)) {
        Code *c = malloc(sizeof(Code));
        if (!c) return 0;
        c->length = depth;
        c->bits = malloc(depth ? depth : 1);
        if (!c->bits) { free(c); return 0; }
        memcpy(c->bits, path, depth);
        table_put(table, ((HuffItem *)n->data)->byte, c);
        return 1;
    }

    path[depth] = 0;
    if (!build_codes(n->left, path, depth + 1, table)) return 0;
    path[depth] = 1;
    return build_codes(n->right, path, depth + 1, table);
}

static void serialize_tree(const TreeNode *n, unsigned char *buf, size_t *len) {
    const HuffItem *item = n->data;

    if (tree_is_leaf(n)) {
        if (item->byte == '*' || item->byte == '\\')
            buf[(*len)++] = '\\';
        buf[(*len)++] = item->byte;
        return;
    }

    buf[(*len)++] = '*';
    serialize_tree(n->left, buf, len);
    serialize_tree(n->right, buf, len);
}

static TreeNode *deserialize_tree(const unsigned char *buf, size_t size, size_t *pos) {
    if (*pos >= size) return NULL;

    unsigned char c = buf[(*pos)++];

    if (c == '*') {
        TreeNode *esq = deserialize_tree(buf, size, pos);
        if (!esq) return NULL;
        TreeNode *dir = deserialize_tree(buf, size, pos);
        if (!dir) { tree_destroy(esq, free_item); return NULL; }

        TreeNode *pai = new_huff_node('*', 0, esq, dir);
        if (!pai) {
            tree_destroy(esq, free_item);
            tree_destroy(dir, free_item);
        }
        return pai;
    }

    if (c == '\\') {
        if (*pos >= size) return NULL;
        c = buf[(*pos)++];
    }
    return new_huff_node(c, 0, NULL, NULL);
}

typedef struct {
    FILE *out;
    unsigned char current;
    int count;
} BitWriter;

static void bitwriter_put(BitWriter *w, unsigned char bit) {
    w->current = (unsigned char)((w->current << 1) | bit);
    if (++w->count == 8) {
        fputc(w->current, w->out);
        w->current = 0;
        w->count = 0;
    }
}

static void bitwriter_flush(BitWriter *w) {
    if (w->count > 0) {
        w->current = (unsigned char)(w->current << (8 - w->count));
        fputc(w->current, w->out);
        w->current = 0;
        w->count = 0;
    }
}

static int compress_file(const char *inPath, const char *outPath) {
    FILE *in = fopen(inPath, "rb");
    if (!in) { fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", inPath); return 1; }

    unsigned long long freq[BYTE_VALUES] = {0};
    unsigned long long total = 0;
    unsigned char buffer[65536];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < n; i++) freq[buffer[i]]++;
        total += n;
    }

    FILE *out = fopen(outPath, "wb");
    if (!out) {
        fprintf(stderr, "Erro: nao foi possivel criar '%s'.\n", outPath);
        fclose(in);
        return 1;
    }

    if (total == 0) {
        fputc(0, out);
        fputc(0, out);
        fclose(in);
        fclose(out);
        return 0;
    }

    int status = 1;
    TreeNode *raiz = build_tree(freq);
    Table *codes = table_create();
    if (!raiz || !codes) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        goto fim;
    }

    unsigned char path[MAX_PATH_DEPTH];
    if (!build_codes(raiz, path, 0, codes)) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        goto fim;
    }

    unsigned char serial[SERIAL_BUFFER];
    size_t treeSize = 0;
    serialize_tree(raiz, serial, &treeSize);
    if (treeSize > MAX_TREE_BYTES) {
        fprintf(stderr, "Erro: arvore grande demais para o cabecalho.\n");
        goto fim;
    }

    unsigned long long bits = 0;
    for (int b = 0; b < BYTE_VALUES; b++) {
        if (freq[b] > 0)
            bits += freq[b] * ((Code *)table_get(codes, (unsigned char)b))->length;
    }
    unsigned int trash = (unsigned int)((8 - (bits % 8)) % 8);

    fputc((int)((trash << 5) | (treeSize >> 8)), out);
    fputc((int)(treeSize & 0xFF), out);
    fwrite(serial, 1, treeSize, out);

    rewind(in);
    BitWriter writer = { out, 0, 0 };

    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < n; i++) {
            const Code *c = table_get(codes, buffer[i]);
            for (size_t k = 0; k < c->length; k++)
                bitwriter_put(&writer, c->bits[k]);
        }
    }
    bitwriter_flush(&writer);

    status = ferror(out) ? 1 : 0;
    if (status) fprintf(stderr, "Erro ao gravar o arquivo de saida.\n");

fim:
    tree_destroy(raiz, free_item);
    table_destroy(codes, free_code);
    fclose(in);
    fclose(out);
    return status;
}

static int decompress_file(const char *inPath, const char *outPath) {
    FILE *in = fopen(inPath, "rb");
    if (!in) { fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", inPath); return 1; }

    int b0 = fgetc(in);
    int b1 = fgetc(in);
    if (b0 == EOF || b1 == EOF) {
        fprintf(stderr, "Erro: arquivo .huff invalido (cabecalho incompleto).\n");
        fclose(in);
        return 1;
    }

    unsigned int trash = (unsigned int)b0 >> 5;
    size_t treeSize = (size_t)(((unsigned int)b0 & 0x1F) << 8) | (size_t)b1;

    FILE *out = fopen(outPath, "wb");
    if (!out) {
        fprintf(stderr, "Erro: nao foi possivel criar '%s'.\n", outPath);
        fclose(in);
        return 1;
    }

    if (treeSize == 0) {
        fclose(in);
        fclose(out);
        return 0;
    }

    int status = 1;
    TreeNode *raiz = NULL;
    unsigned char *serial = malloc(treeSize);
    if (!serial) { fprintf(stderr, "Erro: memoria insuficiente.\n"); goto fim; }

    if (fread(serial, 1, treeSize, in) != treeSize) {
        fprintf(stderr, "Erro: arquivo .huff invalido (arvore incompleta).\n");
        goto fim;
    }

    size_t pos = 0;
    raiz = deserialize_tree(serial, treeSize, &pos);
    if (!raiz || pos != treeSize || tree_is_leaf(raiz)) {
        fprintf(stderr, "Erro: arvore do arquivo .huff invalida.\n");
        goto fim;
    }

    const TreeNode *no = raiz;
    int atual = fgetc(in);

    while (atual != EOF) {
        int proximo = fgetc(in);
        int bitsUteis = (proximo == EOF) ? 8 - (int)trash : 8;

        for (int i = 0; i < bitsUteis; i++) {
            int bit = (atual >> (7 - i)) & 1;
            no = bit ? no->right : no->left;

            if (tree_is_leaf(no)) {
                fputc(((HuffItem *)no->data)->byte, out);
                no = raiz;
            }
        }
        atual = proximo;
    }

    status = ferror(out) ? 1 : 0;
    if (status) fprintf(stderr, "Erro ao gravar o arquivo de saida.\n");

fim:
    tree_destroy(raiz, free_item);
    free(serial);
    fclose(in);
    fclose(out);
    return status;
}

static void usage(const char *prog) {
    fprintf(stderr,
            "Uso:\n"
            "  %s c <entrada> [saida]\n"
            "  %s d <entrada.huff> [saida]\n",
            prog, prog);
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4 || (argv[1][0] != 'c' && argv[1][0] != 'd') || argv[1][1] != '\0') {
        usage(argv[0]);
        return 1;
    }

    const char *in = argv[2];
    char *out = NULL;
    int status;

    if (argc == 4) {
        out = malloc(strlen(argv[3]) + 1);
        if (!out) return 1;
        strcpy(out, argv[3]);
    } else if (argv[1][0] == 'c') {
        out = malloc(strlen(in) + 6);
        if (!out) return 1;
        strcpy(out, in);
        strcat(out, ".huff");
    } else {
        size_t len = strlen(in);
        out = malloc(len + 5);
        if (!out) return 1;
        strcpy(out, in);
        if (len > 5 && strcmp(in + len - 5, ".huff") == 0)
            out[len - 5] = '\0';
        else
            strcat(out, ".out");
    }

    status = (argv[1][0] == 'c') ? compress_file(in, out) : decompress_file(in, out);

    if (status == 0)
        printf("Concluido: %s\n", out);

    free(out);
    return status;
}
