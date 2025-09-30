#ifndef PRESETS_H
#define PRESETS_H

#include <stdint.h>

typedef enum {
    MODE_OPERACIONAL,
    MODE_PROGRAMACAO
} maquina_mode_t;

void presets_init(void);
void presets_update(uint8_t reg_presets, uint8_t reg_programar);
maquina_mode_t presets_get_current_mode(void);

#endif