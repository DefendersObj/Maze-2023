#ifndef Motor_hpp
#define Motor_hpp

#include "DynamixelMotor.h" /*!< Inclusão da biblioteca dos Motores // link: "https://github.com/descampsa/ardyno/tree/master/examples"*/


/*!< Esssa classe faz o controle de 4 motores Dynamixel.
    Ela fornece métodos para configurar os motores e a MPU, definir suas e velocidades.
    A classe utiliza a biblioteca "ardyno" para se comunicar com os motores Dynamixel.*/

// Velocidade da comunicacao com os motores
const long unsigned int baudrate = 1000000;

HardwareDynamixelInterface interface(Serial3);
DynamixelMotor motore1(interface, 11);  //define a porta do motor esquerdo 1
DynamixelMotor motore2(interface, 5);   //define a porta do motor esquerdo 2
DynamixelMotor motord1(interface, 1);   //define a porta do motor direito 1
DynamixelMotor motord2(interface, 6);   //define a porta do motor direito 2

class Motor {

private:


public:

  /*! Funcao que coloca todos motores na mesma velocidade, e realiza seus espelhamentos*/
  void mesma_potencia(int vel, int dif_lado = 0) {
    //Caso normal da movimentacao
    int aux[4] = { -(vel - dif_lado), -(vel - dif_lado), (vel + dif_lado), (vel + dif_lado) };
    potencia(aux);  //E enviado para os motores os valores de correcao
  }

  /*! Funcao que inicializa todas variaveis e itens nescesarrios para o funcionamento dos motores */
  void begin() {
    Serial3.begin(1000000);
    interface.begin(baudrate);
    bool error = false;

    while (!error) {
      // Verifica se foi estabelecida a conexao
      
      uint8_t status[4] = { motore1.init(),
                            motore2.init(),
                            motord1.init(),
                            motord2.init() };

      for (int i = 0; i < 4; i++) {
        if (status[i] != DYN_STATUS_OK) {
          Serial.print("Falha na conexao do motor: ");
          Serial.println(status[i]);
          error = true;
        }
      }
    }

    // Poe o Dynamixel no modo roda, e liga o torque
    motore1.enableTorque();
    motore2.enableTorque();
    motord1.enableTorque();
    motord2.enableTorque();
    motore1.wheelMode();
    motore2.wheelMode();
    motord1.wheelMode();
    motord2.wheelMode();
  }

  /*! Funcao que faz o envio das velocidade individualmente para cada motor*/
  void potencia(int vel[4]) {
    //Filtro para valores erroneos
    for (int i = 0; i < 4; i++) {
      if (vel[i] > 1000) {
        vel[i] = 1000;
      } else if (vel[i] < -1000) {
        vel[i] = -1000;
      }
    }


    //Cada valor recebe sua respectiva velocidade
    motore1.speed(vel[0]);
    motore2.speed(vel[1]);
    motord1.speed(vel[2]);
    motord2.speed(vel[3]);
  }
};
#endif