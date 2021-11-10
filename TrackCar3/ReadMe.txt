include_directories(Core/Inc Drivers/STM32F1xx_HAL_Driver/Inc Drivers/STM32F1xx_HAL_Driver/Inc/Legacy Drivers/CMSIS/Device/ST/STM32F1xx/Include Drivers/CMSIS/Include User/mystdio User/Mode User/Date)

add_definitions(-DUSE_HAL_DRIVER -DSTM32F103xE)

file(GLOB_RECURSE SOURCES "startup/*.*" "Drivers/*.*" "Core/*.*" "User/*.*")

电机阈值：SpeedTarget：100-7200
舵机转向：最低45（左），最高125（右）正中位置70(11月6日)

引脚连接：
1.A电机
PA7<--->AIN1
PA6<--->AIN2

2.A编码器
PA0<--->绿线（A）
PA1<--->白线（B）

3.B电机
PB1<--->BIN1
PB0<--->BIN2

4.B编码器
PD13<--->绿色（A）
PD12<--->白色（B）

5.舵机
PE9<--->橙色

6.红外
[]<--->R4
[]<--->R3
PD8<--->R2(黑)
PD9<--->R1(棕)
PD10<--->L1(红)
PD11<--->L2(橙)
[]<--->L3
[PD13]<--->L4
提示：在维持直行时，那一边的灯灭了就向那一边修