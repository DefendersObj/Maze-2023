


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

//Comunicacao com_;
Mapa mapa_;
Navegacao navegacao;
//Sensores sensores;

/*!<****** Declaração de todas variaveis ********/
//volatile bool reset = true;  //false; //Váriavel volatil para o estado do botão
//bool flag_reset = false;

/*!<******* Protótipo das Funções ********/
char comando_manual();

/*!< ************** Setup do Código *******************/
void setup() {

  delay(1000);
  Serial.begin(250000);
  Serial2.begin(19200);
  Serial3.begin(115200);
  cores.begin();
  /*!< Inicializacoes nescessarias >!*/
  op.iniciar();
  /*Encoder*/
  pinMode(5, INPUT);
  attachInterrupt(digitalPinToInterrupt(5), ler_encoder, CHANGE);
  /*Botão*/
  //pinMode(12, INPUT);
  //attachInterrupt(digitalPinToInterrupt(12), resetar, FALLING);
  //reset = false;

  //Iniciio forcado dos motores
  //while(tempo() <= 60) motores.;
  Serial.println("Terminei Setup");
}

/************ Inicio do Loop *************/
void loop() {
  //while (1) {
  //cores.calibrar_todos();
  //}
  //while (1) Serial.println(cores.buscar());

  //while (1) sensores.inclinacao_mpu();
  //while(1)cores.calibrar_todos();
  //while (1) {
  // Serial.println(cores.buscar());
  // Serial.println("------------------");
  //}

  /*LEDs Ligados enquanto esperam o botão*/
  op.ligaLED_sinal();
  op.ligaLED_resgate();
  delay(5000);

  //Serial.println(".");

  /*Inicia se o botão for pressionado*/
  //if (reset) {
  //if (flag_reset == true) {
  //flag_reset = false;
  //delay(1000);
  // }

  //Desliga o LED de resgate para a rodada
  op.desligaLED_resgate();

  /*Loop de execução do código*/
  while (1) {  //Caso  botão de reset seja pressionado, sai do loop

    Serial.println("Entrei no Loop");

    //Atualiza o mapa caso o quadrado nao tenha sido visitado
    mapa.get_info(navegacao.X, navegacao.Y);
    if (mapa.Color == 'u')
      navegacao.update_map();
    Serial.println("Atualizei o mapa");
    ;

    char com = 0;

    while (com != 'F' && com != 'L') {

      /*Estágio 1*/
      com = navegacao.decisao();
      Serial.println("Decisão estagio 1");
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
//motores.parar();
//navegacao.falha_de_progresso();
//}

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

  //Reset do botão
  //if (flag_reset) return;

  //Movimento de 1 quadrado para frente
  if (com == 'F') {
    Serial.println("Vamos para Frente!!");
    delay(300);
    if (busca == true) op.buscar_vit();
    op.frente();
    if (op._kits != 9) op.resgate(op._kits, op._lado);  //Se houver vítimas resgata
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

/*!Verifica o botão atraves de interrupção*/
//void resetar() {
//reset = !reset;
//flag_reset = true;
//if (reset == false) {
//op.ligaLED_resgate();
//} else op.desligaLED_resgate();
//}
