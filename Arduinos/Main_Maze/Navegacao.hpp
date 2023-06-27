#ifndef Navegacao_hpp
#define Navegacao_hpp

/*! A classe Navegacao tem como funcao tomar as decisoes do robo dentro da pista*/

#include "Operacional.hpp"
#include "Mapa.hpp"

Operacional op;
Mapa mapa;

class Navegacao {

private:

  /*A pilha contém 100 linhas e 2 colunas, armazenando cordenadas X,Y*/
  uint8_t pilha[100][2];
  uint8_t _pilha_x, _pilha_y;
  unsigned int _pilha_control = 0; //Controle de qual posição da pilha estamos 

  /*Recebe as cordenadas x e y de um nó e adiciona na pilha*/
  void put_node(uint8_t i, uint8_t j) {
    pilha[_pilha_control][0] = i;
    pilha[_pilha_control][1] = j;
    _pilha_control++;
  }

  /*Retiramos o nó do topo da pilha para analise*/
  void get_node() {
    _pilha_control--;
    _pilha_x = pilha[_pilha_control][0];
    _pilha_y = pilha[_pilha_control][1];
  }

public:

  /*Cordenadas e orientação inicial*/
  uint8_t X = 0, Y = 0, orientation = 0;

  /*Atualiza o mapa*/
  void update_map() {
    sensores.ler_dist();
    byte no = 0;

    //ler cor
    //Atualiza a cor
    mapa.set_color(X, Y, 'w');

    uint8_t passagens = 0;

    //Procura por passagens nas quatros direções.
    //Caso o quadrado tenha mais de duas passagen salvamos suas coordenadas como nó
    //Norte
    if (sensores.dist[0] > 20.0) {
      passagens |= 0b00000001;
      no++;
    }
    //Leste
    if (sensores.dist[1] > 20.0) {
      passagens |= 0b00000010;
      no++;
    }
    //Sul
    if (1) {
      passagens |= 0b00000100;
      no++;
    }
    //Oeste
    if (sensores.dist[5] > 20.0) {
      passagens |= 0b00001000;
      no++;
    }
    if (no > 2) put_node(X, Y);

    //Setamos as passagens do quadrado no mapa
    mapa.set_passages(X, Y, passagens, orientation);
  }

  /*Estágio 1 da Decisão, neste estagio fazemos uma busca apenas nos limites do quadrado atual.
  Priorizando a movimentação em direção ao Norte do mapa e sentido horário, até que nao existam quadrados não visitados nas redondezas
  imediatas do robô*/
  char decisao() {
    //Puxamos as informações do quadrado atual
    mapa.get_info(X, Y);

    bool passagens[4];

    for (int i = 0; i < 4; i++) passagens[i] = mapa.Passages[i];

    //Norte
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
    //Leste
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

    //Sul
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

    //Oeste
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

    //Caso não tenha mais quadrados nas redondezas imediatas, retorna 'L'
    return 'L';
  }
};
#endif