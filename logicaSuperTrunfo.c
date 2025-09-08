#include <stdio.h>
#include <string.h>

/*
  Desafio: Comparação de Cartas do Super Trunfo (nível novato)

  Como usar:
  1) Altere a constante COMP_ATTR para escolher o atributo de comparação:
     - 0: População (maior vence)
     - 1: Área (maior vence)
     - 2: PIB (maior vence)
     - 3: Densidade Populacional (menor vence)
     - 4: PIB per capita (maior vence)

  2) Compile e rode:
     gcc -Wall -Wextra -O2 super_trunfo.c -o super_trunfo && ./super_trunfo
*/

// ======= CONFIGURAÇÃO DO ATRIBUTO DE COMPARAÇÃO =======
#define ATTR_POPULACAO        0
#define ATTR_AREA             1
#define ATTR_PIB              2
#define ATTR_DENSIDADE        3
#define ATTR_PIB_PER_CAPITA   4

// Escolha aqui o atributo usado na comparação:
#define COMP_ATTR ATTR_POPULACAO
// ======================================================

typedef struct {
    char estado[3];            // Ex: "SP"
    char codigo[4];            // Ex: "A01"
    char nomeCidade[64];       // Ex: "São Paulo"
    int  populacao;            // habitantes
    double area;               // km²
    double pib;                // em bilhões (ou a unidade que você preferir)
    int  pontosTuristicos;     // quantidade
    // Derivados (calculados)
    double densidade;          // populacao / area
    double pibPerCapita;       // pib / populacao
} Carta;

/* Calcula atributos derivados de uma carta. */
void calcular_derivados(Carta *c) {
    // Proteções simples para evitar divisão por zero
    if (c->area > 0.0) {
        c->densidade = (double)c->populacao / c->area;
    } else {
        c->densidade = 0.0;
    }

    if (c->populacao > 0) {
        c->pibPerCapita = c->pib / (double)c->populacao;
    } else {
        c->pibPerCapita = 0.0;
    }
}

/* Retorna o valor numérico do atributo escolhido para a carta c. */
double valor_do_atributo(const Carta *c, int atributo) {
    switch (atributo) {
        case ATTR_POPULACAO:      return (double)c->populacao;
        case ATTR_AREA:           return c->area;
        case ATTR_PIB:            return c->pib;
        case ATTR_DENSIDADE:      return c->densidade;
        case ATTR_PIB_PER_CAPITA: return c->pibPerCapita;
        default:                  return 0.0;
    }
}

/* Diz se "maior vence" para o atributo. Densidade é a exceção (menor vence). */
int maior_vence(int atributo) {
    return atributo != ATTR_DENSIDADE;
}

/* Compara duas cartas com base em um atributo.
   Retorna: 1 se carta1 vence, 2 se carta2 vence, 0 se empate. */
int comparar_cartas(const Carta *c1, const Carta *c2, int atributo) {
    double v1 = valor_do_atributo(c1, atributo);
    double v2 = valor_do_atributo(c2, atributo);

    if (maior_vence(atributo)) {
        if (v1 > v2) return 1;
        if (v2 > v1) return 2;
    } else { // menor vence (densidade)
        if (v1 < v2) return 1;
        if (v2 < v1) return 2;
    }
    return 0; // empate
}

/* Nome legível do atributo para impressão. */
const char* nome_atributo(int atributo) {
    switch (atributo) {
        case ATTR_POPULACAO:      return "População";
        case ATTR_AREA:           return "Área";
        case ATTR_PIB:            return "PIB";
        case ATTR_DENSIDADE:      return "Densidade Populacional";
        case ATTR_PIB_PER_CAPITA: return "PIB per capita";
        default:                  return "Atributo desconhecido";
    }
}

/* Impressão amigável de uma carta. */
void imprimir_carta(const Carta *c) {
    printf("Carta %s - %s (%s)\n", c->codigo, c->nomeCidade, c->estado);
    printf("  População: %d hab\n", c->populacao);
    printf("  Área: %.2f km²\n", c->area);
    printf("  PIB: %.2f\n", c->pib);
    printf("  Pontos turísticos: %d\n", c->pontosTuristicos);
    printf("  Densidade Populacional: %.2f hab/km²\n", c->densidade);
    printf("  PIB per capita: %.6f\n", c->pibPerCapita);
}

/* Impressão formatada do valor do atributo escolhido. */
void imprimir_valor_atributo(const Carta *c, int atributo) {
    switch (atributo) {
        case ATTR_POPULACAO:
            printf("%d\n", c->populacao);
            break;
        case ATTR_AREA:
            printf("%.2f km²\n", c->area);
            break;
        case ATTR_PIB:
            printf("%.2f\n", c->pib);
            break;
        case ATTR_DENSIDADE:
            printf("%.2f hab/km²\n", c->densidade);
            break;
        case ATTR_PIB_PER_CAPITA:
            printf("%.6f\n", c->pibPerCapita);
            break;
        default:
            printf("(atributo inválido)\n");
    }
}

int main(void) {
    // ===== Cadastro das cartas (exemplo; reutilize seu código se quiser) =====
    Carta carta1 = {
        .estado = "SP",
        .codigo = "A01",
        .nomeCidade = "São Paulo",
        .populacao = 12300000,
        .area = 1521.11,
        .pib = 699.28,          // unidade livre (ex.: bilhões)
        .pontosTuristicos = 30
    };

    Carta carta2 = {
        .estado = "RJ",
        .codigo = "B02",
        .nomeCidade = "Rio de Janeiro",
        .populacao = 6710000,
        .area = 1200.27,
        .pib = 364.11,
        .pontosTuristicos = 25
    };

    // Calcular derivados
    calcular_derivados(&carta1);
    calcular_derivados(&carta2);

    // ===== Saída clara dos dados =====
    printf("=== Cartas cadastradas ===\n");
    imprimir_carta(&carta1);
    printf("\n");
    imprimir_carta(&carta2);
    printf("\n");

    // ===== Comparação =====
    int atributo = COMP_ATTR;
    int resultado = comparar_cartas(&carta1, &carta2, atributo);

    printf("=== Comparação de cartas (Atributo: %s) ===\n", nome_atributo(atributo));

    printf("Carta 1 - %s (%s): ", carta1.nomeCidade, carta1.estado);
    imprimir_valor_atributo(&carta1, atributo);

    printf("Carta 2 - %s (%s): ", carta2.nomeCidade, carta2.estado);
    imprimir_valor_atributo(&carta2, atributo);

    if (resultado == 1) {
        printf("\nResultado: Carta 1 (%s) venceu!\n", carta1.nomeCidade);
    } else if (resultado == 2) {
        printf("\nResultado: Carta 2 (%s) venceu!\n", carta2.nomeCidade);
    } else {
        printf("\nResultado: Empate!\n");
    }

    // Observação: Para Densidade Populacional, a menor vence (regra aplicada no código).
    return 0;
}
