


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

  teste();
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
  sensores.passos_cm = sensores.passos * 0.78530;  //0.78530;
}

/*!Verifica o botão atraves de interrupção*/
void resetar() {
  Serial.println("APERTOU");
  reset = !reset;
  delay(1000);
}


void teste() {
  int sen[] = { 4 };
  int controle = 0;
  float leituras[10];
  sensores.zerar_encoder();
  unsigned int last_passo = sensores.passos;
  while (sensores.passos < 10) {
    op.movimento();
    if (last_passo != sensores.passos) {
      sensores.ler_dist_rapido(sen);
      leituras[controle] = sensores.dist[4];
      controle++;
      last_passo = sensores.passos;
    }
  }
  motores.parar();

  for (int x = 0; x < 10; x++) {
    Serial.println(leituras[x]);
  }
  float b1 = 0, b2 = 0, b3 = 0, b4 = 0;

  for (float x = 1; x <= 10; x++) {
    b1 += (x * 0.78530) * leituras[(int)x - 1];
    b2 += (x * 0.78530);
    b3 += leituras[(int)x - 1];
    b4 += pow((x * 0.78530), 2);
  }

  b1 *= 10;
  float b23 = b2 * b3;
  b4 *= 10;
  float bp = pow(b2, 2);

  float B = (b1 - b23) / (b4 - bp);
  float A = (b3 - B * b2) / 10;

  float angulo = (90 - (acos(B) * (180.0 / M_PI)));

  float dist = A + (B * (10 * 0.78530));
  float real_dist = cos(angulo / (180.0 / M_PI)) * dist;
  float lateral_dist = 7.0 - fmod(real_dist, 30.0);
  float h = sqrt(pow((10 * 0.78530), 2) - pow(lateral_dist, 2));
  float t = sqrt(pow(30 - h, 2) + pow(lateral_dist, 2));
  float angulo_trajetoria = asin(lateral_dist/t) * (180.0 / M_PI);

  angulo *= -1;

  float correcao = angulo + angulo_trajetoria;

  Serial.print("angulo: ");
  Serial.print(angulo);
  Serial.print(" dist: ");
  Serial.print(dist);
  Serial.print(" dist real: ");
  Serial.print(real_dist);
  Serial.print(" dist quadrado: ");
  Serial.print(lateral_dist);
  Serial.print(" trajetoria: ");
  Serial.print(t);
  Serial.print(" angulo trajetoria: ");
  Serial.println(angulo_trajetoria);




  op.girar(300, correcao);
  sensores.zerar_encoder();

  while(sensores.passos_cm < t){
    op.movimento(500);
  }
  motores.parar();

  op.girar(300, -angulo_trajetoria);
  




  while (1)
    ;
}