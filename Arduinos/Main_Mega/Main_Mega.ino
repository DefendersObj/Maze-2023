
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
  /*Procura o marcador de inicio de pacote 'i'*/
  if (Serial2.available() && Serial2.read() == 'i') {
    char buff[32];
    byte buff_control = 0;
    byte data_count = 0;

    while (1) {
      if (Serial2.available()) {
        /*Salva dados recebidos do Arduino no Buffer*/
        buff[buff_control] = Serial2.read();
        if (buff[buff_control] == 'p') break; // 'p' marca o fim de um pacote
        if(buff[buff_control] == '\n') data_count++; // '\n' marca o fim de um numero
        if (buff[buff_control] == 'i') {buff_control = 0; data_count = 0;} // Caso receba 'i' durante um pacote reiniciamos
        buff_control++;
        if (buff_control >= 30)break; // Limite do buffer
      }
    }

    if (buff_control < 30 && data_count == 4) {
      Serial2.print('k'); // Echo enviado ao Due
      buff_control = 0;
      //Concatenamos os chars recebidos em strings
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
    Serial.println("-----------------");
    //Envio para os motores
    motores.potencia(potencias);
    
  }
}


}
