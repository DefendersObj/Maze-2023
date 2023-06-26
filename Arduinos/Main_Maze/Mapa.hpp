#ifndef Mapa_hpp
#define Mapa_hpp

//Tamanho do mapa
#define SIZE 3

//Cordenadas do robo
int x = 1,
    y = x,
    last_x,
    last_y,
    checkpoint_x = x,
    checkpoint_y = y;

class Mapa {

private:

  //Informacoes a serem recebidas
  char cor;
  char mov;
  int ori;
  bool passagens[4];

  unsigned char mapa[SIZE][SIZE] = { { 0 } };
  


public:

 
  
  /* Dada as cordenadas zeramos os bits 3 da cor e atualizamos com os valores novos por cima*/
  void set_color(int x, int y, char color) {
    switch (color){
      case 'w':
        color = 1; //1
      break;

      case 'b':
        color = 2;//10
      break;

      case 's':
        color = 3;//011
      break;

      case 'p':
        color = 4;//100
      break;
    }
    mapa[x][y] &= ~(0b01110000);//Bits utilizados para cor sao zerados
    mapa[x][y] |= (color<<4);
  }

  /*Dadas as cordenadas e a orientacao em relacao ao norte do mapa, recebemos as passagens
    do quadrado, zeramos qualquer passagem previamente salva, e alteramanos os bits das passagens*/
  void set_passages(int x, int y, char passages, byte orientation = 0){
    mapa[x][y] &= ~(0b00001111);//Bits das passagens sao zerados
    mapa[x][y] |= (((passages << orientation) |  (passages >> (4 - orientation))) & ~(0b11110000));//Adicionamos e invertemos de acordo com a orientacao
  }


  /* Atualiza os bits da posicao atual do mapa */
  void atualizacao(bool passagens[4], char cor) {

    bool bit_cor[3];

    //Adiciona os bits das passagens
    for (int i = 0; i < 4; i++) {
      mapa[x][y] |= (passagens[i] << (7 - i));
    }
    //Verifica e adiciona os bits da cor
    if (cor == 'w') {
      bit_cor[0] = false;
      bit_cor[1] = false;
      bit_cor[2] = true;
    } else if (cor == 'b') {
      bit_cor[0] = false;
      bit_cor[1] = true;
      bit_cor[2] = false;
    } else if (cor == 's') {
      bit_cor[0] = false;
      bit_cor[1] = true;
      bit_cor[2] = false;
    } else if (cor == 'p') {
      bit_cor[0] = true;
      bit_cor[1] = false;
      bit_cor[2] = false;
    }

    for (int i = 0; i < 3; i++) {
      mapa[x][y] |= (bit_cor[i] << (2 - i));
    }
  }

  /* Ajusta os valores recebidos das passagens, de acordo com a orientacao do robo */
  void orientacao(int ori) {
    bool aux;
    if (ori == 0) {
    }                     //Frente
    else if (ori == 1) {  //Direita
      aux = passagens[0];
      passagens[0] = passagens[3];
      passagens[3] = passagens[2];
      passagens[2] = passagens[1];
      passagens[1] = aux;
    } else if (ori == 2) {  //Tras
      aux = passagens[0];
      passagens[0] = passagens[2];
      passagens[2] = aux;

      aux = passagens[1];
      passagens[1] = passagens[3];
      passagens[3] = aux;
    } else if (ori == 3) {  //Esquerda
      aux = passagens[0];
      passagens[0] = passagens[1];
      passagens[1] = passagens[2];
      passagens[2] = passagens[3];
      passagens[3] = aux;
    }
  }

  /*Movimentas as cordenadas atuais do robo
- 'F' move em 1 seguindo a orientacao
- buraco move para o ultimo quadrado visitado
- sequestro move para o ultimo checkpoint visitado*/
  void move_cordenada(bool buraco, bool sequestro) {
    if (buraco == true) {
      x = last_x;
      y = last_y;
    } else if (sequestro == true) {
      x = last_x;
      y = last_y;
    } else if (ori == 0 && mov == 'F') {
      x--;
    } else if (ori == 1 && mov == 'F') {
      y++;
    } else if (ori == 2 && mov == 'F') {
      x++;
    } else if (ori == 3 && mov == 'F') {
      x--;
    }
  }

  /*! Impressão completa do mapa atual*/
  void imprimir() {

    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        // Imprime em hexadecimal 
        Serial.print("[");
        Serial.print(mapa[i][j], HEX);
        Serial.print("] ");

        // Imp binary format
        Serial.print(String(mapa[i][j], BIN));
        Serial.print(" ");
      }
      Serial.println();  // Move to the next line after each row
    }
  }
};
#endif