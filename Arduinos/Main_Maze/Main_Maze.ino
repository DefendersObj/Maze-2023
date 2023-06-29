


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
#include "Cor.hpp"

#define ENCODER_CONSTANT 0.78530



//Comunicacao com_;
Mapa mapa_;
Navegacao navegacao;
Cor color;
//Sensores sensores;

/*!<****** Declaração de todas variaveis ********/
volatile bool reset = true;  //false; //Váriavel volatil para o estado do botão

/*!<******* Protótipo das Funções ********/
char comando_manual();

/*!< ************** Setup do Código *******************/
void setup() {

  Serial.begin(250000);
  Serial2.begin(115200);
  Serial3.begin(115200);

  Serial.println("Iniciei");
  /*!< Inicializacoes nescessarias >!*/
  op.iniciar();
  /*Encoder*/
  pinMode(5, INPUT);
  attachInterrupt(digitalPinToInterrupt(5), ler_encoder, CHANGE);
  Serial.println("1");
  /*Botão*/
  Serial.println("2");
  //pinMode(12, INPUT);
  attachInterrupt(digitalPinToInterrupt(12), resetar, FALLING);
  Serial.println("3");
}

/************ Inicio do Loop *************/
void loop() {

  Serial.println(".");

  /*LEDs Ligados enquanto o robo espera ser iniciado pelo botão*/
  op.ligaLED_sinal();
  op.ligaLED_resgate();

  /*Só inicia caso o botão seja pressionado*/
  if (reset) {

    /*Desliga o LED de resgate para a rodada*/
    op.desligaLED_resgate();

    //Teste

    /*Loop de execução do código*/
    while (reset == true) {  //Caso  botão de reset seja pressionado, sai do loop
      Serial.println("1");
      //Atualiza o mapa
      mapa.get_info(navegacao.X, navegacao.Y);
      if (mapa.Color == 'u')
        navegacao.update_map();
      Serial.println("2");
      char com = 0;

      while (com != 'F' && com != 'L') {
        Serial.println("3");
        com = navegacao.decisao();
        ler_comando(com);
        if (com == 'L') {
          navegacao.last_node();
          navegacao.calc_route(navegacao.X, navegacao.Y);
          navegacao.dump_stack();
          uint16_t string_control = 0;
          while (navegacao._commands[string_control] != '\0') {
            ler_comando(navegacao._commands[string_control]);
            string_control++;
          }
        }
      }
    }
    //Código de reset do mapa
  }
}

/*!<******** Declaração das Funções *********/

/*! Recebe o comando e orientacao pela porta Serial*/
char comando_manual() {

  char input;
  while (!Serial.available()) {
    // Wait for character input
  }

  input = Serial.read();

  return input;
}

/*Lê o comando recebido pela decisão*/
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
  sensores.passos_cm = sensores.passos * ENCODER_CONSTANT;  //0.78530;
}

/*!Verifica o botão atraves de interrupção*/
void resetar() {
  Serial.println("APERTOU");
  reset = !reset;
  delay(1000);
}

