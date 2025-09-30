#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "../include/modbus.h"
#include "../include/raiox.h"
#include "../include/encoder.h"
#include "../include/motores.h"
#include "../include/dashboard.h"
#include "../include/sensor_BMP280.h"
#include "../include/botoes.h"
#include "../include/calibracao.h"
#include "../include/posicionamento.h"
#include "../include/presets.h"

void handle_sigint(int sig);

// Função que centraliza a leitura do painel touch
void ler_painel_touch(uint8_t* reg_move_x, uint8_t* reg_move_y, uint8_t* reg_presets, uint8_t* reg_programar, uint8_t* reg_calibrar) {
    uint8_t buf[256], resp[256], dados[2] = {0x00, 0x05};
    uint8_t matr[] = {0,0,6,6}; // Matrícula
    mb_package pkg = {.addr=1, .code=3, .offset=2, .data=dados, .matr=matr};
    
    int len = fill_buffer(buf, pkg);
    int fd  = open_uart("/dev/serial0");
    if (fd < 0) return;

    send_uart(fd, buf, len);
    usleep(50000);
    int rcv = receive_uart(fd, resp, 256);
    close_uart(fd);

    if (rcv >= 9 && get_crc(resp, rcv - 2) == *(short*)&resp[rcv - 2]) {
        *reg_move_x    = resp[3]; // Addr 0x00
        *reg_move_y    = resp[4]; // Addr 0x01
        *reg_presets   = resp[5]; // Addr 0x02
        *reg_programar = resp[6]; // Addr 0x03
        *reg_calibrar  = resp[7]; // Addr 0x04
    }
}

int main() {
    int pulsos_max_x, pulsos_max_y;
    signal(SIGINT, handle_sigint);

    // Inicializações
    encoder_init();
    motores_init();
    botoes_init();
    presets_init();
    raiox_init();
    posicionamento_init(); 
    sensor_temperatura_init();

    // Calibração Inicial Obrigatória
    calibrar_maquina(&pulsos_max_x, &pulsos_max_y);
    dashboard_set_calibracao(abs(pulsos_max_x), abs(pulsos_max_y));
    
    while(1){
        if (botao_emergencia_ativo()) {
            motor_x_parar(); motor_y_parar();
            if(posicionamento_em_movimento()){
                posicionamento_goto(encoder_get_posX(), encoder_get_posY());
            }
            raiox_set(0);
        } else {
            // 1. LÊ OS DADOS DO PAINEL TOUCH
            uint8_t reg_move_x = 0, reg_move_y = 0, reg_presets = 0, reg_programar = 0, reg_calibrar = 0;
            ler_painel_touch(&reg_move_x, &reg_move_y, &reg_presets, &reg_programar, &reg_calibrar);

            // 2. VERIFICA SE O BOTÃO DE CALIBRAÇÃO FOI PRESSIONADO
            if (reg_calibrar) {
                printf("Comando de calibração recebido!\n");
                // Para qualquer movimento em andamento antes de calibrar
                motor_x_parar();
                motor_y_parar();
                // Executa a rotina de calibração completa
                calibrar_maquina(&pulsos_max_x, &pulsos_max_y);
                // Atualiza o dashboard com os novos limites
                dashboard_set_calibracao(abs(pulsos_max_x), abs(pulsos_max_y));
                // Reseta o botão no painel para não re-calibrar no próximo ciclo
                botoes_resetar_registrador_touch(4); // Endereço 0x04
            } else {
                // Se não estiver calibrando, segue a lógica normal
                presets_update(reg_presets, reg_programar);

                if (posicionamento_em_movimento()) {
                    posicionamento_update();
                } else if (presets_get_current_mode() != MODE_PROGRAMACAO) {
                    botoes_processa_movimento_manual(reg_move_x, reg_move_y);
                }
            }

            // 3. ATUALIZA SENSORES E DASHBOARD
            double temp = sensor_temperatura_read();
            double press = sensor_pressao_read();
            int em_prog = (presets_get_current_mode() == MODE_PROGRAMACAO);
            dashboard_envia_dados_loop(temp, press, posicionamento_em_movimento() || em_prog);
        }
        
        usleep(20000);
    }
    return 0;
}

void handle_sigint(int sig) {
    (void)sig; 
    printf("\n\nCtrl+C recebido. Desligando motores e encerrando...\n");
    motor_x_parar(); motor_y_parar(); raiox_set(0);
    exit(0);
}