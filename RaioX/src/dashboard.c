#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "../include/dashboard.h"
#include "../include/modbus.h"
#include "../include/encoder.h"

// --- Definições Globais ---
#define UART_DEVICE "/dev/serial0"
static uint8_t MATR[4] = {0, 0, 6, 6}; // Sua Matrícula

// Variáveis para conversão de pulsos para metros
int _pulsos_max_x = 1;
int _pulsos_max_y = 1;
#define X_TOTAL_METROS 3.0f
#define Y_TOTAL_METROS 1.8f

// Protótipos das funções da UART (definidas em uart.c)
extern int open_uart(const char *device);
extern void send_uart(int fd, const uint8_t *buffer, int len);
extern void close_uart(int fd);

/**
 * @brief Envia um lote de dados para o painel de uma só vez.
 * ESTA É A VERSÃO OTIMIZADA E CORRIGIDA.
 */
void dashboard_write_batch(float vx, float vy, float x, float y, float temp, float press, uint8_t estado) {
    int uart_fd = open_uart(UART_DEVICE);
    if (uart_fd < 0) {
        perror("Dashboard: Nao foi possivel abrir a UART.");
        return;
    }

    uint8_t buffer[64];
    mb_package pkg = {.addr = 0x01, .code = 0x06, .matr = MATR};

    // --- Envio de Floats ---
    // Para garantir que não haja corrupção de dados, preparamos
    // o pacote completo para cada envio.
    
    // Enviar Velocidade X
    {
        uint8_t data[] = {0x05, 0x04, 0, 0, 0, 0};
        memcpy(&data[2], &vx, sizeof(float));
        pkg.offset = sizeof(data);
        pkg.data = data;
        send_uart(uart_fd, buffer, fill_buffer(buffer, pkg));
        usleep(20000);
    }

    // Enviar Velocidade Y
    {
        uint8_t data[] = {0x09, 0x04, 0, 0, 0, 0};
        memcpy(&data[2], &vy, sizeof(float));
        pkg.offset = sizeof(data);
        pkg.data = data;
        send_uart(uart_fd, buffer, fill_buffer(buffer, pkg));
        usleep(20000);
    }

    // Enviar Posição X
    {
        uint8_t data[] = {0x0D, 0x04, 0, 0, 0, 0};
        memcpy(&data[2], &x, sizeof(float));
        pkg.offset = sizeof(data);
        pkg.data = data;
        send_uart(uart_fd, buffer, fill_buffer(buffer, pkg));
        usleep(20000);
    }

    // Enviar Posição Y
    {
        uint8_t data[] = {0x11, 0x04, 0, 0, 0, 0};
        memcpy(&data[2], &y, sizeof(float));
        pkg.offset = sizeof(data);
        pkg.data = data;
        send_uart(uart_fd, buffer, fill_buffer(buffer, pkg));
        usleep(20000);
    }
    
    // Enviar Temperatura
    {
        uint8_t data[] = {0x15, 0x04, 0, 0, 0, 0};
        memcpy(&data[2], &temp, sizeof(float));
        pkg.offset = sizeof(data);
        pkg.data = data;
        send_uart(uart_fd, buffer, fill_buffer(buffer, pkg));
        usleep(20000);
    }

    // Enviar Pressão
    {
        uint8_t data[] = {0x19, 0x04, 0, 0, 0, 0};
        memcpy(&data[2], &press, sizeof(float));
        pkg.offset = sizeof(data);
        pkg.data = data;
        send_uart(uart_fd, buffer, fill_buffer(buffer, pkg));
        usleep(20000);
    }

    // --- Envio de Byte (Estado) ---
    {
        uint8_t data[] = {0x1D, 0x01, estado};
        pkg.offset = sizeof(data);
        pkg.data = data;
        send_uart(uart_fd, buffer, fill_buffer(buffer, pkg));
        usleep(20000);
    }

    close_uart(uart_fd);
}

/**
 * @brief Define o estado da máquina (0=Operação, 1=Calibração).
 * Usado pela rotina de calibração para atualizar o painel.
 */
void dashboard_write_estado(uint8_t estado) {
    int uart_fd = open_uart(UART_DEVICE);
    if (uart_fd < 0) return;
    
    uint8_t buffer[64];
    uint8_t data[] = {0x1D, 0x01, estado};

    mb_package pkg = {
        .addr = 0x01,
        .code = 0x06,
        .offset = sizeof(data),
        .data = data,
        .matr = MATR
    };

    send_uart(uart_fd, buffer, fill_buffer(buffer, pkg));
    usleep(50000);
    close_uart(uart_fd);
}

/**
 * @brief Armazena os valores máximos de pulsos após a calibração.
 */
void dashboard_set_calibracao(int pulsos_max_x, int pulsos_max_y) {
    _pulsos_max_x = (pulsos_max_x > 0) ? pulsos_max_x : 1;
    _pulsos_max_y = (pulsos_max_y > 0) ? pulsos_max_y : 1;
}

/**
 * @brief Calcula as posições e velocidades e envia o lote de dados para o painel.
 * Esta função deve ser chamada em loop pelo main.c.
 */
void dashboard_envia_dados_loop(float temp, float press, int estado_op) {
    static float last_pos_x_m = 0.0f;
    static float last_pos_y_m = 0.0f;
    static struct timeval last_time = {0, 0};

    if (last_time.tv_sec == 0) {
        gettimeofday(&last_time, NULL);
        return;
    }

    struct timeval now;
    gettimeofday(&now, NULL);
    double dt = (now.tv_sec - last_time.tv_sec) + (now.tv_usec - last_time.tv_usec) / 1e6;

    if (dt >= 0.1) { // Taxa de atualização do painel (ex: a cada 100ms)
        float pos_x_metros = ((float)-encoder_get_posX() / (float)_pulsos_max_x) * X_TOTAL_METROS;
        float pos_y_metros = ((float)-encoder_get_posY() / (float)_pulsos_max_y) * Y_TOTAL_METROS;
        float vel_x = (pos_x_metros - last_pos_x_m) / dt;
        float vel_y = (pos_y_metros - last_pos_y_m) / dt;

        dashboard_write_batch(vel_x, vel_y, pos_x_metros, pos_y_metros, temp, press, (uint8_t)estado_op);

        last_pos_x_m = pos_x_metros;
        last_pos_y_m = pos_y_metros;
        last_time = now;
    }
}