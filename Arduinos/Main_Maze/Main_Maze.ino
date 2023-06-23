


//   _____       __                _
//  |  __ \     / _|              | |
//  | |  | | ___| |_ ___ _ __   __| | ___ _ __ ___
//  | |  | |/ _ \  _/ _ \ '_ \ / _` |/ _ \ '__/ __|
//  | |__| |  __/ ||  __/ | | | (_| |  __/ |  \__ \
//  |_____/ \___|_| \___|_| |_|\__,_|\___|_|  |___/
//

/*!< Incluindo classes  */
#include "Operacional.hpp"
#include "Estrategia.hpp"
#include "Mapa.hpp"
#include "Comunicacao.hpp"
#include "Sensores_Novo.hpp"
#include <Servo.h>

//Comunicacao com_;
Mapa mapa_;
Estrategia estrategia;
Servo servo_frontal;
//Sensores sensores;

/*!<****** Declaração de todas variaveis ********/

/*!<******* Protótipo das Funções ********/
char comando_manual();

/*!< ************** Setup do Código *******************/
void setup() {

  Serial.begin(250000);
  Serial3.begin(115200);
  /*!< Inicializacoes nescessarias >!*/
  pinMode(5, INPUT);
  attachInterrupt(digitalPinToInterrupt(5), ler_encoder, CHANGE);
  estrategia.iniciar();
  servo_frontal.attach(41);
  servo_frontal.write(36);
  Serial.println("Iniciei");
}

/****************** Inicio do Loop ********************/
void loop() {

  //Teste manual (Essas informacoes devem vir da Decisao)
  Serial.println("ME MANDA O COMANDO");
  char comando = comando_manual();
  int ori = 0;         //comando_manual();
  bool busca = false;  //comando_manual();



  //Frente
  if (comando == 'F') {
    estrategia.frente(ori, busca);
    Serial.println("Frente!!");
  }
  //Start
  else if (comando == 'S') {
    estrategia.iniciar();
  }
  //Agua
  else if (comando == 'P') {
  }

  //Giro para Esquerda
  else if (comando == 'E') {
    op.girar(500, -90.0);
  }

  //Giro para Esquerda
  else if (comando == 'D') {
    op.girar(500, 90.0);
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

/*! Le o encoder via interrupcao e converte ele para centimetros*/
void ler_encoder() {
  sensores.passos++;
  sensores.passos_cm = sensores.passos * 0.78530;
}