#ifndef ENCODER_H
#define ENCODER_H

void encoder_init();
int encoder_get_posX();
int encoder_get_posY();
void encoder_print_pos();
void encoder_reset_posX(); // NOVO
void encoder_reset_posY(); // NOVO

#endif
