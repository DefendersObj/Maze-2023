


//   _____       __                _
//  |  __ \     / _|              | |
//  | |  | | ___| |_ ___ _ __   __| | ___ _ __ ___
//  | |  | |/ _ \  _/ _ \ '_ \ / _` |/ _ \ '__/ __|
//  | |__| |  __/ ||  __/ | | | (_| |  __/ |  \__ \
//  |_____/ \___|_| \___|_| |_|\__,_|\___|_|  |___/
//

/*!< Incluindo classes  */
#include "Operacional.hpp"
#include "Navegacao.hpp"
#include "Mapa.hpp"
#include "Comunicacao.hpp"
#include "Sensores_Novo.hpp"
#include <Servo.h>

//Comunicacao com_;
Mapa mapa_;
Navegacao navegacao;
//Sensores sensores;

/*!<****** Declaração de todas variaveis ********/

/*!<******* Protótipo das Funções ********/
char comando_manual();

/*!< ************** Setup do Código *******************/
void setup() {

  Serial.begin(250000);
  Serial2.begin(115200);
  Serial3.begin(115200);

  /*!< Inicializacoes nescessarias >!*/
  pinMode(5, INPUT);
  attachInterrupt(digitalPinToInterrupt(5), ler_encoder, CHANGE);
  op.iniciar();
  Serial.println("Iniciei");

}

/****************** Inicio do Loop ********************/
void loop() {
  navegacao.update_map();

  char com = 0;

  while(com != 'F'){
    com = navegacao.decisao();
    ler_comando(com);
  }
  
 
}

/*!<********** Declaração das Funções ***********/

/*! Recebe o comando e orientacao pela porta Serial*/
char comando_manual() {

  char input;
  while (!Serial.available()) {
    // Wait for character input
  }

  input = Serial.read();

  return input;
}

void ler_comando(char com) {
  if (com == 'F') {
    delay(300);
    op.frente();
    Serial.println("Frente!!");
  }
  //Start
  else if (com == 'S') {
    op.iniciar();
  }
  //Agua
  else if (com == 'P') {
  }

  //Giro para Esquerda
  else if (com == 'E') {
    op.girar(500, -90.0);
  }

  //Giro para Direita
  else if (com == 'D') {
    op.girar(500, 90.0);
  }


  //Giro para Tras
  else if (com == 'T') {
    op.girar(500, 180.0);
  }
}

/*! Le o encoder via interrupcao e converte ele para centimetros*/
void ler_encoder() {
  sensores.passos++;
  sensores.passos_cm = sensores.passos * 0.78530;
}