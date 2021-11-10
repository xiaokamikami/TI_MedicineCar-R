#ifndef __CONTROL_H
#define __CONTROL_H

extern unsigned int BMotorPulse,AMotorPulse;
extern int SpeedTarget;
extern int MotorOutput;
extern int Pwm;
extern int Error,Pulse;//Æ«²î
extern int MotorCouter;


void GetMotorPulse(void);
int SpeedInnerControl(int BPulse,int APulse,int Target);
void VoltageAndDirection(int Pwm);
void OpposeVoltageAndDirection(int Pwm);
void LeftDifferential(int Pwm);         //×ó²îÊý
void RightDifferential(int Pwm);        //ÓÒ²îÊý

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
