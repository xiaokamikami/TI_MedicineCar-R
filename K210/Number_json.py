#M1W
import sensor, image, lcd, time
from Maix import GPIO
import KPU as kpu
from fpioa_manager import fm    # 导入库
import gc
import ujson
from machine import UART
import machine
from board import board_info
import ujson

#系统初始化
gc.collect()


#引脚定义
LED_G = 12
LED_R = 13
LED_B = 14
LED_Compensation = 16



fm.register(LED_G, fm.fpioa.GPIO0)
fm.register(LED_R, fm.fpioa.GPIO1)
fm.register(LED_B, fm.fpioa.GPIO2)

fm.register(LED_Compensation, fm.fpioa.GPIO3)


led_b=GPIO(GPIO.GPIO0, GPIO.OUT)
led_g=GPIO(GPIO.GPIO1, GPIO.OUT)
led_r=GPIO(GPIO.GPIO2, GPIO.OUT)

led_c=GPIO(GPIO.GPIO3, GPIO.OUT)

led_g.value(1)
led_b.value(1)
led_r.value(1)
led_c.value(1)

fm.register(15,fm.fpioa.UART2_TX)
fm.register(17,fm.fpioa.UART2_RX)
uart_stm32 = machine.UART(UART.UART2, 115200, 8, 1, 0, timeout=100, read_buf_len=128)


class_IDs = ['1', '2', '3', '4', '5', '6', '7', '8']


def drawConfidenceText(image, rol, classid, value):
    text = ""
    _confidence = int(value * 100)
    classid = classid+1
    text = str(classid)+':' + str(_confidence) + '%'
    image.draw_string(rol[0], rol[1], text , color=color_G, scale=2.5)




lcd.init()
sensor.reset(dual_buff=True)
#sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224,224))
sensor.set_hmirror(1)    #设置摄像头镜像
sensor.set_vflip(1)      #设置摄像头翻转
sensor.skip_frames(time = 200)     # Wait for settings take effect.
clock = time.clock()                # Create a clock object to track the FPS
#sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
#sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
sensor.run(1)


task = kpu.load(0x300000)

anchor = (0.733, 1.0402, 0.9391, 1.4141, 1.731, 3.3042, 1.7591, 3.8099, 1.9976, 4.6233)
#anchor = (1.3316, 2.3774, 1.4857, 2.6176, 1.6022, 2.8221, 1.8719, 3.375, 2.6094, 1.5146)
_ = kpu.init_yolo2(task, 0.5, 0.3, 5, anchor)
img_lcd = image.Image()

ball_dict = {}
color_G = (0, 255, 0)

while (True):
    clock.tick()                    # Update the FPS clock.

    img = sensor.snapshot()
    img.lens_corr(1.8)

    code = kpu.run_yolo2(task, img)
    if code:
        totalRes = len(code)

        for item in code:
            confidence = float(item.value())
            itemROL = item.rect()
            classID = int(item.classid())
            print_args = (item.x(), item.y(), classID)

            _ = img.draw_rectangle(itemROL, color_G, tickness=5)
            if totalRes == 1:
                drawConfidenceText(img, (0, 0), classID, confidence)
            _ = lcd.display(img)

            #ball_dict["x"]   = print_args[0]
            #ball_dict["v"]   = print_args[1]
            #ball_dict["n"]  = (print_args[2]+1)


            if(confidence > 0.6):
                #encoded = ujson.dumps(ball_dict)
                #uart_stm32.write(encoded+"\n")
                #print(encoded)
                encoded = str('['+str((print_args[2]+1))+','+str(print_args[0]+100)+']')
                uart_stm32.write(encoded+"\n")
                #uart_stm32.write('['+str((print_args[2]+1))+str(print_args[0]+100)+']'+"\n")
                #print(encoded)
                if(clock.fps()<10):
                    pass
                else:
                    time.sleep_ms(int(clock.fps()/10)*100)
                #print(clock.fps())





    else:
        led_g.value(1)
        led_r.value(1)
        led_b.value(1)
        _ = lcd.display(img)
    gc.collect()


uart_A.deinit()
del uart_A
_ = kpu.deinit(task)
