#ifndef Comunicacao_hpp
#define Comunicacao_hpp


/*!<*/

class Comunicacao {

private:


public:

  /*!Envia as potencias para os motores*/
  void envio_motores(int pot_int[4]) {
    while (Serial3.available()) Serial3.read();  //Limpa a Serial
    while (1) {
      Serial3.print('i');  //Marcador do inicio de pacote

      for (byte i = 0; i < 4; i++) {
        Serial3.println(pot_int[i]);
        //Serial.println(pot_int[i]);
      }

      Serial3.print('p');  //Marcador do fim do pacote

      /*! Espera echo do arudino mega, caso nao receba repetimos o enviuo*/
      unsigned long to_timer = millis();
      while (!Serial3.available() && (millis() - to_timer) < 2)
        ;
      if ((millis() - to_timer) < 5) {
        if (Serial3.read() == 'k') break;
      }
    }
  }

  /********************* OpenMV H7 *************************/

  /*Envia o lado que a camera está olhando*/
  void envia_lado(char lado) {
    char starter_char = '#',
         end_char = '\n';
    char menssagem[] = { starter_char, lado, end_char };
    Serial2.print(menssagem);
  }

  /* Lê a serial e manda um echo para a camera */
  String recebe_menssagem() {
    String menssagem = Serial2.readStringUntil('\n');
    Serial2.print(menssagem + "\n");
    return menssagem;
  }

  /* Retorna o numero de kits necessarios para as vitimas e caso não ache vitimas manda 9 */
  int process_menssagem(String menssagem) {
    if (menssagem[0] == '#') {
      switch (menssagem[1]) {
        case '0':
          Serial.println('0');
          return 0;
          break;
        case '1':
          Serial.println('1');
          return 1;
          break;
        case '2':
          Serial.println('2');
          return 2;
          break;
        case '3':
          Serial.println('3');
          return 3;
          break;
        default:
          Serial.println("No victim");
          return 9;
          break;
      }
    }
  }

  /* void loop() {

    // APENAS PARA TESTES RECEBE MENSAGEM VIA SERIAL0
    if (Serial.available()) {
      char side = Serial.read();
      send_side(side);
    }
    // LÊ A SERIAL DA CAMERA
    if (Serial2.available()) {
      process_menssagem(receive_message());
    }
  } */
};
#endif