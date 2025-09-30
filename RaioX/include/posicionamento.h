#ifndef POSICIONAMENTO_H
#define POSICIONAMENTO_H

/**
 * @brief Inicializa os controladores PID para os eixos X e Y.
 * Deve ser chamado uma vez no início do programa.
 */
void posicionamento_init(void);

/**
 * @brief Define uma nova posição alvo para a máquina.
 * O sistema começará a se mover para este ponto.
 * @param target_x Posição alvo em pulsos do encoder para o eixo X.
 * @param target_y Posição alvo em pulsos do encoder para o eixo Y.
 */
void posicionamento_goto(int target_x, int target_y);

/**
 * @brief Atualiza o estado do controle de posicionamento.
 * Esta função deve ser chamada repetidamente no loop principal do programa.
 * Ela calcula a saída do PID e comanda os motores.
 */
void posicionamento_update(void);

/**
 * @brief Verifica se a máquina ainda está em movimento para o alvo.
 * @return 1 se a máquina está se movendo, 0 se chegou ao destino.
 */
int posicionamento_em_movimento(void);

#endif // POSICIONAMENTO_H