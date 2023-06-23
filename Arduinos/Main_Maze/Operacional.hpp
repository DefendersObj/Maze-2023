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
PID pidG(60.0, 0.1, 2.0, 0, 10, 50);  //Giroscopio
PID pidF(1, 0.0, 0.1, 7, 10, 50);     //Frontal


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

  float correction_angle = 0;
  float trajetoria = 30.0;
  bool passagens[4];

  /*! Verifica a existencia de passagens nas quatro direcoes*/
  void medir_passagens() {
    sensores.ler_dist();
    if (sensores.dist[0] >= 25) passagens[0] = true;
    if (sensores.dist[1] + sensores.dist[2] / 2 >= 25) passagens[1] = true;
    if (sensores.dist[3] >= 25) passagens[2] = true;
    if (sensores.dist[4] + sensores.dist[5] / 2 >= 25) passagens[3] = true;
  }

  /*! Estima o angulo atual com base em dois valores de distancia*/
  float angulo() {

    sensores.ler_dist();
    float df = sensores.dist[1],
          dt = sensores.dist[2],
          et = sensores.dist[4],
          ef = sensores.dist[5];
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

    //Filtro
    if (frente <= 45.0 && tras <= 45.0) {
      angulo = atan(cat_op / COMPRIMENTO_ROBO) * (180.0 / M_PI);
    } else {
      angulo = 0.0;
      Serial.println("Valores Invalidos para correcao");
    }

    //Correcao dependendo da angulacao
    if (frente > tras && lado == true) {
      angulo = angulo * -1.0;
    } else if (frente < tras && lado == false) {
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
  void girar(int vel, float ang) {
    int aux[] = { -vel, -vel, -vel, -vel };  // Inicia com valores de 'E'
    sensores.zerar_mpu();

    //Esquerda
    if (ang < 0) {
      while (sensores.angulo_mpu() > ang) {
        motores.potencia(aux);
      }
      //Direita
    } else if (ang >= 0) {
      aux[0] = vel;
      aux[1] = vel;
      aux[2] = vel;
      aux[3] = vel;
      while (sensores.angulo_mpu() < ang) {
        motores.potencia(aux);
        //sensores.calibrar_offset();
      }
    }
    parar();
    sensores.zerar_mpu();
  }

  /*! Para todos motores*/
  void parar() {
    for (int i = 0; i < 5; i++) motores.mesma_potencia(0);
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
  void setar_quadrado() {
    sensores.zerar_encoder();
    zerar_trajetoria_por_parede();
  }

  /*Um dos parametros da troca*/
  bool troca_encoder() {

    //Checa se foram passos suficientes
    Serial.println(sensores.passos_cm);
    if (sensores.passos_cm >= trajetoria) {

      sensores.zerar_encoder();
      return true;
    } else {
      return false;
    }
  }


  /*Busca uma alteração brusca entre as medidas de distancia, fornecendo um parametro mais preciso para o Encoder*/

  //Direita frontal e Esquerda frontal
  float last_dist[2] = { sensores.dist[1], sensores.dist[5] };
  bool limitador = false;  //Limita o numero de vezes em que zeramos o Encoder

  void trajetoria_por_parede() {

    //Sensores que devem ser atualizados
    int aux[4] = { 1, 5 };

    sensores.ler_dist_rapido(aux);


    Serial.print("Esq atual: ");
    Serial.print(sensores.dist[5]);
    Serial.print(" Esq Passado: ");
    Serial.print(last_dist[1]);
    Serial.print(" Dir atual: ");
    Serial.print(sensores.dist[1]);
    Serial.print(" Dir Passado: ");
    Serial.println(last_dist[0]);

    //Filtro lado direito
    if (sensores.dist[1] <= 50.0 && last_dist[0] <= 50.0) {
      if (abs(sensores.dist[1] - last_dist[0]) >= 28.0) {  //Procura diferença
        trajetoria = 20.0;
        if (limitador == false) {
          sensores.zerar_encoder();
          Serial.print(" DDDDDDDDDDDDDDDDDDDDDDDDDD ");
          limitador = true;
        }
      }
    }
    //Filtro lado Esquerdo
    if (sensores.dist[5] <= 50.0 && last_dist[1] <= 50.0) {
      if (abs(sensores.dist[5] - last_dist[1]) >= 28.0) {  //Procura diferença
        trajetoria = 20.0;
        if (limitador == false) {
          sensores.zerar_encoder();
          Serial.print(" EEEEEEEEEEEEEEEEEEEEEEEEEEEEE ");
          limitador = true;
        }
      }
    }

    //Salvamos para proximas iteracoes
    last_dist[0] = sensores.dist[1];
    last_dist[1] = sensores.dist[5];
  }

  /*Utilizada apos finalizar uma movimentacao, para previnir erros*/
  void zerar_trajetoria_por_parede() {

    //Sensores que devem ser atualizados
    int aux[4] = { 1, 5 };

    sensores.ler_dist_rapido(aux);

    last_dist[0] = sensores.dist[1];
    last_dist[1] = sensores.dist[5];
    limitador = false;
  }


  /* Verifica a troca de quadrado */
  bool troca_quadrado() {

    //Sensores que devem ser atualizados
    int aux[1] = { 0 };

    sensores.ler_dist_rapido(aux);

    // Troca pelas distancias
    if (sensores.dist[0] <= 7.5 && sensores.dist[0] != 0.0) {
      Serial.println("Troquei pela distancia");
      return true;

      // Troca pelo Encoder
    }
    if (troca_encoder() == true) {
      Serial.println("Troquei pelo Encoder");
      return true;

      // Nao houve troca
    } else {
      return false;
    }
  }

  /************************************ CORREÇÃO ******************************************/

  /*Correcao do angulo do robo a cada parada*/
  void correcao() {
    float ang = angulo();
    //Parte 1, alinha o robo com a parede
    //Ajuste para esquerda
    if (ang <= -2.0 && ang >= -20.0) {
      girar(200, ang);
    }
    //Ajuste para direita
    else if (ang >= 2.0 && ang <= 20.0) {
      girar(200, ang);
    }
  }

  /*Tendo a distancia nescessaria para atingir o centro do proximo quadrado,
   e o angulo que devemos manter para alcancala, esta funcao ajusta o robo no dado angulo, e atualiza a distancia
   nescessaria para a troca pelo encoder*/
  void correcao_trajetoria() {
    if (correction_angle < -2.0 && correction_angle > -30.0) girar(200, correction_angle);
    else if (correction_angle > 2.0 && correction_angle < 30.0) girar(200, correction_angle);
  }

  /*Cálculo do comprimento da nova trajetória */
  void calcular_trajetoria() {

    sensores.ler_dist();
    float cateto;
    trajetoria = 30.0;


    //Escolhemos a parede mas proxima para estimar a nova trajetoria, eaplicamos um filtro
    if (sensores.dist[1] <= sensores.dist[5] && sensores.dist[1] <= 45.0) {  //Próximo da Direita
      cateto = -(7 - fmod(sensores.dist[1], 30.0));
    }

    else if (sensores.dist[5] <= sensores.dist[1] && sensores.dist[5] <= 45.0) {  //Próximo da Esquerda
      cateto = (7 - fmod(sensores.dist[5], 30.0));
    }
    //Caso nao nescessite de correcao
    else {
      return;
    }

    //Nova distancia a ser percorrida (Deve ser passada para a troca)
    trajetoria = sqrt(pow(cateto, 2) + pow(30.0, 2));
    correction_angle = atan(cateto / 30.0) * (180.0 / M_PI);
    Serial.print("cateto: ");
    Serial.println(cateto);
    Serial.print("Trajetoria: ");
    Serial.println(trajetoria);
    Serial.print("Angulo estimado: ");
    Serial.println(correction_angle);
  }
  /********************** Servos*************************/
};
#endif