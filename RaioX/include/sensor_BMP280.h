/* include/sensor_temperatura.h */
#ifndef SENSOR_TEMPERATURA_H
#define SENSOR_TEMPERATURA_H

#include <stdint.h>

/**
 * Inicializa o BMP280 via I2C (Device File).
 * Deve ser chamado antes de qualquer leitura.
 * @return file descriptor >= 0 em caso de sucesso, ou -1 em erro.
 */
int sensor_temperatura_init(void);

/**
 * Lê a temperatura compensada em °C.
 * Deve ser chamado após sensor_temperatura_init().
 * @return temperatura em °C, ou NAN em caso de falha.
 */
double sensor_temperatura_read(void);

/**
 * Lê a pressão compensada em hPa.
 * Deve ser chamado após sensor_temperatura_init().
 * @return pressão em hPa, ou NAN em caso de falha.
 */
double sensor_pressao_read(void);

#endif // SENSOR_TEMPERATURA_H