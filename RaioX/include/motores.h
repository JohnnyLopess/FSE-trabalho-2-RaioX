// motores.h
#ifndef MOTORES_H
#define MOTORES_H

extern int DUTY_CYCLE;

void motores_init(void);
void motor_x_frente(void);
void motor_x_reverso(void);
void motor_x_parar(void);
void motor_y_frente(void);
void motor_y_reverso(void);
void motor_y_parar(void);
void motor_atualizar_com_botoes(void);

int  fim_curso_x_min(void);
int  fim_curso_x_max(void);
int  fim_curso_y_min(void);
int  fim_curso_y_max(void);

void motor_x_movimentar_pid(int pwm);
void motor_y_movimentar_pid(int pwm);

/**
 * Chama a parada de cada eixo se o respectivo fim de curso estiver ativo.
 * Deve ser executado a cada iteração do loop principal.
 */
void motores_stop_on_limits(void);

#endif // MOTORES_H
