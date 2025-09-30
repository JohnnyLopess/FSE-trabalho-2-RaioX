#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "../include/posicionamento.h"
#include "../include/pid.h"
#include "../include/motores.h"
#include "../include/encoder.h"

static PIDController pid_x, pid_y;
static int setpoint_x = 0, setpoint_y = 0;
static volatile int em_movimento = 0;
static struct timeval last_update_time;
#define POS_TOLERANCE 5

void posicionamento_init(void) {
    double out_min = -100.0, out_max = 100.0;
    
    // Constantes estáveis para o Eixo X
    double kp_x = 5.0, ki_x = 0.1, kd_x = 25.0;
    printf("PID X. Kp=%.2f, Ki=%.2f, Kd=%.2f\n", kp_x, ki_x, kd_x);
    pid_init(&pid_x, kp_x, ki_x, kd_x, out_min, out_max);

    // Constantes um pouco mais fortes para o Eixo Y
    double kp_y = 8.0, ki_y = 0.5, kd_y = 30.0;
    printf("PID Y. Kp=%.2f, Ki=%.2f, Kd=%.2f\n", kp_y, ki_y, kd_y);
    pid_init(&pid_y, kp_y, ki_y, kd_y, out_min, out_max);

    gettimeofday(&last_update_time, NULL);
}

void posicionamento_goto(int target_x, int target_y) {
    setpoint_x = target_x;
    setpoint_y = target_y;
    pid_reset(&pid_x);
    pid_reset(&pid_y);
    em_movimento = 1;
}

int posicionamento_em_movimento(void) { return em_movimento; }

void posicionamento_update(void) {
    if (!em_movimento) return;

    struct timeval now;
    gettimeofday(&now, NULL);
    double dt = (now.tv_sec - last_update_time.tv_sec) + (now.tv_usec - last_update_time.tv_usec) / 1e6;
    last_update_time = now;
    if (dt <= 0) return;

    int current_pos_x = -encoder_get_posX();
    int error_x = setpoint_x - current_pos_x;
    int chegou_x = abs(error_x) <= POS_TOLERANCE;
    if (chegou_x) {
        motor_x_parar();
    } else {
        motor_x_movimentar_pid((int)pid_update(&pid_x, setpoint_x, current_pos_x, dt));
    }

    int current_pos_y = -encoder_get_posY();
    int error_y = setpoint_y - current_pos_y;
    int chegou_y = abs(error_y) <= POS_TOLERANCE;
    if (chegou_y) {
        motor_y_parar();
    } else {
        motor_y_movimentar_pid((int)pid_update(&pid_y, setpoint_y, current_pos_y, dt));
    }

    if (chegou_x && chegou_y) {
        em_movimento = 0;
        printf("CHEGOU AO DESTINO.\n");
    }
}