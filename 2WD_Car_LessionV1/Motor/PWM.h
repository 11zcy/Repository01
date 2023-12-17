#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void Set_compare(uint8_t Oc,int16_t count);
int16_t Limit_Pwm(int16_t val,int16_t Max);

#endif
