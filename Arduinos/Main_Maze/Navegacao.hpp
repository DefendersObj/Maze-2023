#ifndef Navegacao_hpp
#define Navegacao_hpp

/*! A classe Navegacao tem como funcao tomar as decisoes do robo dentro da pista*/

#include "Operacional.hpp"
#include "Mapa.hpp"
#define START_POINT_X 22
#define START_POINT_Y 22

Operacional op;
Mapa mapa;

class Navegacao {

private:

  /*A pilha contém 100 linhas e 2 colunas, armazenando cordenadas X,Y*/
  uint8_t pilha[100][2];
  uint8_t _pilha_x, _pilha_y;
  unsigned int _pilha_control = 0;  //Controle de qual posição da pilha estamos
  uint8_t _next_x, _next_y;

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


  unsigned int _stack_control = 0;
  uint8_t _stack_x, _stack_y;
  uint8_t _stack[100][2];

  void set_stack(uint8_t i, uint8_t j) {
    _stack[_stack_control][0] = i;
    _stack[_stack_control][1] = j;
    _stack_control++;
  }

  void get_stack() {
    _stack_control--;
    _stack_x = _stack[_stack_control][0];
    _stack_y = _stack[_stack_control][1];
  }

  bool find_stack(uint8_t i, uint8_t j) {

    for (unsigned int passo; passo < _stack_control; passo++) {
      if (_stack[passo][0] == i && _stack[passo][1] == j) return true;
    }

    return false;
  }

  void dump_stack() {
    for (unsigned int passo; passo < _stack_control; passo++) {
      Serial.print("x: ");
      Serial.print(_stack[passo][0]);
      Serial.print(" y: ");
      Serial.println(_stack[passo][1]);
    }
  }

public:

  /*Cordenadas e orientação inicial*/
  uint8_t X = START_POINT_X, Y = START_POINT_Y, orientation = 0;

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

  /*Estágio 2 da Navegação*/
  void last_node() {
    while (1) {
      if (_pilha_control == 0) {
        _next_x = START_POINT_X;
        _next_y = START_POINT_Y;
        return;
      }

      get_node();

      mapa.get_info(_pilha_x, _pilha_y);

      bool passagens[4];
      for (int i = 0; i < 4; i++) passagens[i] = mapa.Passages[i];

      if (passagens[0]) {
        mapa.get_info(_pilha_x, _pilha_y + 1);
        if (mapa.Color == 'u') {
          break;
        }
      }
      //Leste
      if (passagens[1]) {
        mapa.get_info(_pilha_x + 1, _pilha_y);
        if (mapa.Color == 'u') {
          break;
        }
      }

      //Sul
      if (passagens[2]) {
        mapa.get_info(_pilha_x, _pilha_y - 1);
        if (mapa.Color == 'u') {
          break;
        }
      }

      //Oeste
      if (passagens[3]) {
        mapa.get_info(_pilha_x - 1, _pilha_y);
        if (mapa.Color == 'u') {
          break;
        }
      }
    }
    put_node(_pilha_x, _pilha_y);
    _next_x = _pilha_x;
    _next_y = _pilha_y;
    Serial.print("node x: ");
    Serial.print(_next_x);
    Serial.print(" node y: ");
    Serial.println(_next_y);
  }

  uint8_t calc_route(uint8_t x, uint8_t y) {
    mapa.get_info(x, y);

    bool passagens[4];
    for (int i = 0; i < 4; i++) passagens[i] = mapa.Passages[i];
    float dist[4];

    if (passagens[0]) {
      mapa.get_info(x, y + 1);
      if (mapa.Color != 'u' && mapa.Color != 'b') {
        dist[0] = sqrt(pow(_next_x - x, 2) + pow(_next_y - (y + 1), 2));
      }
    }
    //Leste
    if (passagens[1]) {
      mapa.get_info(x + 1, y);
      if (mapa.Color != 'u' && mapa.Color != 'b') {
        dist[1] = sqrt(pow(_next_x - (x + 1), 2) + pow(_next_y - y, 2));
      }
    }

    //Sul
    if (passagens[2]) {
      mapa.get_info(x, y - 1);
      if (mapa.Color != 'u' && mapa.Color != 'b') {
        dist[2] = sqrt(pow(_next_x - x, 2) + pow(_next_y - (y - 1), 2));
      }
    }

    //Oeste
    if (passagens[3]) {
      mapa.get_info(x - 1, y);
      if (mapa.Color != 'u' && mapa.Color != 'b') {
        dist[3] = sqrt(pow(_next_x - (x - 1), 2) + pow(_next_y - y, 2));
      }
    }
  }
};
#endif