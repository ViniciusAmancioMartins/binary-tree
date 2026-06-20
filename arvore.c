#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#include "print-ab.h"

// nó da arvore binaria de busca
struct s_no
{
    int32_t chave : 30;
    int32_t reservado : 2;
    struct s_no *esq;
    struct s_no *dir;
};

// formato que vai pro arquivo binario
struct s_arq_no
{
    int32_t chave : 30;
    uint32_t esq : 1;
    uint32_t dir : 1;
};

struct s_no *insere(struct s_no *raiz, int32_t v)
{
    if (raiz == NULL)
    {
        struct s_no *novo = malloc(sizeof(struct s_no));
        novo->chave = v;
        novo->reservado = 0;
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }

    if (v < raiz->chave)
        raiz->esq = insere(raiz->esq, v);
    else if (v > raiz->chave)
        raiz->dir = insere(raiz->dir, v);
    // se igual nao insere (sem duplicata)

    return raiz;
}

// vai descendo pela esquerda ate acabar
struct s_no *acha_min(struct s_no *n)
{
    if (n == NULL)
        return NULL;
    while (n->esq)
        n = n->esq;
    return n;
}

struct s_no *acha_max(struct s_no *n)
{
    if (n == NULL)
        return NULL;
    while (n->dir)
        n = n->dir;
    return n;
}

struct s_no *remover(struct s_no *raiz, int32_t v)
{
    if (raiz == NULL)
        return NULL;

    if (v < raiz->chave)
    {
        raiz->esq = remover(raiz->esq, v);
    }
    else if (v > raiz->chave)
    {
        raiz->dir = remover(raiz->dir, v);
    }
    else
    {
        // achou o no pra remover
        if (raiz->esq == NULL && raiz->dir == NULL)
        {
            // folha, sem filhos
            free(raiz);
            return NULL;
        }
        else if (raiz->esq == NULL)
        {
            // so filho direito
            struct s_no *temp = raiz->dir;
            free(raiz);
            return temp;
        }
        else if (raiz->dir == NULL)
        {
            // so filho esquerdo
            struct s_no *temp = raiz->esq;
            free(raiz);
            return temp;
        }
        else
        {
            // dois filhos: substitui pelo menor da sub-arvore direita
            struct s_no *subs = acha_min(raiz->dir);
            raiz->chave = subs->chave;
            raiz->dir = remover(raiz->dir, subs->chave);
        }
    }
    return raiz;
}

struct s_no *busca(struct s_no *raiz, int32_t v)
{
    if (raiz == NULL || raiz->chave == v)
        return raiz;
    if (v < raiz->chave)
        return busca(raiz->esq, v);
    return busca(raiz->dir, v);
}

// proximo maior que val
struct s_no *sucessor(struct s_no *raiz, int32_t val)
{
    struct s_no *suc = NULL;
    struct s_no *atual = raiz;

    while (atual)
    {
        if (atual->chave > val)
        {
            suc = atual;
            atual = atual->esq;
        }
        else if (atual->chave < val)
        {
            atual = atual->dir;
        }
        else
        {
            // achou o no: sucessor e o menor da direita
            if (atual->dir)
                suc = acha_min(atual->dir);
            break;
        }
    }
    return suc;
}

struct s_no *predecessor(struct s_no *raiz, int32_t val)
{
    struct s_no *pred = NULL;
    struct s_no *atual = raiz;

    while (atual)
    {
        if (atual->chave < val)
        {
            pred = atual;
            atual = atual->dir;
        }
        else if (atual->chave > val)
        {
            atual = atual->esq;
        }
        else
        {
            if (atual->esq)
                pred = acha_max(atual->esq);
            break;
        }
    }
    return pred;
}

void pre_ordem(struct s_no *r)
{
    if (r == NULL)
        return;
    printf("%d ", r->chave);
    pre_ordem(r->esq);
    pre_ordem(r->dir);
}

void em_ordem(struct s_no *r)
{
    if (r == NULL)
        return;
    em_ordem(r->esq);
    printf("%d ", r->chave);
    em_ordem(r->dir);
}

void pos_ordem(struct s_no *r)
{
    if (r == NULL)
        return;
    pos_ordem(r->esq);
    pos_ordem(r->dir);
    printf("%d ", r->chave);
}

void libera(struct s_no *r)
{
    if (r == NULL)
        return;
    libera(r->esq);
    libera(r->dir);
    free(r);
}

// serializa em pre-ordem pra facilitar a leitura depois
void salvar(struct s_no *r, FILE *f)
{
    if (r == NULL)
        return;

    struct s_arq_no buf;
    buf.chave = r->chave;
    buf.esq = r->esq ? 1 : 0;
    buf.dir = r->dir ? 1 : 0;
    fwrite(&buf, sizeof(buf), 1, f);

    salvar(r->esq, f);
    salvar(r->dir, f);
}

struct s_no *carregar(FILE *f)
{
    struct s_arq_no buf;
    if (fread(&buf, sizeof(buf), 1, f) != 1)
        return NULL;

    struct s_no *n = malloc(sizeof(struct s_no));
    n->chave = buf.chave;
    n->reservado = 0;
    n->esq = NULL;
    n->dir = NULL;

    if (buf.esq)
        n->esq = carregar(f);
    if (buf.dir)
        n->dir = carregar(f);

    return n;
}

int main()
{
    struct s_no *raiz = NULL;
    int op;
    int32_t val;

    do
    {
        printf("\n[1] inserir  [2] remover  [3] buscar\n");
        printf("[4] min/max  [5] suc/pred [6] ordens\n");
        printf("[7] salvar   [8] carregar [0] sair\n");
        printf("> ");
        scanf("%d", &op);

        switch (op)
        {
        case 1:
            printf("valor: ");
            scanf("%d", &val);
            raiz = insere(raiz, val);
            break;

        case 2:
            printf("remover: ");
            scanf("%d", &val);
            raiz = remover(raiz, val);
            break;

        case 3:
            printf("buscar: ");
            scanf("%d", &val);
            if (busca(raiz, val))
                printf("encontrado: %d\n", val);
            else
                printf("nao encontrado\n");
            break;

        case 4:
            if (raiz == NULL)
            {
                printf("arvore vazia\n");
            }
            else
            {
                printf("min: %d  max: %d\n",
                       acha_min(raiz)->chave,
                       acha_max(raiz)->chave);
            }
            break;

        case 5:
        {
            printf("val: ");
            scanf("%d", &val);
            struct s_no *suc = sucessor(raiz, val);
            struct s_no *pred = predecessor(raiz, val);
            printf("suc:  %s\n", suc ? "existe" : "nenhum");
            if (suc)
                printf("  -> %d\n", suc->chave);
            printf("pred: %s\n", pred ? "existe" : "nenhum");
            if (pred)
                printf("  -> %d\n", pred->chave);
            break;
        }

        case 6:
            printf("pre : ");
            pre_ordem(raiz);
            printf("\n");
            printf("em  : ");
            em_ordem(raiz);
            printf("\n");
            printf("pos : ");
            pos_ordem(raiz);
            printf("\n");
            break;

        case 7:
        {
            FILE *f = fopen("arvore.bin", "wb");
            if (f)
            {
                salvar(raiz, f);
                fclose(f);
                printf("salvo\n");
            }
            else
            {
                printf("erro ao abrir arquivo\n");
            }
            break;
        }

        case 8:
        {
            FILE *f = fopen("arvore.bin", "rb");
            if (f)
            {
                libera(raiz);
                raiz = carregar(f);
                fclose(f);
                printf("carregado\n");
            }
            else
            {
                printf("arquivo nao encontrado\n");
            }
            break;
        }

        case 0:
            libera(raiz);
            break;

        default:
            printf("?\n");
        }

        if (op != 0)
        {
            
            imprime_arvore(raiz);
        }

    } while (op != 0);

    return 0;
}