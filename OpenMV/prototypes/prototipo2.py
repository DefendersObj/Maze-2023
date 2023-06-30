# Main - By: Arthur - seg mai 29 2023

import sensor, image, time
import pyb
uart = pyb.UART(3, 19200) #portas p4 e p5 da camera (P4 = TX, P5 = RX), baudrate 19200

#LIGA LED DA CAMERA NA COR BRANCA
pyb.LED(1).on()
pyb.LED(2).on()
pyb.LED(3).on()


#INICIA A CAMERA
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_auto_exposure(False)
sensor.set_windowing((160,160))
sensor.set_auto_gain(True)
sensor.skip_frames(100)

#INTERVALO DE CORES PARA DETECTAR LETRA, VERDE, AMARELO, VERMELHO
threshold_list = [
                  (0, 31, 52, -34, -128, 127), #  Letra
                  (0, 100, -24, -11, -51, 127), # Verde
                  (0, 100, 15, 127, 46, 127), #   Amarelo
                  (0, 100, 26, 127, 18, 127) #    Vermelho
                  ]

#CARREGA OS TEMPLATES DAS LETRAS:
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


while(True):
    i = 0
    if uart.any(): # verifica se a serial tem alguma mensagem. Se tiver entao executa codigo de detecção uart.any()
        serial = uart.read(nbytes = 1) # comando: [lado] (inicial do lado em ingles)
        # loop para verificar 50 vezes o lado
        while i < 50:
            img = sensor.snapshot()
            for detection in img.find_blobs(threshold_list, area_threshold = 9216, merge = True):
                if 200 < detection.pixels() <= 9216:
                    if detection.code() == 1: # Verifica se é uma letra
                        img.to_grayscale()
                        if serial[0] == 'L':
                            if img.find_template(HL, 0.7, search = image.SEARCH_EX):
                                uart.write('3')
                                print('H')
                                break
                            elif img.find_template(SL, 0.7, search = image.SEARCH_EX):
                                uart.write('2')
                                print('S')
                                break
                            elif img.find_template(UL, 0.7, search = image.SEARCH_EX):
                                uart.write('0')
                                print('U')
                                break
                            else:
                                print('[DEBUG]')

                        elif serial[0] == 'M':
                            if img.find_template(HM, 0.7, search = image.SEARCH_EX):
                                uart.write('3')
                                print('H')
                                break
                            elif img.find_template(SM, 0.7, search = image.SEARCH_EX):
                                uart.write('2')
                                print('S')
                                break
                            elif img.find_template(UM, 0.7, search = image.SEARCH_EX):
                                uart.write('0')
                                print('U')
                                break
                            else:
                                print('[DEBUG]')
                        elif  serial[0] == 'R':
                            if img.find_template(HL, 0.7, search = image.SEARCH_EX):
                                uart.write('3')
                                print('H')
                                break
                            elif img.find_template(SL, 0.7, search = image.SEARCH_EX):
                                uart.write('2')
                                print('S')
                                detect_anything = True
                            elif img.find_template(UL, 0.7, search = image.SEARCH_EX):
                                uart.write('0')
                                print('U')
                                break
                            else:
                                print('[DEBUG]')
                    elif detection.code() == 2: # Se for verde retorna 0 kits
                        uart.write('0')
                        print("Verde")
                        break
                    else: # Se não for um letra ou verde retorna 1 kit
                        uart.write('1')
                        print("Vermelho ou amarelo")
                        break
            i=i+1
            if i==50:
                uart.write('4')

                img.draw_rectangle(detection.rect())


