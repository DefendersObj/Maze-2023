#ifndef Motor_hpp
#define Motor_hpp

#include "Comunicacao.hpp"

Comunicacao com;

class Motor {

private:


public:

  /*! Funcao que coloca todos motores na mesma velocidade, e realiza seus espelhamentos*/
  void mesma_potencia(int vel, int dif_lado = 0) {
    //Caso normal da movimentacao
    int aux[4] = { (vel - dif_lado), -(vel + dif_lado), (vel - dif_lado) , -(vel + dif_lado)};
    potencia(aux);  //E enviado para os motores os valores de correcao
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
    //Comunica valores dos motores paara o Mega
    com.envio_motores(vel);
  }
};
#endif