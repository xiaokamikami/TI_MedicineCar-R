#ifndef __CONTROL_H
#define __CONTROL_H
#include "main.h"
extern unsigned int BMotorPulse,AMotorPulse;
extern int SpeedTarget;
extern int MotorOutput;
extern int Pwm;
extern int Error,Pulse;//Æ«²î
extern int MotorCouter;


void GetMotorPulse(void);
int SpeedInnerControl(int BPulse,int APulse,int Target);
void VoltageAndDirection(int Pwm1,int Pwm2);
void OpposeVoltageAndDirection(int Pwm1,int Pwm2);
void LeftDifferential(int Pwm1,int Pwm2);         //×ó²îÊý
void RightDifferential(int Pwm1,int Pwm2);        //ÓÒ²îÊý
void Motor_dict(uint8_t Motox,uint8_t mode);
int SpeedInnerControl1(int BPulse,int Target);
int SpeedInnerControl2(int APulse,int Target);
void BaseOne(void);
void BaseTwo(void);
void BaseThree(void);
void BaseFour(void);
void BaseFive(void);
void BaseSix(void);
void BaseSeven(void);
void BaseEight(void);

void CarStop(void);
void CarStart(void);

#endif
