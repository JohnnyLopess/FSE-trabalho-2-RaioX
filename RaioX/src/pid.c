#include "pid.h"

void pid_init(PIDController* pid, double kp, double ki, double kd, double out_min, double out_max) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->out_min = out_min;
    pid->out_max = out_max;
    pid->integral = 0.0;
    pid->prev_error = 0.0;
}

void pid_reset(PIDController* pid) {
    pid->integral = 0.0;
    pid->prev_error = 0.0;
}

double pid_update(PIDController* pid, double setpoint, double process_variable, double dt) {
    // Calcula o erro
    double error = setpoint - process_variable;

    // Termo Proporcional
    double p_out = pid->kp * error;

    // Termo Integral com anti-windup
    pid->integral += error * dt;
    // Evita que o termo integral cresca indefinidamente
    if (pid->integral > pid->out_max) pid->integral = pid->out_max;
    else if (pid->integral < pid->out_min) pid->integral = pid->out_min;
    
    double i_out = pid->ki * pid->integral;

    // Termo Derivativo
    double derivative = (error - pid->prev_error) / dt;
    double d_out = pid->kd * derivative;

    // Saida total
    double output = p_out + i_out + d_out;

    // Limita a saida aos valores maximo e minimo
    if (output > pid->out_max) output = pid->out_max;
    else if (output < pid->out_min) output = pid->out_min;
    
    // Guarda o erro atual para a proxima iteracao
    pid->prev_error = error;

    return output;
}