#ifndef __CONTROL_H
#define __CONTROL_H

extern unsigned int BMotorPulse,AMotorPulse;
extern int SpeedTarget;
extern int MotorOutput;
extern int Pwm;
extern int Error,Pulse;//ƫ��
extern int MotorCouter;


void GetMotorPulse(void);
int SpeedInnerControl(int BPulse,int APulse,int Target);
void VoltageAndDirection(int Pwm);
void OpposeVoltageAndDirection(int Pwm);
void LeftDifferential(int Pwm);         //�����
void RightDifferential(int Pwm);        //�Ҳ���

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
