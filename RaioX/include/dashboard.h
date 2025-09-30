#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <stdint.h>

/**
 * @brief Armazena os valores máximos de pulsos após a calibração.
 * @param pulsos_max_x Contagem máxima de pulsos do encoder no eixo X.
 * @param pulsos_max_y Contagem máxima de pulsos do encoder no eixo Y.
 */
void dashboard_set_calibracao(int pulsos_max_x, int pulsos_max_y);

/**
 * @brief Define o estado da máquina (0=Operação, 1=Calibração) no painel.
 * @param estado O novo estado a ser exibido.
 */
void dashboard_write_estado(uint8_t estado);

/**
 * @brief Calcula as posições/velocidades e envia todos os dados para o painel.
 * Esta é a função principal que deve ser chamada em loop pelo main.c.
 * @param temp Valor da temperatura a ser enviado.
 * @param press Valor da pressão a ser enviado.
 * @param estado_op Estado atual da operação (0=Parado/Manual, 1=Mov. Automático).
 */
void dashboard_envia_dados_loop(float temp, float press, int estado_op);

#endif // DASHBOARD_H