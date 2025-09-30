#include <stdio.h>
#include <wiringPi.h>
#include "raiox.h"
#include "botoes.h"

#define PIN_RAIOX 18
static int estado_raiox = 0; // 0 = desligado, 1 = ligado

void raiox_init(void) {
    wiringPiSetupGpio();
    pinMode(PIN_RAIOX, OUTPUT);
    estado_raiox = 0;
    digitalWrite(PIN_RAIOX, LOW);
}

void raiox_set(int estado) {
    estado_raiox = estado ? 1 : 0;
    digitalWrite(PIN_RAIOX, estado_raiox ? HIGH : LOW);
}

void raiox_teste(void) {
    raiox_set(1);
    printf("[RaioX - TESTE] Forçando LED LIGADO para teste\n");
}


