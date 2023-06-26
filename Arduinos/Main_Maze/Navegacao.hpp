#ifndef Navegacao_hpp
#define Navegacao_hpp

/*! A classe Navegacao tem como funcao tomar as decisoes do robo dentro da pista*/

#include "Operacional.hpp"
#include "Mapa.hpp"

Operacional op;
Mapa mapa;

class Navegacao {

private:

  uint8_t pilha[100][2];
  uint8_t _pilha_x, _pilha_y;
  unsigned int _pilha_control = 0;

  void put_node(uint8_t i, uint8_t j) {
    pilha[_pilha_control][0] = i;
    pilha[_pilha_control][1] = j;
    _pilha_control++;
  }

  void get_node() {
    _pilha_control--;
    _pilha_x = pilha[_pilha_control][0];
    _pilha_y = pilha[_pilha_control][1];
  }

public:
  uint8_t X = 2, Y = 9, orientation = 0;

  void update_map() {
    sensores.ler_dist();
    byte no = 0;

    //ler cor
    mapa.set_color(X, Y, 'w');

    uint8_t passagens = 0;
    if (sensores.dist[0] > 20.0) {
      passagens |= 0b00000001;
      no++;
    }
    if (sensores.dist[1] > 20.0) {
      passagens |= 0b00000010;
      no++;
    }
    if (1) {
      passagens |= 0b00000100;
      no++;
    }
    if (sensores.dist[5] > 20.0) {
      passagens |= 0b00001000;
      no++;
    }
    if (no > 2) put_node(X, Y);

    mapa.set_passages(X, Y, passagens, orientation);
  }

  char decisao() {
    mapa.get_info(X, Y);

    bool passagens[4];

    for (int i = 0; i < 4; i++) passagens[i] = mapa.Passages[i];

    if (passagens[0]) {
      mapa.get_info(X, Y + 1);
      if (mapa.Color == 'u') {
        if (orientation == 0) {
          Y++;
          return 'F';
        }
        if (orientation == 1) {
          orientation = 0;
          return 'E';
        }
        if (orientation == 2) {
          orientation = 0;
          return 'T';
        }
        if (orientation == 3) {
          orientation = 0;
          return 'D';
        }
      }
    }

    if (passagens[1]) {
      mapa.get_info(X + 1, Y);
      if (mapa.Color == 'u') {
        if (orientation == 1) {
          X++;
          return 'F';
        }
        if (orientation == 0) {
          orientation = 1;
          return 'D';
        }
        if (orientation == 2) {
          orientation = 1;
          return 'E';
        }
        if (orientation == 3) {
          orientation = 1;
          return 'T';
        }
      }
    }

    if (passagens[2]) {
      mapa.get_info(X, Y - 1);
      if (mapa.Color == 'u') {
        if (orientation == 2) {
          Y--;
          return 'F';
        }
        if (orientation == 0) {
          orientation = 2;
          return 'T';
        }
        if (orientation == 1) {
          orientation = 2;
          return 'D';
        }
        if (orientation == 3) {
          orientation = 2;
          return 'E';
        }
      }
    }

    if (passagens[3]) {
      mapa.get_info(X - 1, Y);
      if (mapa.Color == 'u') {
        if (orientation == 3) {
          X--;
          return 'F';
        }
        if (orientation == 0) {
          orientation = 3;
          return 'E';
        }
        if (orientation == 1) {
          orientation = 3;
          return 'T';
        }
        if (orientation == 2) {
          orientation = 3;
          return 'D';
        }
      }
    }

    return 'L';
  }
}
;
#endif