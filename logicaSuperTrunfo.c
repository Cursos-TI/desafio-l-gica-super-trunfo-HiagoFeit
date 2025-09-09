#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
  Super Trunfo – Países
  Níveis atendidos:
    - Novato: cadastro interativo, comparação básica, exibição de resultados.
    - Aventureiro: menu interativo com switch, comparação aninhada, regra especial.
    - Mestre: escolha de DOIS atributos, lógica encadeada + operadores ternários, menus dinâmicos.

  Como compilar:
    gcc -Wall -Wextra -O2 super_trunfo_paises.c -o super_trunfo && ./super_trunfo
*/

/* ============================
   Modelos e Constantes
   ============================ */

typedef struct {
    char codigo[8];            // Ex: "A01"
    char nomePais[64];         // Ex: "Brasil"
    long long populacao;       // habitantes
    double area;               // km²
    double pib;                // unidade livre (ex.: bilhões)
    int  pontosTuristicos;     // quantidade
    // Derivados
    double densidade;          // populacao / area
    double pibPerCapita;       // pib / populacao
} Carta;

/* IDs dos atributos apresentados ao usuário */
enum {
    ATTR_NOME = 1,            // exibição apenas
    ATTR_POPULACAO,
    ATTR_AREA,
    ATTR_PIB,
    ATTR_PONTOS,
    ATTR_DENSIDADE
};

/* ============================
   Utilidades de Entrada
   ============================ */

static void finalizar_por_eof(void) {
    puts("\nEntrada encerrada. Até mais!");
    exit(0);
}

static void ler_string(const char *prompt, char *dest, size_t tam) {
    printf("%s", prompt);
    if (!fgets(dest, (int)tam, stdin)) finalizar_por_eof();
    size_t n = strlen(dest);
    if (n && dest[n-1] == '\n') dest[n-1] = '\0';
}

static int ler_int(const char *prompt) {
    char buf[128]; char *end;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) finalizar_por_eof();
        long v = strtol(buf, &end, 10);
        while (*end && isspace((unsigned char)*end)) end++;
        if (end != buf && (*end == '\0' || *end == '\n')) return (int)v;
        puts("⚠️  Valor inválido. Tente novamente.");
    }
}

static long long ler_int64(const char *prompt) {
    char buf[128]; char *end;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) finalizar_por_eof();
        long long v = strtoll(buf, &end, 10);
        while (*end && isspace((unsigned char)*end)) end++;
        if (end != buf && (*end == '\0' || *end == '\n')) return v;
        puts("⚠️  Valor inválido. Tente novamente.");
    }
}

static double ler_double(const char *prompt) {
    char buf[128]; char *end;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) finalizar_por_eof();
        double v = strtod(buf, &end);
        while (*end && isspace((unsigned char)*end)) end++;
        if (end != buf && (*end == '\0' || *end == '\n')) return v;
        puts("⚠️  Valor inválido. Tente novamente.");
    }
}

/* ============================
   Cálculos, Impressão, Atributos
   ============================ */

static void calcular_derivados(Carta *c) {
    c->densidade   = (c->area > 0.0) ? ( (double)c->populacao / c->area ) : 0.0;
    c->pibPerCapita= (c->populacao > 0) ? ( c->pib / (double)c->populacao ) : 0.0;
}

static const char* nome_atributo(int attr) {
    switch (attr) {
        case ATTR_NOME:       return "Nome do país (exibição)";
        case ATTR_POPULACAO:  return "População (maior vence)";
        case ATTR_AREA:       return "Área (maior vence)";
        case ATTR_PIB:        return "PIB (maior vence)";
        case ATTR_PONTOS:     return "Pontos turísticos (maior vence)";
        case ATTR_DENSIDADE:  return "Densidade demográfica (MENOR vence)";
        default:              return "Atributo desconhecido";
    }
}

/* Para formatação na exibição do valor */
static void imprimir_valor_atributo(const Carta *c, int attr) {
    switch (attr) {
        case ATTR_NOME:       printf("%s\n", c->nomePais); break;
        case ATTR_POPULACAO:  printf("%lld hab\n", c->populacao); break;
        case ATTR_AREA:       printf("%.2f km²\n", c->area); break;
        case ATTR_PIB:        printf("%.2f\n", c->pib); break;
        case ATTR_PONTOS:     printf("%d ponto(s)\n", c->pontosTuristicos); break;
        case ATTR_DENSIDADE:  printf("%.2f hab/km²\n", c->densidade); break;
        default:              printf("(atributo inválido)\n");
    }
}

static void imprimir_carta(const Carta *c) {
    printf("Carta %s - %s\n", c->codigo, c->nomePais);
    printf("  População: %lld hab\n", c->populacao);
    printf("  Área: %.2f km²\n", c->area);
    printf("  PIB: %.2f\n", c->pib);
    printf("  Pontos turísticos: %d\n", c->pontosTuristicos);
    printf("  Densidade demográfica: %.2f hab/km²\n", c->densidade);
    printf("  PIB per capita: %.6f\n", c->pibPerCapita);
}

/* Valor numérico para comparação (nome não é comparável) */
static double valor_attr(const Carta *c, int attr) {
    switch (attr) {
        case ATTR_POPULACAO:  return (double)c->populacao;
        case ATTR_AREA:       return c->area;
        case ATTR_PIB:        return c->pib;
        case ATTR_PONTOS:     return (double)c->pontosTuristicos;
        case ATTR_DENSIDADE:  return c->densidade;
        default:              return 0.0;
    }
}

/* “Maior vence?” — densidade é exceção (menor vence) */
static int maior_vence(int attr) {
    return attr != ATTR_DENSIDADE; /* 1 para maior-vence, 0 para menor-vence */
}

/* ============================
   Cadastro
   ============================ */

static void cadastrar_carta(Carta *c, const char *rotulo) {
    printf("\n=== Cadastro da %s ===\n", rotulo);
    ler_string("Código da carta: ", c->codigo, sizeof(c->codigo));
    ler_string("Nome do país: ", c->nomePais, sizeof(c->nomePais));
    c->populacao        = ler_int64("População (habitantes): ");
    c->area             = ler_double("Área (km²): ");
    c->pib              = ler_double("PIB (na unidade que preferir): ");
    c->pontosTuristicos = ler_int("Número de pontos turísticos: ");
    calcular_derivados(c);
}

/* ============================
   Comparações
   ============================ */

/* Retorna: 1 se c1 vence; 2 se c2 vence; 0 empate; -1 atributo não comparável */
static int comparar_um_atributo(const Carta *c1, const Carta *c2, int attr) {
    if (attr == ATTR_NOME) return -1; // não comparável

    const double v1 = valor_attr(c1, attr);
    const double v2 = valor_attr(c2, attr);

    /* Decisão aninhada + regra especial */
    if (maior_vence(attr)) {
        if (v1 > v2) return 1;
        else if (v2 > v1) return 2;
        else {
            /* Desempate ilustrativo: maior PIB per capita vence */
            if (c1->pibPerCapita > c2->pibPerCapita) return 1;
            else if (c2->pibPerCapita > c1->pibPerCapita) return 2;
            else return 0;
        }
    } else { /* menor vence (densidade) */
        if (v1 < v2) return 1;
        else if (v2 < v1) return 2;
        else {
            /* Desempate ilustrativo para densidade: MAIOR área vence */
            if (c1->area > c2->area) return 1;
            else if (c2->area > c1->area) return 2;
            else return 0;
        }
    }
}

/*
  Comparação de DOIS atributos (nível Mestre).
  - Compara pelo atributo primário.
  - Se empatar, compara pelo secundário.
  - Se ainda empatar, aplica um encadeamento com OPERADORES TERNÁRIOS.
  Retorna 1, 2 ou 0 (empate).
*/
static int comparar_dois_atributos(const Carta *c1, const Carta *c2, int attr1, int attr2) {
    int r1 = comparar_um_atributo(c1, c2, attr1);
    if (r1 == 1 || r1 == 2) return r1;

    int r2 = comparar_um_atributo(c1, c2, attr2);
    if (r2 == 1 || r2 == 2) return r2;

    /* Desempate final (ternários): 
       - Primeiro, maior PIB per capita vence.
       - Se ainda empate, maior PIB total vence.
       - Se ainda empate, MAIOR população vence (apenas para desempate final).
    */
    return (c1->pibPerCapita > c2->pibPerCapita) ? 1
         : (c2->pibPerCapita > c1->pibPerCapita) ? 2
         : (c1->pib > c2->pib) ? 1
         : (c2->pib > c1->pib) ? 2
         : (c1->populacao > c2->populacao) ? 1
         : (c2->populacao > c1->populacao) ? 2
         : 0;
}

/* ============================
   Menus (dinâmicos) e Rodadas
   ============================ */

static void menu_atributos(void) {
    puts("Atributos disponíveis:");
    printf("  %d) %s\n", ATTR_NOME,      nome_atributo(ATTR_NOME));
    printf("  %d) %s\n", ATTR_POPULACAO, nome_atributo(ATTR_POPULACAO));
    printf("  %d) %s\n", ATTR_AREA,      nome_atributo(ATTR_AREA));
    printf("  %d) %s\n", ATTR_PIB,       nome_atributo(ATTR_PIB));
    printf("  %d) %s\n", ATTR_PONTOS,    nome_atributo(ATTR_PONTOS));
    printf("  %d) %s\n", ATTR_DENSIDADE, nome_atributo(ATTR_DENSIDADE));
}

static int escolher_atributo_unico(const char *prompt) {
    int a;
    while (1) {
        menu_atributos();
        a = ler_int(prompt);
        if (a >= ATTR_NOME && a <= ATTR_DENSIDADE) return a;
        puts("⚠️  Opção inexistente. Tente novamente.\n");
    }
}

static void escolher_dois_atributos(int *a1, int *a2) {
    while (1) {
        menu_atributos();
        *a1 = ler_int("Escolha o atributo PRIMÁRIO: ");
        *a2 = ler_int("Escolha o atributo SECUNDÁRIO (diferente do primário): ");
        if (*a1 < ATTR_NOME || *a1 > ATTR_DENSIDADE ||
            *a2 < ATTR_NOME || *a2 > ATTR_DENSIDADE) {
            puts("⚠️  Atributo inválido. Tente novamente.\n");
            continue;
        }
        if (*a1 == *a2) {
            puts("⚠️  Os atributos não podem ser iguais. Tente novamente.\n");
            continue;
        }
        break;
    }
}

static void exibir_cartas(const Carta *c1, const Carta *c2) {
    puts("\n=== Cartas cadastradas ===");
    imprimir_carta(c1);
    putchar('\n');
    imprimir_carta(c2);
    putchar('\n');
}

static void rodada_um_atributo(const Carta *c1, const Carta *c2) {
    puts("\n=== COMPARAÇÃO: UM ATRIBUTO ===");
    int attr = escolher_atributo_unico("Escolha o atributo: ");
    exibir_cartas(c1, c2);

    printf("=== Comparação (Atributo: %s) ===\n", nome_atributo(attr));

    printf("Carta 1 - %s: ", c1->nomePais); imprimir_valor_atributo(c1, attr);
    printf("Carta 2 - %s: ", c2->nomePais); imprimir_valor_atributo(c2, attr);

    int r = comparar_um_atributo(c1, c2, attr);
    if (r == -1) {
        printf("\nObservação: 'Nome do país' é apenas exibição — nenhuma carta vence.\n");
        return;
    }
    if (r == 1)      printf("\nResultado: Carta 1 (%s) venceu!\n", c1->nomePais);
    else if (r == 2) printf("\nResultado: Carta 2 (%s) venceu!\n", c2->nomePais);
    else             printf("\nResultado: Empate!\n");
}

static void rodada_dois_atributos(const Carta *c1, const Carta *c2) {
    puts("\n=== COMPARAÇÃO: DOIS ATRIBUTOS (MESTRE) ===");
    int a1, a2;
    escolher_dois_atributos(&a1, &a2);
    exibir_cartas(c1, c2);

    printf("=== Comparação (Primário: %s | Secundário: %s) ===\n",
           nome_atributo(a1), nome_atributo(a2));

    printf("Carta 1 - %s:\n", c1->nomePais);
    printf("  Primário:   "); imprimir_valor_atributo(c1, a1);
    printf("  Secundário: "); imprimir_valor_atributo(c1, a2);

    printf("Carta 2 - %s:\n", c2->nomePais);
    printf("  Primário:   "); imprimir_valor_atributo(c2, a1);
    printf("  Secundário: "); imprimir_valor_atributo(c2, a2);

    /* Se qualquer atributo escolhido for 'nome', tratamos como não comparável no passo daquele atributo.
       A função comparar_dois_atributos já resolve empates com ternários ao final. */
    int r = comparar_dois_atributos(c1, c2, a1, a2);

    if (r == 1)      printf("\nResultado: Carta 1 (%s) venceu!\n", c1->nomePais);
    else if (r == 2) printf("\nResultado: Carta 2 (%s) venceu!\n", c2->nomePais);
    else             printf("\nResultado: Empate!\n");
}

/* ============================
   Menu Principal
   ============================ */

static void menu_principal(void) {
    puts("===== SUPER TRUNFO — PAÍSES =====");
    puts("1) Cadastrar/Editar cartas");
    puts("2) Exibir cartas");
    puts("3) Comparar por UM atributo");
    puts("4) Comparar por DOIS atributos (Mestre)");
    puts("0) Sair");
}

int main(void) {
    Carta c1 = {0}, c2 = {0};
    int cadastradas = 0;

    puts("Bem-vindo ao Super Trunfo — Países!");
    puts("Cadastre duas cartas para começar.");

    while (1) {
        putchar('\n');
        menu_principal();
        int op = ler_int("Escolha uma opção: ");
        switch (op) {
            case 0:
                puts("Saindo... Obrigado por jogar!");
                return 0;

            case 1: { /* Cadastro / edição */
                cadastrar_carta(&c1, "Carta 1");
                cadastrar_carta(&c2, "Carta 2");
                cadastradas = 1;
            } break;

            case 2: { /* Exibir */
                if (!cadastradas) {
                    puts("⚠️  Nenhuma carta cadastrada. Cadastre primeiro (opção 1).");
                } else {
                    exibir_cartas(&c1, &c2);
                }
            } break;

            case 3: { /* Um atributo */
                if (!cadastradas) {
                    puts("⚠️  Nenhuma carta cadastrada. Cadastre primeiro (opção 1).");
                } else {
                    rodada_um_atributo(&c1, &c2);
                }
            } break;

            case 4: { /* Dois atributos (Mestre) */
                if (!cadastradas) {
                    puts("⚠️  Nenhuma carta cadastrada. Cadastre primeiro (opção 1).");
                } else {
                    rodada_dois_atributos(&c1, &c2);
                }
            } break;

            default:
                puts("⚠️  Opção inexistente. Tente novamente.");
        }
    }
    return 0;
}  