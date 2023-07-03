


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
volatile bool reset = false;  //false; //Váriavel volatil para o estado do botão
volatile bool flag_reset = true;

/*!<******* Protótipo das Funções ********/
char comando_manual();

/*!< ************** Setup do Código *******************/
void setup() {

  delay(1000);
  Serial.begin(250000);
  Serial2.begin(115200);
  Serial3.begin(115200);
  color.begin();
  /*!< Inicializacoes nescessarias >!*/
  op.iniciar();
  /*Encoder*/
  pinMode(5, INPUT);
  attachInterrupt(digitalPinToInterrupt(5), ler_encoder, CHANGE);
  /*Botão*/
  pinMode(12, INPUT);
  attachInterrupt(digitalPinToInterrupt(12), resetar, FALLING);
  reset = false;
  Serial.println("3");
}

/************ Inicio do Loop *************/
void loop() {

  /*LEDs Ligados enquanto espera o botão*/
  op.ligaLED_sinal();
  op.ligaLED_resgate();

  /*Inicia se o botão for pressionado*/
  if (reset) {
    if (flag_reset == true) {
      flag_reset = false;
      delay(1000);
    }

    //Desliga o LED de resgate para a rodada
    op.desligaLED_resgate();

    /*Loop de execução do código*/
    while (reset == true) {  //Caso  botão de reset seja pressionado, sai do loop

      //Atualiza o mapa caso o quadrado nao tenha sido visitado
      mapa.get_info(navegacao.X, navegacao.Y);
      if (mapa.Color == 'u')
        navegacao.update_map();
      ;

      char com = 0;

      while (com != 'F' && com != 'L') {

        /*Estágio 1*/
        com = navegacao.decisao();
        /*Todo avanço de quadrado dentro do estagio 1 deve ter busca por vítimas*/
        ler_comando(com, true);

        //So entra se nao houverem caminhos novos
        if (com == 'L') {

          /*Estágio 2: Busca pelo ultimo no valido*/
          navegacao.last_node();

          /*Estágio 3: Calcula rota para ultimo no*/
          navegacao.calc_route(navegacao.X, navegacao.Y);
          navegacao.dump_stack();  //Traduz cordenadas para comandos
          uint16_t string_control = 0;
          while (navegacao._commands[string_control] != '\0') {
            ler_comando(navegacao._commands[string_control], false);
            string_control++;
          }
        }
      }
    }
  }
  /*Limpa o mapa não consolidado e atualiza as cordenadas, ORIENTAÇÃO VOLTA PARA ZERO!!!*/
  navegacao.falha_de_progresso();
}

/*!<******** Declaração das Funções ********/

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
void ler_comando(char com, bool busca) {
  
  //Movimento de 1 quadrado para frente
  if (com == 'F') {
    delay(300);
    if (busca == true) op.buscar_vit();
    op.frente();
    op.resgate(op._kits, op._lado);
    Serial.println("Frente!!");
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
  reset = !reset;
  flag_reset = true;
  if (reset == false) {
    op.ligaLED_resgate();
  } else op.desligaLED_resgate();
}
