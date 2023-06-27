#ifndef Mapa_hpp
#define Mapa_hpp

//Tamanho do mapa
#define SIZE 10

//Cordenadas do robo


class Mapa {

private:


  unsigned char mapa[SIZE][SIZE] = { { 0 } };


public:

  char Color;
  bool Passages[4];


  /* Dada as cordenadas zeramos os bits 3 da cor e atualizamos com os valores novos por cima*/
  void set_color(int x, int y, char color) {
    switch (color) {
      case 'w':
        color = 1;  //1
        break;

      case 'b':
        color = 2;  //10
        break;

      case 's':
        color = 3;  //011
        break;

      case 'p':
        color = 4;  //100
        break;
    }
    mapa[x][y] &= ~(0b01110000);  //Bits utilizados para cor sao zerados
    mapa[x][y] |= (color << 4);   //Adicionamos os bits novos
  }

  /*Dadas as cordenadas e a orientacao em relacao ao norte do mapa, recebemos as passagens
    do quadrado, zeramos qualquer passagem previamente salva, e alteramanos os bits das passagens*/
  void set_passages(int x, int y, char passages, byte orientation = 0) {
    mapa[x][y] &= ~(0b00001111);                                                                    //Bits das passagens sao zerados
    mapa[x][y] |= (((passages << orientation) | (passages >> (4 - orientation))) & ~(0b11110000));  //Adicionamos e corrigimos de acordo com a orientacao
  }

  /*Recebemos as cordenadas x e y, e atualizamos as variaveis Color, com a cor do quadrado
  e a Passages[4] com as passagens registradas (As passagens ja estão em relação ao Norte)*/
  void get_info(int x, int y) {
    uint8_t info = mapa[x][y];

    switch ((info >> 4) & 0b00000111) {
      case 0:
        Color = 'u';
        break;

      case 1:
        Color = 'w';  //1
        break;

      case 2:
        Color = 'b';  //10
        break;

      case 3:
        Color = 's';  //011
        break;

      case 4:
        Color = 'p';  //100
        break;
    }

    //Verifica e atualiza cada uma das passagens 
    //Norte
    if (info & 0b00000001) {
      Passages[0] = true;
    } else {
      Passages[0] = false;
    }
    //Leste
    if (info & 0b00000010) {
      Passages[1] = true;
    } else {
      Passages[1] = false;
    }
    //Sul
    if (info & 0b00000100) {
      Passages[2] = true;
    } else {
      Passages[2] = false;
    }
    //Oeste
    if (info & 0b00001000) {
      Passages[3] = true;
    } else {
      Passages[3] = false;
    }
  }


  /*! Impressão completa do mapa atual*/
  void imprimir() {

    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        Serial.print(mapa[j][i], BIN); //Imprime em binario (Apenas os bits relevantes)
        Serial.print(" ");
      }
      Serial.println();  //Quebra a linhha
    }
  }
};
#endif