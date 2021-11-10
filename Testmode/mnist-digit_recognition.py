from fpioa_manager import *
import os, Maix, lcd, image, sensor, gc, time
from Maix import FPIOA, GPIO
import KPU as kpu
import gc

lcd.init(type=1,freq=15000000,width=240,height=240,color=(0,0,0))
lcd.rotation(3)
lcd.clear(0,0,0)

lcd.draw_string(20,20, "CocoRobo X", lcd.WHITE, lcd.BLACK)
time.sleep(1)
lcd.draw_string(20,40, "- Loading Camera...", lcd.WHITE, lcd.BLACK)

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224, 224))
sensor.set_hmirror(0)    #设置摄像头镜像
sensor.set_vflip(0)      #设置摄像头翻转

sensor.run(1)
sensor.skip_frames(30)

lcd.rotation(0)

# img_boot = image.Image("/sd/images/boot_digit.jpg")
# lcd.display(img_boot, oft=(0,0))
# time.sleep(2)
lcd.clear()
#task = kpu.load("/sd/model/mnist.kmodel")           #load model from flash address 0x200000
#task_mnist = kpu.load("/sd/mnist.kmodel")
task_mnist = kpu.load(0x300000)
sensor.run(1)
clock = time.clock()

while True:
    clock.tick()
    img_mnist1 = sensor.snapshot()
    img_mnist2=img_mnist1.resize(28,28)         #resize to mnist input 28x28
    a=img_mnist2.invert()                 #invert picture as mnist need
    a=img_mnist2.strech_char(1)           #preprocessing pictures, eliminate dark corner
    #lcd.display(img2,oft=(10,40))   #display small 28x28 picture
    a=img_mnist2.pix_to_ai()              #generate data for ai
    fmap_mnist=kpu.forward(task_mnist,img_mnist2)     #run neural network model
    plist_mnist=fmap_mnist[:]                   #get result (10 digit's probability)
    pmax_mnist=max(plist_mnist)                 #get max probability
    max_index_mnist=plist_mnist.index(pmax_mnist)     #get the digit
    print(str(max_index_mnist)+","+str(int(pmax_mnist*100)))
    img_mnist1.draw_rectangle(0,0,45,50,color=(0,0,0),fill=True)
    img_mnist1.draw_string(4,3,str(max_index_mnist),color=(255,255,255),scale=4)
    img_mnist1.draw_string(4,50,str(clock.fps()),color=(255,255,255),scale=4)
    lcd.display(img_mnist1,oft=(8,8))        #display large picture
    # lcd.draw_string(8,8,"%d: %.3f"%(max_index,pmax),lcd.WHITE,lcd.BLACK)
    print(clock.fps())
    gc.collect()
kpu.deinit(task)
