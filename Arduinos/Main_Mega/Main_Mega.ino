
//   _____       __                _
//  |  __ \     / _|              | |
//  | |  | | ___| |_ ___ _ __   __| | ___ _ __ ___
//  | |  | |/ _ \  _/ _ \ '_ \ / _` |/ _ \ '__/ __|
//  | |__| |  __/ ||  __/ | | | (_| |  __/ |  \__ \
//  |_____/ \___|_| \___|_| |_|\__,_|\___|_|  |___/
//

/*!< Incluindo classes  */
#include "Motor.hpp"

Motor motores;

int potencias[4];  // Array to store the received numbers


void setup() {

  Serial.begin(250000);
  Serial2.begin(115200);
  Serial.println("Iniciei MEGA");
  motores.begin();
}


void loop() {

  recebe_serial();
}


/*!Recebe valores dos motores atraves da porta Serial*/
void recebe_serial() {
  if (Serial2.available() && Serial2.read() == 'i') {
    char buff[32];
    byte buff_control = 0;
    byte data_count = 0;

    while (1) {
      if (Serial2.available()) {
        buff[buff_control] = Serial2.read();
        if (buff[buff_control] == 'p') break;
        if(buff[buff_control] == '\n') data_count++;
        if (buff[buff_control] == 'i') {buff_control = 0; data_count = 0;}
        buff_control++;
        if (buff_control >= 30)break;
      }
    }

    if (buff_control < 30 && data_count == 4) {
      Serial2.print('k');
      buff_control = 0;
      for (int i = 0; i < 4; i++){

        String recebe = "";


      while (buff[buff_control] != '\n') {

        recebe.concat(buff[buff_control]);
        buff_control++;
      }
      buff_control++;
      potencias[i] = recebe.toInt();
      Serial.println(potencias[i]);
    }
    motores.potencia(potencias);
    Serial.println("-----------------");
  }
}










/*while(Serial2.available()){
    if(Serial2.read() == 'i')break;
  }
  if (Serial2.available()) {
    

    
   
      for (int i = 0; i < 4; i++) {

        String recebe = "";
        while(!Serial2.available());
        //Le cada caracter rcebido e concatena eles em uma String
        while (Serial2.available()) {
          char a = Serial2.read();
          if (a == 'i') i = 0;
          //Serial.println(a);
          //Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
          if (a == '\n') break;
          recebe.concat(a);
          if(!Serial2.available())delay();
        }
        potencias[i] = recebe.toInt();
        Serial.println(potencias[i]);
      }
      Serial.println("-----------------");
    }*/
}
