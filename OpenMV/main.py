# main - By: Arthur - qui jun 29 2023

import sensor, image, time, pyb

# Liga os leds
pyb.LED(1).on()
pyb.LED(2).on()
pyb.LED(3).on()

# Inicia a camera
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_auto_exposure(False)
sensor.set_framerate(60)
sensor.set_windowing((240,240))
sensor.set_auto_gain(True)
sensor.skip_frames(100)

# Inicia a serial
uart = pyb.UART(3, 115200) # P4 TX e P5 RX baudrate 115200



# Thresholds e templates para detecção

threshold_list = [
                  (0, 31, 52, -34, -128, 127), #  Letra
                  (9, 34, -128, -21, -1, 127), # Verde
                  (56, 100, -26, 127, 48, 127), #   Amarelo
                  (0, 100, 4, 127, 63, 25) #    Vermelho
                  ]

#H:
HL = image.Image("./templates/HL.bmp")
HM = image.Image("./templates/HM.bmp")
HR = image.Image("./templates/HR.bmp")

#S:
SL = image.Image("./templates/SL.bmp")
SM = image.Image("./templates/SM.bmp")
SR = image.Image("./templates/SR.bmp")

#U:
UL = image.Image("./templates/UL.bmp")
UM = image.Image("./templates/UM.bmp")
UR = image.Image("./templates/UR.bmp")


# Funções

def send_kits(nKits, uart) -> None:
    uart.write(nKits)

def receive_side(uart) -> str:
    try:
        side = uart.read().decode('ascii')
        print(side[0])
        return side[0]
    except:
        side = uart.read()
        #print(side)
        return side

def detection_letter(img, side) -> str:
    img.to_grayscale()
    if side == 'L':
        if img.find_template(HL, 0.7, search = image.SEARCH_DS):
            img.to_rgb565()
            print("[H]") # Para debug
            return '3'
        elif img.find_template(SL, 0.7, search = image.SEARCH_DS):
            img.to_rgb565()
            print("[S]") # Para debug
            return '2'
        elif img.find_template(UL, 0.7, search = image.SEARCH_DS):
            img.to_rgb565()
            print("[U]") # Para debug
            return '0'
        else:
            print("Detectei nada") # Para debug
            return '9'
    elif side == 'M':
        if img.find_template(HM, 0.7, search = image.SEARCH_DS):
            img.to_rgb565()
            print("[H]") # Para debug
            return '3'
        elif img.find_template(SM, 0.7, search = image.SEARCH_DS):
            img.to_rgb565()
            print("[S]") # Para debug
            return '2'
        elif img.find_template(UM, 0.7, search = image.SEARCH_DS):
            img.to_rgb565()
            print("[U]") # Para debug
            return '0'
        else:
            img.to_rgb565()
            print("Detectei nada") # Para debug
            return '9'

    elif side == 'R':
        if img.find_template(HR, 0.7, search = image.SEARCH_DS):
            print("[H]") # Para debug
            return '3'
        elif img.find_template(SR, 0.7, search = image.SEARCH_DS):
            print("[S]") # Para debug
            return '2'
        elif img.find_template(UR, 0.7, search = image.SEARCH_DS):
            print("[U]") # Para debug
            return '0'
        else:
            print("Detectei nada")
            return '9'

def max_key(initial_key, victim_counter) -> str:
    max_key = initial_key
    for key in list(victim_counter.keys()):
        if victim_counter[key] > victim_counter[max_key]:
            max_key = key
    return str(max_key)
while(True):
    victim_counter = {'0': 0, '1': 0, '2': 0, '3': 0, '9': 0}
    i = 0
    img = sensor.snapshot()
    if uart.any():
        side = receive_side(uart)
        print("[DO ARDUINO] Lado: " + side)
        while i < 50:
            for detection in img.find_blobs(threshold_list, merge = True):

                if detection.area() > 2000 and detection.code() == 1:
                    print("BURACO") #Discutir com Gabriel

                if detection.code() == 2:
                    victim_counter['0'] = victim_counter['0'] + 1
                    print('[DA OPENMV] Verde')

                elif detection.code() == 4:
                    victim_counter['1'] = victim_counter['1'] + 1
                    print("[DA OPENMV] Amarelo")

                elif detection.code() == 8:
                    victim_counter['1'] = victim_counter['1'] + 1
                    print("[DA OPENMV] Vermelho")

                elif detection.code() == 1:
                    nKits = detection_letter(img, side)
                    victim_counter[nKits] = victim_counter[nKits] + 1
                img.draw_rectangle(detection.rect())
            i = i + 1
            if i == 50:
                max_detections = max_key('0', victim_counter)
                print(max_detections, victim_counter[max_detections])
                uart.write(max_detections)
