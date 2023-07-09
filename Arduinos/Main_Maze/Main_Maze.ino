


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

#define ENCODER_CONSTANT 0.78530

Mapa mapa_;
Navegacao navegacao;

/*!< ************** Setup do Código *******************/
void setup() {

  //delay(1000);
  Serial.begin(250000);
  Serial1.begin(38400);
  Serial2.begin(19200);
  Serial3.begin(115200);


  cores.begin();
  /*!< Inicializacoes nescessarias >!*/
  op.iniciar();
  /*Encoder*/
  pinMode(5, INPUT);
  attachInterrupt(digitalPinToInterrupt(5), ler_encoder, CHANGE);

  Serial.println("Terminei Setup");
}

/************ Inicio do Loop *************/
void loop() {

  while (1) resgate(3);

  /*LEDs Ligados enquanto esperam o botão*/
  //while (1) com.conversation(3);
  op.ligaLED_sinal();
  op.ligaLED_resgate();
  delay(3000);

  //Desliga o LED de resgate para a rodada
  op.desligaLED_resgate();

  /*Loop de execução do código*/
  while (1) {

    Serial.println("Entrei no Loop");

    //Lista of pre recorded movements
    //From Silver to Tile 1
    comando('F', false);
    comando('D', false);
    comando('F', false);
    comando('D', false);
    comando('F', true);
    com.conversation(op._kits);
    op.resgate(com._recebido);

    //From Tile 1 to Tile 2
    comando('T', false);
    comando('F', false);
    comando('F', false);
    comando('F', true);
    com.conversation(op._kits);
    op.resgate(com._recebido);

    //From Tile 2 to Tile 3
    comando('T', false);
    comando('F', false);
    comando('D', false);
    comando('F', false);
    comando('F', false);
    comando('D', false);
    comando('F', true);
    com.conversation(op._kits);
    op.resgate(com._recebido);

    //From Tile 3 to Tile 4
    comando('T', false);
    comando('F', false);
    comando('F', false);
    comando('F', true);
    com.conversation(op._kits);
    op.resgate(com._recebido);


    //From Tile 4 to Silver
    comando('T', false);
    comando('F', false);
    comando('D', false);
    comando('F', false);
    comando('D', false);
    comando('F', false);
    com.conversation(op._kits);
    op.resgate(com._recebido);

    //Ends the run
    motores.parar();
    while (1) op.LED_resgate();
    motores.parar();
    //delay(9999999);
  }
}


/*!<******** Declaração das Funções ********/

//Regra da mão direita
void escolhe_lado() {
  sensores.ler_dist();
  if ((sensores.dist[0] + sensores.dist[1]) / 2) {
  }
}

/*Lê o comando recebido pela decisão*/
void comando(char com, bool busca) {

  //Reset do botão
  //if (flag_reset) return;

  //Movimento de 1 quadrado para frente
  if (com == 'F') {
    Serial.println("Vamos para Frente!!");
    delay(300);
    if (busca == true) op.buscar_vit();
    op.frente();
    //if (op._kits != 9) op.resgate(op._kits, op._lado);  //Se houver vítimas resgata
  }

  //Giro para Esquerda
  else if (com == 'E') {
    op.girar(500, -90.0);
    Serial.println("Esquerdo!!");
  }

  //Giro para Direita
  else if (com == 'D') {
    op.girar(500, 90.0);
    Serial.println("Direita!!");
  }


  //Giro para Tras
  else if (com == 'T') {
    op.girar(500, 180.0);
    Serial.println("Trás!!");
  }
}

/*! Le o encoder via interrupcao e converte ele para centimetros*/
void ler_encoder() {
  sensores.passos++;
  sensores.passos_cm = sensores.passos * ENCODER_CONSTANT;  //0.78530;
}