# Untitled - By: OBR - seg mai 15 2023

import sensor, image, time
import pyb
uart = pyb.UART(3, 19200) # Serial portas p4 e p5, baudrate de 19200
# LIGA LED DA CAMERA
pyb.LED(1).on()
pyb.LED(2).on()
pyb.LED(3).on()

#INICIALIZA A CAMERA
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((160,160))
sensor.skip_frames(64)
sensor.set_framerate(10)

#thresholds para detecção de cores e letras
threshold_array = [(0, 31, 52, -34, -128, 127),(0, 100, 51, 117, 27, 127), (0, 100, -64, 127, 47, 127), (0, 100, -128, -31, 10, 127)]
t = False
clock = time.clock()
while(True):
    #if (uart.any()):{
        serial = uart.read() # seguir forma #[comando][parede]

        clock.tick()
        img = sensor.snapshot()
        #detections = img.find_blobs(threshold_array, area_thresholds = 250, merge=True)
        #maxAreaBlob = max(detections)
        for detect in img.find_blobs(threshold_array, area_thresholds = 9216, merge=True):
            if 200 < detect.pixels() <= 9216:
                cropped_image = img.copy(roi = (detect.x(), detect.y(), detect.w(), detect.h()))
                cropped_image.to_grayscale()

                #pyb.delay(5000)
                img.to_grayscale()


                if not t:
                    cropped_image.save("./templates/SM.bmp")
                    t = True

                temp = image.Image("./templates/HM.bmp")
                temp2 = image.Image("./templates/HL.bmp")
                temp3 =  image.Image("./templates/HR.bmp")
                temp.to_grayscale()
                if img.find_template(temp, 0.7, search = image.SEARCH_EX) or img.find_template(temp2, 0.7, search = image.SEARCH_EX) or img.find_template(temp3, 0.7, search = image.SEARCH_EX) :
                    print("H")
                img.draw_rectangle(detect.x(), detect.y(), detect.w(), detect.h(), (255, 0, 255), 2)
                #img.draw_rectangle(detect.rect())
                print(f"X:{detect.cx()}\nY:{detect.cy()}\nArea: {detect.pixels()}")
