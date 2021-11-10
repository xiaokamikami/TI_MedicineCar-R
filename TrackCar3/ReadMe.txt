include_directories(Core/Inc Drivers/STM32F1xx_HAL_Driver/Inc Drivers/STM32F1xx_HAL_Driver/Inc/Legacy Drivers/CMSIS/Device/ST/STM32F1xx/Include Drivers/CMSIS/Include User/mystdio User/Mode User/Date)

add_definitions(-DUSE_HAL_DRIVER -DSTM32F103xE)

file(GLOB_RECURSE SOURCES "startup/*.*" "Drivers/*.*" "Core/*.*" "User/*.*")

�����ֵ��SpeedTarget��100-7200
���ת�����45���󣩣����125���ң�����λ��70(11��6��)

�������ӣ�
1.A���
PA7<--->AIN1
PA6<--->AIN2

2.A������
PA0<--->���ߣ�A��
PA1<--->���ߣ�B��

3.B���
PB1<--->BIN1
PB0<--->BIN2

4.B������
PD13<--->��ɫ��A��
PD12<--->��ɫ��B��

5.���
PE9<--->��ɫ

6.����
[]<--->R4
[]<--->R3
PD8<--->R2(��)
PD9<--->R1(��)
PD10<--->L1(��)
PD11<--->L2(��)
[]<--->L3
[PD13]<--->L4
��ʾ����ά��ֱ��ʱ����һ�ߵĵ����˾�����һ����