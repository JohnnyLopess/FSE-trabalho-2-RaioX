#ifndef PID
#define PID

// Estrutura para armazenar o estado e os parametros de um controlador PID
typedef struct {
    // Ganhos do controlador
    double kp;
    double ki;
    double kd;

    // Termos do PID
    double integral;
    double prev_error;

    // Limites da saida (ex: 0 a 100 para o PWM)
    double out_min;
    double out_max;

} PIDController;

void pid_init(PIDController* pid, double kp, double ki, double kd, double out_min, double out_max);
void pid_reset(PIDController* pid);
double pid_update(PIDController* pid, double setpoint, double process_variable, double dt);

#endif // PID_CONTROL_H