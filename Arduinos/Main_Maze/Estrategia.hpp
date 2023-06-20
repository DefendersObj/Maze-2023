#ifndef Estrategia_hpp
#define Estrategia_hpp

/*! A classe Estrategia tem como funcao juntar e abstrair, ainda mais, o conteudo da classe "Operacional".
As funcoes dessa classe são as UNICAS que devem ser utilizadas no main*/

#include "Operacional.hpp"
#include "Mapa.hpp"

Operacional op;
Mapa mapa;

class Estrategia {

private:

public:

  /*! Realiza todas inicializações*/
  void iniciar() {
    op.begin();
  }


  /*! Movimentamos 1 quadrado para Frente */
  void frente(int ori, bool busca) {

    //Cordenadas de inicio
    //mapa.save_cord();

    //Ajusta o robo no próprio eixo
    op.correcao();
    op.calcular_trajetoria();
    op.correcao_trajetoria();

    //Parametros para troca
    //op.setar_quadrado(op.dist[0], op.dist[3]);

    op.zerar_mpu();
    sensores.zerar_encoder();
    //Loop ate a troca de quadrado
    //while(1){
      //Serial.println(sensores.passos);
    //}
    while (op.troca_encoder() == false) {
      //op.ler_distancias();
      //sensores.passos = 0;
      op.movimento(500);
      //Serial.println(sensores.passos_cm);
    }

    op.parar();
    //op.medir_passagens();
    //mapa.recebe_passagens_cor(op.passagens, op.cor());
    //mapa.orientacao(ori);
    //mapa.move_cordenada(false, false);
    //mapa.imprimir();
  }

  /*! Giramos para Esq ou Dir */
  void giro(char com) {
    op.girar(com);
  }

  /*! Busca vitimas com a camera*/
  void buscar_vit() {
  }
};
#endif