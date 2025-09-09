#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
  Super Trunfo — Países (Nível Mestre)
  Requisitos atendidos:
    - Escolha de DOIS atributos numéricos distintos via menus dinâmicos (switch).
    - Comparação individual por atributo (maior vence; densidade MENOR vence).
    - Soma dos atributos (valores brutos). Maior soma vence. Empate tratado.
    - Exibição clara: países, atributos, valores, somas e resultado.
    - Tratamento de entradas inválidas e mensagens claras.
    - Uso de operador ternário na lógica de decisão.

  Como compilar:
    gcc -Wall -Wextra -O2 super_trunfo_mestre.c -o super_trunfo && ./super_trunfo
*/

typedef struct {
    char codigo[8];
    char nomePais[64];
    long long populacao;  // habitantes
    double area;          // km²
    double pib;           // unidade livre (ex.: bilhões)
    int    pontosTuristicos;
    // derivados
    double densidade;     // populacao / area
    double pibPerCapita;  // pib / populacao
} Carta;

enum {
    ATTR_NOME = 1,  // não numérico para comparação
    ATTR_POPULACAO,
    ATTR_AREA,
    ATTR_PIB,
    ATTR_PONTOS,
    ATTR_DENSIDADE
};

/* ============================
   Utilidades de entrada
   ============================ */
static void finalizar_por_eof(void) {
    puts("\nEntrada encerrada. Até mais!");
    exit(0);
}

static int ler_int(const char *prompt) {
    char buf[128], *end;
    long v;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) finalizar_por_eof();
        v = strtol(buf, &end, 10);
        while (*end && isspace((unsigned char)*end)) end++;
        if (end != buf && (*end == '\0' || *end == '\n')) return (int)v;
        puts("⚠️  Valor inválido. Tente novamente.");
    }
}

static double ler_double(const char *prompt) {
    char buf[128], *end;
    double v;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) finalizar_por_eof();
        v = strtod(buf, &end);
        while (*end && isspace((unsigned char)*end)) end++;
        if (end != buf && (*end == '\0' || *end == '\n')) return v;
        puts("⚠️  Valor inválido. Tente novamente.");
    }
}

static void ler_string(const char *prompt, char *dest, size_t tam) {
    printf("%s", prompt);
    if (!fgets(dest, (int)tam, stdin)) finalizar_por_eof();
    size_t n = strlen(dest);
    if (n && dest[n-1] == '\n') dest[n-1] = '\0';
}

/* ============================
   Cálculos e helpers
   ============================ */
static void calcular_derivados(Carta *c) {
    c->densidade    = (c->area > 0.0) ? ( (double)c->populacao / c->area ) : 0.0;
    c->pibPerCapita = (c->populacao > 0) ? ( c->pib / (double)c->populacao ) : 0.0;
}

static const char* nome_atributo(int a) {
    switch (a) {
        case ATTR_NOME:       return "Nome do país (exibição)";
        case ATTR_POPULACAO:  return "População (maior vence)";
        case ATTR_AREA:       return "Área (maior vence)";
        case ATTR_PIB:        return "PIB (maior vence)";
        case ATTR_PONTOS:     return "Pontos turísticos (maior vence)";
        case ATTR_DENSIDADE:  return "Densidade demográfica (MENOR vence)";
        default:              return "Atributo desconhecido";
    }
}

static int atributo_e_numerico(int a) {
    return (a >= ATTR_POPULACAO && a <= ATTR_DENSIDADE);
}

static double valor_attr(const Carta *c, int a) {
    switch (a) {
        case ATTR_POPULACAO:  return (double)c->populacao;
        case ATTR_AREA:       return c->area;
        case ATTR_PIB:        return c->pib;
        case ATTR_PONTOS:     return (double)c->pontosTuristicos;
        case ATTR_DENSIDADE:  return c->densidade;
        default:              return 0.0; // nome não é numérico
    }
}

static int maior_vence(int a) {
    return a != ATTR_DENSIDADE; // densidade: menor vence
}

/* ============================
   Exibição
   ============================ */
static void imprimir_carta(const Carta *c) {
    printf("Carta %s — %s\n", c->codigo, c->nomePais);
    printf("  População: %lld hab\n", c->populacao);
    printf("  Área: %.2f km²\n", c->area);
    printf("  PIB: %.2f\n", c->pib);
    printf("  Pontos turísticos: %d\n", c->pontosTuristicos);
    printf("  Densidade demográfica: %.2f hab/km²\n", c->densidade);
    printf("  PIB per capita: %.6f\n", c->pibPerCapita);
}

static void imprimir_valor_atributo(const Carta *c, int a) {
    switch (a) {
        case ATTR_POPULACAO:  printf("%.0f hab",  valor_attr(c,a)); break;
        case ATTR_AREA:       printf("%.2f km²",  valor_attr(c,a)); break;
        case ATTR_PIB:        printf("%.2f",      valor_attr(c,a)); break;
        case ATTR_PONTOS:     printf("%.0f",      valor_attr(c,a)); break;
        case ATTR_DENSIDADE:  printf("%.2f hab/km²", valor_attr(c,a)); break;
        default:              printf("(n/d)"); break;
    }
}

/* Resultado individual de um atributo:
   Retorna 1 (c1), 2 (c2) ou 0 (empate).
   Usa operador ternário no resultado final.
*/
static int vencedor_por_atributo(const Carta *c1, const Carta *c2, int a) {
    double v1 = valor_attr(c1, a);
    double v2 = valor_attr(c2, a);

    if (maior_vence(a)) {
        return (v1 > v2) ? 1 : (v2 > v1) ? 2 : 0;
    } else {
        return (v1 < v2) ? 1 : (v2 < v1) ? 2 : 0;
    }
}

/* ============================
   Menus dinâmicos (switch)
   ============================ */
static void menu_atributos_primario(void) {
    puts("=== Escolha o ATRIBUTO PRIMÁRIO ===");
    puts(" 2) População (maior vence)");
    puts(" 3) Área (maior vence)");
    puts(" 4) PIB (maior vence)");
    puts(" 5) Pontos turísticos (maior vence)");
    puts(" 6) Densidade demográfica (MENOR vence)");
}

static void menu_atributos_secundario_excluindo(int exclui) {
    puts("=== Escolha o ATRIBUTO SECUNDÁRIO ===");
    if (exclui != ATTR_POPULACAO) puts(" 2) População (maior vence)");
    if (exclui != ATTR_AREA)      puts(" 3) Área (maior vence)");
    if (exclui != ATTR_PIB)       puts(" 4) PIB (maior vence)");
    if (exclui != ATTR_PONTOS)    puts(" 5) Pontos turísticos (maior vence)");
    if (exclui != ATTR_DENSIDADE) puts(" 6) Densidade demográfica (MENOR vence)");
}

static int ler_opcao_primaria(void) {
    int a;
    while (1) {
        menu_atributos_primario();
        a = ler_int("Opção: ");
        switch (a) {
            case ATTR_POPULACAO:
            case ATTR_AREA:
            case ATTR_PIB:
            case ATTR_PONTOS:
            case ATTR_DENSIDADE:
                return a;
            default:
                puts("⚠️  Opção inválida. Tente novamente.\n");
        }
    }
}

static int ler_opcao_secundaria(int primario) {
    int a;
    while (1) {
        menu_atributos_secundario_excluindo(primario);
        a = ler_int("Opção: ");
        switch (a) {
            case ATTR_POPULACAO:
            case ATTR_AREA:
            case ATTR_PIB:
            case ATTR_PONTOS:
            case ATTR_DENSIDADE:
                if (a == primario) {
                    puts("⚠️  O secundário deve ser diferente do primário.\n");
                    break;
                }
                return a;
            default:
                puts("⚠️  Opção inválida. Tente novamente.\n");
        }
    }
}

/* ============================
   Rodada MESTRE (2 atributos)
   ============================ */
static void rodada_mestre(const Carta *c1, const Carta *c2) {
    puts("\n===== COMPARAÇÃO: DOIS ATRIBUTOS =====");
    int a1 = ler_opcao_primaria();
    int a2 = ler_opcao_secundaria(a1);

    puts("\n=== Cartas cadastradas ===");
    imprimir_carta(c1); puts("");
    imprimir_carta(c2); puts("");

    printf("=== Atributos escolhidos ===\n");
    printf("Primário:   %s\n", nome_atributo(a1));
    printf("Secundário: %s\n\n", nome_atributo(a2));

    /* Comparação individual (apenas exibição do vencedor por atributo) */
    int r1 = vencedor_por_atributo(c1, c2, a1);
    int r2 = vencedor_por_atributo(c1, c2, a2);

    printf("Valores — %s\n", c1->nomePais);
    printf("  Primário:   "); imprimir_valor_atributo(c1, a1); printf("\n");
    printf("  Secundário: "); imprimir_valor_atributo(c1, a2); printf("\n\n");

    printf("Valores — %s\n", c2->nomePais);
    printf("  Primário:   "); imprimir_valor_atributo(c2, a1); printf("\n");
    printf("  Secundário: "); imprimir_valor_atributo(c2, a2); printf("\n\n");

    printf("Vencedor por atributo:\n");
    printf("  %s: %s\n", nome_atributo(a1),
           r1==1 ? c1->nomePais : r1==2 ? c2->nomePais : "Empate");
    printf("  %s: %s\n\n", nome_atributo(a2),
           r2==1 ? c1->nomePais : r2==2 ? c2->nomePais : "Empate");

    /* Soma dos atributos (valores brutos) */
    double soma1 = valor_attr(c1, a1) + valor_attr(c1, a2);
    double soma2 = valor_attr(c2, a1) + valor_attr(c2, a2);

    printf("Somas:\n");
    printf("  %s: %.6f\n", c1->nomePais, soma1);
    printf("  %s: %.6f\n\n", c2->nomePais, soma2);

    /* Resultado final por soma */
    if (soma1 > soma2) {
        printf("Resultado: %s venceu pela maior soma!\n", c1->nomePais);
    } else if (soma2 > soma1) {
        printf("Resultado: %s venceu pela maior soma!\n", c2->nomePais);
    } else {
        printf("Resultado: Empate!\n");
    }
}

/* ============================
   Menu principal
   ============================ */
static void menu_principal(void) {
    puts("===== SUPER TRUNFO — PAÍSES (MESTRE) =====");
    puts("1) Exibir cartas");
    puts("2) Comparar por DOIS atributos");
    puts("3) Editar cartas");
    puts("0) Sair");
}

/* Cadastro simples (permitido manter pré-cadastradas, mas opção de editar ajuda usabilidade) */
static void cadastrar_carta(Carta *c, const char *rotulo) {
    printf("\n=== Cadastro da %s ===\n", rotulo);
    ler_string("Código da carta: ", c->codigo, sizeof(c->codigo));
    ler_string("Nome do país: ", c->nomePais, sizeof(c->nomePais));
    c->populacao        = (long long)ler_double("População (habitantes): ");
    c->area             = ler_double("Área (km²): ");
    c->pib              = ler_double("PIB (unidade livre): ");
    c->pontosTuristicos = ler_int("Pontos turísticos: ");
    calcular_derivados(c);
}

int main(void) {
    /* Cartas pré-cadastradas (permitido pelo enunciado do nível avançado) */
    Carta c1 = { "A01", "Brasil",        203000000, 8515767.0,  9847.0, 35, 0 };
    Carta c2 = { "B02", "Argentina",      46000000, 2780400.0,  4876.0, 22, 0 };
    calcular_derivados(&c1);
    calcular_derivados(&c2);

    while (1) {
        puts("");
        menu_principal();
        int op = ler_int("Opção: ");
        switch (op) {
            case 0:
                puts("Saindo... Obrigado por jogar!");
                return 0;

            case 1:
                puts("");
                imprimir_carta(&c1); puts("");
                imprimir_carta(&c2);
                break;

            case 2:
                rodada_mestre(&c1, &c2);
                break;

            case 3:
                cadastrar_carta(&c1, "Carta 1");
                cadastrar_carta(&c2, "Carta 2");
                break;

            default:
                puts("⚠️  Opção inválida. Tente novamente.");
                break;
        }
    }
    return 0;
}
