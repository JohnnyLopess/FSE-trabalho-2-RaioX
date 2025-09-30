#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>  // <--- NOVO
#include "motores.h"
#include <stdlib.h>

// Definição dos GPIOs dos motores (BCM)
#define X_PWM   17
#define X_DIR1  27
#define X_DIR2  22

#define Y_PWM   23
#define Y_DIR1  24
#define Y_DIR2  25

#define X_MIN_PIN 26
#define X_MAX_PIN 19
#define Y_MIN_PIN 20
#define Y_MAX_PIN 21

int DUTY_CYCLE = 80; // Valor padrão inicial

void motores_init() {
    wiringPiSetupGpio();

    pinMode(X_DIR1, OUTPUT);  pinMode(X_DIR2, OUTPUT);
    pinMode(Y_DIR1, OUTPUT);  pinMode(Y_DIR2, OUTPUT);
    pinMode(X_MIN_PIN, INPUT);
    pinMode(X_MAX_PIN, INPUT);
    pinMode(Y_MIN_PIN, INPUT);
    pinMode(Y_MAX_PIN, INPUT);

    digitalWrite(X_DIR1, LOW); digitalWrite(X_DIR2, LOW);
    digitalWrite(Y_DIR1, LOW); digitalWrite(Y_DIR2, LOW);

    // Inicializa PWM com duty inicial 0 e valor máximo 100
    softPwmCreate(X_PWM, 0, 100);
    softPwmCreate(Y_PWM, 0, 100);
}

void motor_x_frente() {
    // Se já estiver no limite máximo de X, não anda pra frente
    if (fim_curso_x_max()) {
        printf("[Motor X] Limite X_MAX atingido! Parando.\n");
        motor_x_parar();
        return;
    }
    digitalWrite(X_DIR1, HIGH);
    digitalWrite(X_DIR2, LOW);
    softPwmWrite(X_PWM, DUTY_CYCLE);
    printf("[Motor X] Frente\n");
}

void motor_x_reverso() {
    // Se já estiver no limite mínimo de X, não anda pra trás
    if (fim_curso_x_min()) {
        printf("[Motor X] Limite X_MIN atingido! Parando.\n");
        motor_x_parar();
        return;
    }
    digitalWrite(X_DIR1, LOW);
    digitalWrite(X_DIR2, HIGH);
    softPwmWrite(X_PWM, DUTY_CYCLE);
    printf("[Motor X] Reverso\n");
}

void motor_x_parar() {
    digitalWrite(X_DIR1, HIGH);  // freio ativo
    digitalWrite(X_DIR2, HIGH);
    softPwmWrite(X_PWM, 0);      // desliga PWM
    printf("[Motor X] Parado\n");
}

void motor_y_frente() {
    // Se já estiver no limite máximo de Y, não sobe mais
    if (fim_curso_y_max()) {
        printf("[Motor Y] Limite Y_MAX atingido! Parando.\n");
        motor_y_parar();
        return;
    }
    digitalWrite(Y_DIR1, HIGH);
    digitalWrite(Y_DIR2, LOW);
    softPwmWrite(Y_PWM, DUTY_CYCLE);
    printf("[Motor Y] Frente\n");
}

void motor_y_reverso() {
    // Se já estiver no limite mínimo de Y, não desce mais
    if (fim_curso_y_min()) {
        printf("[Motor Y] Limite Y_MIN atingido! Parando.\n");
        motor_y_parar();
        return;
    }
    digitalWrite(Y_DIR1, LOW);
    digitalWrite(Y_DIR2, HIGH);
    softPwmWrite(Y_PWM, DUTY_CYCLE);
    printf("[Motor Y] Reverso\n");
}

void motor_y_parar() {
    digitalWrite(Y_DIR1, HIGH);  // freio ativo
    digitalWrite(Y_DIR2, HIGH);
    softPwmWrite(Y_PWM, 0);      // desliga PWM
    printf("[Motor Y] Parado\n");
}

void motor_x_movimentar_pid(int pwm) {
    if (pwm > 100) pwm = 100;
    if (pwm < -100) pwm = -100;

    if (pwm > 0) { // Frente
        if (fim_curso_x_max()) { motor_x_parar(); return; }
        digitalWrite(X_DIR1, HIGH);
        digitalWrite(X_DIR2, LOW);
        softPwmWrite(X_PWM, pwm);
    } else if (pwm < 0) { // Reverso
        if (fim_curso_x_min()) { motor_x_parar(); return; }
        digitalWrite(X_DIR1, LOW);
        digitalWrite(X_DIR2, HIGH);
        softPwmWrite(X_PWM, abs(pwm));
    } else {
        motor_x_parar();
    }
}

void motor_y_movimentar_pid(int pwm) {
    // Implementação similar para o eixo Y
    if (pwm > 100) pwm = 100;
    if (pwm < -100) pwm = -100;

    if (pwm > 0) { // Frente (para cima)
        if (fim_curso_y_max()) { motor_y_parar(); return; }
        digitalWrite(Y_DIR1, HIGH);
        digitalWrite(Y_DIR2, LOW);
        softPwmWrite(Y_PWM, pwm);
    } else if (pwm < 0) { // Reverso (para baixo)
        if (fim_curso_y_min()) { motor_y_parar(); return; }
        digitalWrite(Y_DIR1, LOW);
        digitalWrite(Y_DIR2, HIGH);
        softPwmWrite(Y_PWM, abs(pwm));
    } else {
        motor_y_parar();
    }
}

int fim_curso_x_min() { return digitalRead(X_MIN_PIN) == 1; }
int fim_curso_x_max() { return digitalRead(X_MAX_PIN) == 1; }
int fim_curso_y_min() { return digitalRead(Y_MIN_PIN) == 1; }
int fim_curso_y_max() { return digitalRead(Y_MAX_PIN) == 1; }

void motores_stop_on_limits(void) {
    // se qualquer limite X estiver acionado, freia X
    if (fim_curso_x_min() || fim_curso_x_max()) {
        motor_x_parar();
    }
    // se qualquer limite Y estiver acionado, freia Y
    if (fim_curso_y_min() || fim_curso_y_max()) {
        motor_y_parar();
    }
}