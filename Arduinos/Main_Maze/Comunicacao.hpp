#ifndef Comunicacao_hpp
#define Comunicacao_hpp


/*!<*/

class Comunicacao {

private:


public:

  /***************** ARDUINO MEGA ******************/
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
      while (!Serial3.available() && (millis() - to_timer) < 5)
        ;
      if ((millis() - to_timer) < 5) {
        if (Serial3.read() == 'k') break;
      }
    }
  }

  /********************* OpenMV H7 *************************/

  /*Protocolo que envia 1 lado e a OPenMV devolve se existem vitimas*/
  int camera(char lado) {

    String menssagem_cam;

    while (Serial2.available()) Serial2.read();  //Limpa a Serial


    /* lado = lado que a Camera está observano
      \n = marcador de fim de pacote*/
    char end_char = '\n';
    char menssagem[] = { lado, end_char };
    Serial2.print(menssagem);
    //Serial.print(menssagem);
    

    //Recebe a quantidade de kits via Serial2
    while (1) {
      if (Serial2.available()) {
        menssagem_cam = Serial2.readStringUntil('\n');
        break;
      } else {
        delay(100);
        Serial2.print(menssagem);
        Serial.println("Esperando Resposta");
      }
    }

    Serial.print("Resposta: ");
    Serial.println(menssagem_cam[0]);

    switch (menssagem_cam[0]) {
      case '0':  //Vítimas mas 0 kits
        Serial.println('0');
        delay(10000);
        return 0;
        break;
      case '1':  //Vítima com 1 kits
        Serial.println('1');
        delay(10000);
        return 1;
        break;
      case '2':  //Vítima com 2 kits
        Serial.println('2');
        delay(10000);
        return 2;
        break;
      case '3':  //Vítima com 3 kits
        Serial.println('3');
        delay(10000);
        return 3;
        break;
      default:  //Sem vítimas
        Serial.println("No victim");
        delay(500);
        return 9;
        break;
    }
  }
};
#endif