#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include "../include/botoes.h"
#include "../include/modbus.h"
#include "../include/motores.h"

#define UART_DEVICE "/dev/serial0"
static uint8_t MATR[4] = {0, 0, 6, 6};
static uint8_t last_reg_move_x = 0, last_reg_move_y = 0;
static int state_x = 0, state_y = 0;

// --- Pinos dos botões físicos (um por linha) ---
#define BTN_CIMA   16
#define BTN_BAIXO   1
#define BTN_ESQ     7
#define BTN_DIR     8
#define BTN_EMER   11

extern int open_uart(const char*); extern void send_uart(int, const uint8_t*, int);
extern short get_crc(unsigned char*, int); extern void close_uart(int);

void botoes_resetar_registrador_touch(uint8_t reg) {
    uint8_t buf[11] = {1, 6, reg, 1, 0, MATR[0], MATR[1], MATR[2], MATR[3]};
    short crc = get_crc(buf, 9);
    memcpy(&buf[9], &crc, 2);
    int fd = open_uart(UART_DEVICE);
    if (fd >= 0) { send_uart(fd, buf, 11); usleep(50000); close_uart(fd); }
}

void botoes_init(void) {
    wiringPiSetupGpio();
    pinMode(BTN_CIMA, INPUT); pullUpDnControl(BTN_CIMA, PUD_UP);
    pinMode(BTN_BAIXO, INPUT); pullUpDnControl(BTN_BAIXO, PUD_UP);
    pinMode(BTN_ESQ, INPUT); pullUpDnControl(BTN_ESQ, PUD_UP);
    pinMode(BTN_DIR, INPUT); pullUpDnControl(BTN_DIR, PUD_UP);
    pinMode(BTN_EMER, INPUT); pullUpDnControl(BTN_EMER, PUD_UP);
}

void botoes_processa_movimento_manual(uint8_t reg_move_x, uint8_t reg_move_y) {
    if ((reg_move_x & 1) && !(last_reg_move_x & 1)) { state_x = (state_x == 1) ? 0 : 1; botoes_resetar_registrador_touch(0); }
    if ((reg_move_x & 2) && !(last_reg_move_x & 2)) { state_x = (state_x == 2) ? 0 : 2; botoes_resetar_registrador_touch(0); }
    if ((reg_move_y & 1) && !(last_reg_move_y & 1)) { state_y = (state_y == 1) ? 0 : 1; botoes_resetar_registrador_touch(1); }
    if ((reg_move_y & 2) && !(last_reg_move_y & 2)) { state_y = (state_y == 2) ? 0 : 2; botoes_resetar_registrador_touch(1); }
    last_reg_move_x = reg_move_x; last_reg_move_y = reg_move_y;

    if (digitalRead(BTN_ESQ) == HIGH) motor_x_reverso();
    else if (digitalRead(BTN_DIR) == HIGH) motor_x_frente();
    else { if (state_x == 1) motor_x_reverso(); else if (state_x == 2) motor_x_frente(); else motor_x_parar(); }

    if (digitalRead(BTN_CIMA) == HIGH) motor_y_frente();
    else if (digitalRead(BTN_BAIXO) == HIGH) motor_y_reverso();
    else { if (state_y == 1) motor_y_frente(); else if (state_y == 2) motor_y_reverso(); else motor_y_parar(); }
}

int botao_emergencia_ativo(void) { return digitalRead(BTN_EMER) == HIGH; }