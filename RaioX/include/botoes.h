#ifndef BOTOES_H
#define BOTOES_H

#include <stdint.h>

void botoes_init(void);
void botoes_processa_movimento_manual(uint8_t reg_move_x, uint8_t reg_move_y);
int botao_emergencia_ativo(void);
void botoes_resetar_registrador_touch(uint8_t reg);

#endif