#include <stdio.h>
#include "../include/presets.h"
#include "../include/encoder.h"
#include "../include/posicionamento.h"
#include "../include/botoes.h" // Inclui para usar a função de reset

// Variáveis e structs do módulo...
typedef struct { int x; int y; } preset_posicao_t;
static preset_posicao_t posicoes_salvas[4];
static maquina_mode_t modo_atual = MODE_OPERACIONAL;

void presets_init(void) {
    for (int i = 0; i < 4; i++) {
        posicoes_salvas[i] = (preset_posicao_t){0, 0};
    }
    printf("Módulo de presets inicializado.\n");
}

maquina_mode_t presets_get_current_mode(void) {
    return modo_atual;
}

void presets_update(uint8_t reg_presets, uint8_t reg_programar) {
    if (reg_programar && modo_atual == MODE_OPERACIONAL) {
        modo_atual = MODE_PROGRAMACAO;
        printf("MODO DE PROGRAMAÇÃO ATIVADO. Posicione a máquina e aperte P1-P4 para salvar.\n");
        botoes_resetar_registrador_touch(3); // Reseta o botão "Programar" (addr 0x03)
        return;
    }

    if (modo_atual == MODE_PROGRAMACAO) {
        if (reg_presets > 0) {
            int pos_x_atual = -encoder_get_posX(); // Inverte X
            int pos_y_atual = -encoder_get_posY(); // Inverte Y
            int slot = -1;

            if (reg_presets & 0x01) slot = 0;
            else if (reg_presets & 0x02) slot = 1;
            else if (reg_presets & 0x04) slot = 2;
            else if (reg_presets & 0x08) slot = 3;

            if (slot != -1) {
                posicoes_salvas[slot] = (preset_posicao_t){pos_x_atual, pos_y_atual};
                printf("Posição P%d salva: X=%d, Y=%d\n", slot + 1, pos_x_atual, pos_y_atual);
                modo_atual = MODE_OPERACIONAL;
                printf("MODO DE OPERAÇÃO ATIVADO.\n");
                botoes_resetar_registrador_touch(2);
            }
        }
    } else { // MODE_OPERACIONAL
        if (reg_presets > 0 && !posicionamento_em_movimento()) {
            preset_posicao_t alvo = {0,0};
            int slot = -1;

            if (reg_presets & 0x01) slot = 0;
            else if (reg_presets & 0x02) slot = 1;
            else if (reg_presets & 0x04) slot = 2;
            else if (reg_presets & 0x08) slot = 3;

            if (slot != -1) {
                alvo = posicoes_salvas[slot];
                printf("Comando P%d recebido. Indo para: X=%d, Y=%d\n", slot + 1, alvo.x, alvo.y);
                posicionamento_goto(alvo.x, alvo.y);
                botoes_resetar_registrador_touch(2);
            }
        }
    }
}