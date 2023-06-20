#ifndef Operacional_hpp
#define Operacional_hpp

/*A classe Operacional tem como funcao juntar e abstrair, as leituras dos sensores,
e o funcionamento dos Atuadores.
As funcoes dessa classe ainda nao sao as ideias para utilizicacao no codigo main*/

/*!< Incluindo classes */
//#include "Motor_Novo.hpp"
#include "Motor.hpp"
#include "Sensores_Novo.hpp"
#include "PID.hpp"

Motor motores;
Sensores sensores;

#define CIRCUNFERENCIA_RODA 20
#define COMPRIMENTO_ROBO 8.5
#define NUM_PASSOS 10


/*! Constroi os PIDs*/
//KP, KI, KD, Setpoint, windup, limite
PID pidG(60.0, 0.1, 2.0, 0, 10, 50);     //Giroscopio
PID pidF(0.5, 0.2, 0.1, 100, 100, 100);  //Frontal


class Operacional {

private:

  int
    quadrado_ant[2];

  bool
    ultimo_passo,  //Encoder
    passo;

public:

  /*! Inicializacao dos Motores e Sensores*/
  void begin() {
    //sensores.begin_enconder();
    sensores.begin_mpu();
  }
  /**************** CORES *****************/
  char cor() {
    //Le o sensor
    char cor = 'w';
    return cor;
  }

  /**************** PIDS ******************/

  /*! PID para diagonal */
  int PID_diagonal() {
    int aux = pidG.calcular(sensores.angulo_mpu());
    return aux;
  }

  /*! Setar PID para andar em diagonal*/
  void setar_PID_diagonal(int setpoint = 0) {
    pidG.setSetpoint(setpoint);
  }
  /******************** ANGULO **********************/
  void zerar_mpu() {
    sensores.zerar_mpu();
  }
  /******************** DISTANCIAS **********************/
  float dist[6];
  float correction_angle = 0;
  float trajetoria = 30.0;
  bool passagens[4];


  /*! Lê as distancias dos 6 ultrassonicos*/
  void ler_distancias() {
    sensores.ler_dist();
    for (int i = 0; i < 6; i++) {
      dist[i] = sensores.dist[i];
      Serial.print("Dist ");
      Serial.print(i);
      Serial.print(":");
      Serial.println(dist[i]);
    }
    Serial.println("--------------");
  }

  /*! Verifica a existencia de passagens nas quatro direcoes*/
  void medir_passagens() {
    if (dist[0] >= 25) { passagens[0] = true; }
    if (dist[1] + dist[2] / 2 >= 25) { passagens[1] = true; }
    if (dist[3] >= 25) { passagens[2] = true; }
    if (dist[4] + dist[5] / 2 >= 25) { passagens[3] = true; }
  }

  /*! Estima o angulo atual com base em dois valores de distancia*/
  float angulo() {

    ler_distancias();
    float df = dist[1],
          dt = dist[2],
          et = dist[4],
          ef = dist[5];
    float angulo,
      cat_op,
      frente,
      tras;

    bool lado = false;

    //Decidimos qual lado do robo e o mais proximo para medir o angulo
    if (ef + et <= df + dt) {
      frente = ef;
      tras = et;
      lado = true;
    } else {
      frente = df;
      tras = dt;
    }


    if (frente > tras) {
      cat_op = frente - tras;
    } else {
      cat_op = tras - frente;
    }

    angulo = atan(cat_op / COMPRIMENTO_ROBO) * (180.0 / M_PI);

    //Correcao dependendo da angulacao
    if (frente < tras && lado == true) {
      angulo = angulo * -1.0;
    } else if (frente > tras && lado == false) {
      angulo = angulo * -1.0;
    }

    Serial.print("Angulo: ");
    Serial.println(angulo);
    Serial.print("Cateto Oposto: ");
    Serial.println(cat_op);
    return angulo;
  }



  /********************************************************************* MOVIMENTACAO ********************************************************************/

  /**************************************** BÁSICAS ****************************************/

  /*! Girar o Robo */
  void girar(char com) {
    int aux[] = { -500, -500, -500, -500 };  // Inicia com valores de 'E'
    sensores.zerar_mpu();

    //Esquerda
    if (com == 'E') {
      while (sensores.angulo_mpu() <= 90) {
        motores.potencia(aux);
      }
      //Direita
    } else if (com == 'D') {
      aux[0] = 500;
      aux[1] = 500;
      aux[2] = 500;
      aux[3] = 500;
      while (sensores.angulo_mpu() >= -90) {
        motores.potencia(aux);
        //sensores.calibrar_offset();
      }
    }
    parar();
    sensores.zerar_mpu();
  }

  /*! Para todos motores*/
  void parar() {
    motores.mesma_potencia(0);
  }

  /*! Volta de re quando entramos em um quadrado preto*/
  void sair_preto() {
    motores.mesma_potencia(-300);
  }

  /*! Espera 5 seg no azul*/
  void espera_azul() {
    parar();
    delay(5000);
  }

  /*! Movimenta o robo para frente*/
  void movimento(int velocidade = 500) {
    int diferenca_lateral = pidG.calcular(sensores.angulo_mpu());
    motores.mesma_potencia(velocidade, diferenca_lateral);
  }

  /****************************************** TROCA DE QUADRADO *********************************************/

  /*! São definidos os parametros de distancia e do Encoder, para a troca*/
  void setar_quadrado(int frente, int tras) {
    quadrado_ant[0] = frente;
    quadrado_ant[1] = tras;
    sensores.zerar_encoder();
  }



  /*Um dos parametros da troca*/
  bool troca_encoder() {

    //Checa se foram passos suficientes
    if (sensores.passos_cm >= trajetoria) {
      sensores.zerar_encoder();
      return true;
    } else {
      return false;
    }
  }

  /* Verifica a troca de quadrado */
  bool troca_quadrado(int f_atual, int t_atual) {
    if (f_atual <= quadrado_ant[0] - 35 || t_atual >= quadrado_ant[1] + 35) {  // Troca pelas distancias
      return true;
      //} else if (troca_encoder() == true) {  // Troca pelo Encoder
      //return true;
    } else {  // Nao houve troca
      return false;
    }
  }

  /************************************ CORREÇÃO *********************************************/

  /*Correcao do angulo do robo a cada parada*/
  void correcao() {
    float ang = angulo();
    //Parte 1 alinha o robo com a parede
    int aux[] = { -200, -200, -200, -200 };  // Inicia com valores para esquerda
    sensores.zerar_mpu();

    //Ajuste para esquerda
    if (ang >= 2.0 && ang <= 20.0) {
      while (sensores.angulo_mpu() < ang) {
        motores.potencia(aux);
      }
    }
    //Ajuste para direita
    else if (ang <= -2.0 && ang >= -20.0) {
      aux[0] = 200;
      aux[1] = 200;
      aux[2] = 200;
      aux[3] = 200;
      while (sensores.angulo_mpu() > ang) {
        motores.potencia(aux);
      }
    }
    //Caso de saída
    else {
    }
    parar();
    sensores.zerar_mpu();
  }

  /* */
  void correcao_trajetoria() {
    int aux[] = { -200, -200, -200, -200 };  // Inicia com valores para esquerda

    sensores.zerar_mpu();


    if (correction_angle < 0) {  //Direita
      while (sensores.angulo_mpu() <= -correction_angle) {
        motores.potencia(aux);
      }
    }

    else if (correction_angle > 0) {  //Eesquerda
      aux[0] = 200;
      aux[1] = 200;
      aux[2] = 200;
      aux[3] = 200;

      while (sensores.angulo_mpu() >= -correction_angle) {
        motores.potencia(aux);
      }
    }
    parar();
    sensores.zerar_mpu();
  }

  /*Cálculo do comprimento da nova trajetória */
  void calcular_trajetoria() {

    float cateto;
    trajetoria = 30.0;

    //Escolhemos o lado mais proximo e verificamos se ele é coerente
    if (dist[1] <= dist[5]) {  //Próximo da esquerda
      cateto = -(7 - fmod(dist[1], 30.0));
    }

    else if (dist[5] <= dist[1]) {  //Próximo da direita
      cateto = (7 - fmod(dist[5], 30.0));
    }
    //Caso nao nescessite de correcao
    else {
      return;
    }

    //Nova distancia a ser percorrida (Deve ser passada para a troca)
    trajetoria = sqrt(pow(cateto, 2) + pow(30.0, 2));  // Calculate the hypotenuse
    correction_angle = atan(cateto / 30.0) * (180.0 / M_PI);
    Serial.print("cateto: ");
    Serial.println(cateto);
    Serial.print("Trajetoria: ");
    Serial.println(trajetoria);
    Serial.print("Angulo estimado: ");
    Serial.println(correction_angle);
  }
};
#endif