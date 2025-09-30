#ifndef RAIOX_H
#define RAIOX_H

/**
 * Inicializa o pino do LED de Raio-X.
 */
void raiox_init(void);

/**
 * Liga/desliga o LED de Raio-X.
 * @param estado 1 = ligar, 0 = desligar
 */
void raiox_set(int estado);

/**
 * Função de teste: força ligar o LED.
 */
void raiox_teste(void);

#endif // RAIOX_H